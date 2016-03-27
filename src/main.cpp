#include <string>
#include <map>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui/imguiRenderGL3.h"

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <glog/logging.h>
#include <callbacks/CallbacksManager.hpp>
#include <glm/gtc/random.hpp>


#include "geometry/Spline.h"

#include "graphics/ShaderProgram.hpp"
#include "graphics/Texture.h"
#include "graphics/TextureHandler.h"
#include "graphics/VertexDescriptor.h"
#include "graphics/VertexBufferObject.h"
#include "graphics/VertexArrayObject.h"
#include "graphics/UBO.hpp"
#include "graphics/Mesh.h"
#include "graphics/UBO_keys.hpp"
#include "graphics/ShadowMapFBO.hpp"
#include "graphics/GeometricFBO.hpp"
#include "graphics/BeautyFBO.hpp"
#include "graphics/PostFxFBO.hpp"
#include "graphics/ModelMeshGroup.hpp"
#include "graphics/ModelMeshInstanced.hpp"
#include "graphics/Scene.h"
#include "graphics/DebugBoundingBoxes.hpp"
#include "graphics/DebugDrawer.h"
#include "graphics/CubeMapTexture.hpp"
#include "graphics/Skybox.hpp"

#include "gui/Gui.hpp"
#include "gui/ObjectPicker.h"
#include "gui/SplinePicker.hpp"

#include "lights/Light.hpp"

#include "view/CameraFreefly.hpp"
#include "view/CameraController.hpp"

#include "gui/UserInput.hpp"
#include "utils/utils.h"

#include "data/SceneIOJson.hpp"
#include "data/UniformCamera.hpp"

#include "callbacks/CallbacksManager.hpp"



#define IMGUI_DRAW 1

// Font buffers IMGUI
extern const unsigned char DroidSans_ttf[];
extern const unsigned int DroidSans_ttf_len;

int main( int argc, char **argv ) {


    // GLOG debug level, 0 == all
    FLAGS_minloglevel = 0;

    int DPI;
    GLFWwindow * window = nullptr;
    bool displayGui = true;
    bool keypressedDrawGui = true;
    // glm::ivec2 dimViewport(1280, 720);
    glm::ivec2 dimViewport(1280, 760);
    int& width = dimViewport.x, height = dimViewport.y;
    float fps = 0.f;

    // Init glfw, GL and IMGUI
    initContextWindowGL("luminolGL", window, DPI, dimViewport);
    glViewport( 0, 0, width, height );

    if (!imguiRenderGLInit(DroidSans_ttf, DroidSans_ttf_len))
        throw std::runtime_error("Could not init GUI renderer");

    // GUI
    int guiExpandWidth = 400, guiExpandHeight = 550;
    int guiMinimalWidth = 400, guiMinimalHeight = 80;
    GUI::UserInput userInput;
    Gui::ObjectPicker picker;
    Gui::Gui gui(DPI, width, height, guiExpandWidth, guiExpandHeight, "LuminoGL");
    bool castShadowKeyPressed = false;


//    DLOG(INFO) << "creating meshgrid...";
//    DLOG(INFO) << "creating texture...";
//    Graphics::Texture gridTex("../assets/models/ground/ground03_height.tga");
//    DLOG(INFO) << "generating meshgrid...";
//    Graphics::Mesh grid = Graphics::Mesh::genGrid(7, 7, &gridTex, glm::vec3(1), 0.1f, 7);
//    grid.saveOBJ("../assets/models/ground/", "ground03", false);
//    DLOG(INFO) << "meshgrid created !";
//    return 0;

    // SHADERS
    Graphics::ShaderProgram mainShader("../shaders/aogl.vert", "", "../shaders/aogl.frag");
    Graphics::ShaderProgram blitShader("../shaders/blit.vert", "", "../shaders/blit.frag");
    Graphics::ShaderProgram pointLightShader(blitShader.vShader(), "../shaders/pointLight.frag");
    Graphics::ShaderProgram ssaoMixBeautyShader(blitShader.vShader(), "../shaders/ssaoMixBeauty.frag");
    Graphics::ShaderProgram fireflyShader(blitShader.vShader(), "../shaders/firefly.frag");
    Graphics::ShaderProgram directionalLightShader(blitShader.vShader(), "../shaders/directionnalLight.frag");
    Graphics::ShaderProgram spotLightShader(blitShader.vShader(), "../shaders/spotLight.frag");
    Graphics::ShaderProgram debugShapesShader("../shaders/debug.vert", "", "../shaders/debug.frag");
    Graphics::ShaderProgram shadowShader("../shaders/shadow.vert", "", "../shaders/shadow.frag");
    Graphics::ShaderProgram gammaShader(blitShader.vShader(), "../shaders/gammaCorrection.frag");
    Graphics::ShaderProgram sobelShader(blitShader.vShader(), "../shaders/sobel.frag");
    Graphics::ShaderProgram blurShader(blitShader.vShader(), "../shaders/blur.frag");
    Graphics::ShaderProgram circleConfusionShader(blitShader.vShader(), "../shaders/coc.frag");
    Graphics::ShaderProgram depthOfFieldShader(blitShader.vShader(), "../shaders/dof.frag");
    Graphics::ShaderProgram cameraMotionBlurShader(blitShader.vShader(), "../shaders/cameraMotionBlur.frag");
    Graphics::ShaderProgram ssaoShader(blitShader.vShader(), "../shaders/ssao.frag");
    Graphics::ShaderProgram waterReflectionShader("../shaders/waterReflectionRefraction.vert", mainShader.fShader());
    Graphics::ShaderProgram waterRenderShader(mainShader.vShader(), "../shaders/water.frag");
    Graphics::ShaderProgram ambientShader(blitShader.vShader(), "../shaders/ambient.frag");
    Graphics::ShaderProgram fogShader(blitShader.vShader(), "../shaders/fog.frag");


    const std::string scenePath             = "../assets/luminolGL.json";
    const std::string splineCamPositions    = "../assets/camPos.txt";
    const std::string splineCamTargets      = "../assets/camTargets.txt";
    const std::string splineCamSpeeds       = "../assets/camSpeeds.txt";

    // Camera config
    View::CameraFreefly camera(glm::vec2(width, height), glm::vec2(0.01, 1000.f));
    camera.setEye(glm::vec3(10,10,-10));

    // Camera splines config
    View::CameraController cameraController(camera, userInput, 0.05);
    try{
        cameraController.positions().load(splineCamPositions);
        cameraController.viewTargets().load(splineCamTargets);
        cameraController.speeds().load(splineCamSpeeds);
    }
    catch(std::exception& e){
        DLOG(WARNING) << e.what();
    }



    // Camera link with spline picker
    Gui::SplinePicker splinePicker(cameraController.positions(), cameraController.viewTargets(), cameraController.speeds());


    // Create Quad for FBO -------------------------------------------------------------------------------------------------------------------------------
    int   quad_triangleCount = 2;

    std::vector<glm::vec2> quadVertices = {
        glm::vec2(-1.0, -1.0),
        glm::vec2(1.0, -1.0),
        glm::vec2(-1.0, 1.0),
        glm::vec2(1.0, 1.0)
    };

    std::vector<int> quadIds = {0, 1, 2, 2, 1, 3};

    Graphics::VertexBufferObject quadVerticesVbo(Graphics::DataType::VEC2);
    Graphics::VertexBufferObject quadIdsVbo(Graphics::DataType::ELEMENT_ARRAY_BUFFER);

    Graphics::VertexArrayObject quadVAO;
    quadVAO.addVBO(&quadVerticesVbo);
    quadVAO.addVBO(&quadIdsVbo);
    quadVAO.init();

    quadVerticesVbo.updateData(quadVertices);
    quadIdsVbo.updateData(quadIds);

    // unbind everything
    Graphics::VertexArrayObject::unbindAll();
    Graphics::VertexBufferObject::unbindAll();

    // Create Scene -------------------------------------------------------------------------------------------------------------------------------
    Graphics::Skybox skybox(Graphics::CubeMapTexture("../assets/textures/skyboxes/ocean", {}, ".jpg"));

    Data::SceneIOJson sceneIOJson;
    Graphics::Scene scene(&sceneIOJson, scenePath);

    Callbacks::CallbacksManager::init(window, &scene, &picker);
    picker.attachToScene(&scene);

    Graphics::DebugBoundingBoxes debugScene(scene.meshInstances());
    scene.setFar(camera.getNearFar().y);

    checkErrorGL("Scene");


    // My Lights / Fireflies -------------------------------------------------------------------------------------------------------------------------------
    Light::LightHandler lightHandler;
    lightHandler.setDirectionalLight(glm::vec3(0.818, -0.862, -1), glm::vec3(1), 1);

    glm::vec3 ambient(1);
    float ambientIntensity = 0.2;
    float multIntensity = 0.5;

    ////////////// Sun ---- Point Light 
    lightHandler.addPointLight(glm::vec3(-15000, 12000, 15000), glm::vec3(24.5, 18.5, 0.05), 0.8, 2.0, Light::SUN);

    ////////////// Firefly fixe---- 
    // lightHandler.addPointLight(glm::vec3(-4.3, 9.5f, -7), glm::vec3(0.9, 0.2, 0.6), 0.35, 2.0, Light::FIXE);
    
    ////////////// Tornado Fireflies ---- // fd, rayon, int step, NB_TORNADO_FIREFLIES, multCounterCircle, w 
    // lightHandler.createFirefliesTornado(10, 1, 1, 800, 5, 1);

    ////////////// Rising Fireflies ---- // NB_RISING_FIREFLIES, width, profondeur, height, center 
    lightHandler.createRisingFireflies(50, 10, 10, 160, glm::vec3(20,0,20)); 
    
    ////////////// Random Displacement Fireflies ---- // NB_RANDOM_FIREFLIES, width, profondeur, height, center 
    lightHandler.createRandomFireflies(50, 50, 50, 40, glm::vec3(-150,20,50));

    
    ////////////// Random Displacement Fireflies ---- Point Light // NB_RANDOM_FIREFLIES, width, profondeur, height 
//    lightHandler.createRandomFireflies(500, 200, 200, 70);


    // ---------------------- For Geometry Shading
    float timeGLFW = 0;
    bool drawFBOTextures    = true;
    int isNormalMapActive   = 0;

    mainShader.updateUniform(Graphics::UBO_keys::DIFFUSE, 0);
    mainShader.updateUniform(Graphics::UBO_keys::SPECULAR, 1);
    mainShader.updateUniform(Graphics::UBO_keys::NORMAL_MAP, 2);
    mainShader.updateUniform(Graphics::UBO_keys::NORMAL_MAP_ACTIVE, isNormalMapActive);
    checkErrorGL("Uniforms");

    // ---------------------- For Light Pass Shading
    directionalLightShader.updateUniform(Graphics::UBO_keys::COLOR_BUFFER, 0);
    directionalLightShader.updateUniform(Graphics::UBO_keys::NORMAL_BUFFER, 1);
    directionalLightShader.updateUniform(Graphics::UBO_keys::DEPTH_BUFFER, 2);

    spotLightShader.updateUniform(Graphics::UBO_keys::COLOR_BUFFER, 0);
    spotLightShader.updateUniform(Graphics::UBO_keys::NORMAL_BUFFER, 1);
    spotLightShader.updateUniform(Graphics::UBO_keys::DEPTH_BUFFER, 2);

    fireflyShader.updateUniform(Graphics::UBO_keys::DEPTH_BUFFER, 2);

    ssaoMixBeautyShader.updateUniform(Graphics::UBO_keys::BEAUTY_BUFFER, 0);
    ssaoMixBeautyShader.updateUniform(Graphics::UBO_keys::SSAO_BUFFER, 1);

    ambientShader.updateUniform(Graphics::UBO_keys::COLOR_BUFFER, 0);
    ambientShader.updateUniform(Graphics::UBO_keys::BEAUTY_BUFFER, 1);

    checkErrorGL("Uniforms");

    // ---------------------- FX Variables
    float shadowBias            = 0.00019;
    float shadowBiasDirLight    = 0.001571;

    float gamma                 = 1.22;
    float sobelIntensity        = 0.05;
    float sampleCount           = 1; // blur
    float motionBlurSampleCount = 8; // motion blur
    float dirLightOrthoProjectionDim = 200;
    glm::vec3 focus(0, 1, 100);

    // ---------------------- FX uniform update
    // For shadow pass shading (unit texture)
    spotLightShader.updateUniform(Graphics::UBO_keys::SHADOW_BUFFER, 3);
    directionalLightShader.updateUniform(Graphics::UBO_keys::SHADOW_BUFFER, 3);

    gammaShader.updateUniform(Graphics::UBO_keys::GAMMA, gamma);
    sobelShader.updateUniform(Graphics::UBO_keys::SOBEL_INTENSITY, sobelIntensity);
    blurShader.updateUniform(Graphics::UBO_keys::BLUR_SAMPLE_COUNT, (int)sampleCount);
    blurShader.updateUniform(Graphics::UBO_keys::BLUR_DIRECTION, glm::ivec2(1,0));

    // ---------------------- For coc Correction
    circleConfusionShader.updateUniform(Graphics::UBO_keys::FOCUS, focus);

    // ---------------------- For dof Correction
    depthOfFieldShader.updateUniform(Graphics::UBO_keys::DOF_COLOR, 0);
    depthOfFieldShader.updateUniform(Graphics::UBO_keys::DOF_COC, 1);
    depthOfFieldShader.updateUniform(Graphics::UBO_keys::DOF_BLUR, 2);

    // ---------------------- For camera motion blur
    cameraMotionBlurShader.updateUniform(Graphics::UBO_keys::MOTION_BLUR_COLOR, 0);
    cameraMotionBlurShader.updateUniform(Graphics::UBO_keys::MOTION_BLUR_DEPTH, 1);

    // ---------------------- For ssao

    ssaoShader.updateUniform(Graphics::UBO_keys::SSAO_POSITION_DEPTH, 0);
    ssaoShader.updateUniform(Graphics::UBO_keys::SSAO_DEPTH, 1);
    ssaoShader.updateUniform(Graphics::UBO_keys::SSAO_NORMAL, 2);
    ssaoShader.updateUniform(Graphics::UBO_keys::SSAO_NOISE, 3);
    ssaoShader.updateUniform(Graphics::UBO_keys::SSAO_SCREEN_DIM, glm::vec2(width, height));

    checkErrorGL("Uniforms - post_fx Uniforms");

    // My FBO -------------------------------------------------------------------------------------------------------------------------------
    Graphics::GeometricFBO gBufferFBO(dimViewport);
    Graphics::ShadowMapFBO shadowMapFBO(glm::ivec2(2048));
    Graphics::BeautyFBO beautyFBO(dimViewport);
    Graphics::PostFxFBO fxFBO(dimViewport, 5);

    float shadowPoissonSampleCount = 1, shadowPoissonSpread = 1;

    // Create UBOs For Light & Cam -------------------------------------------------------------------------------------------------------------------------------
    const GLuint LightBindingPoint = 0;
    const GLuint CameraBindingPoint = 1;

    Graphics::UBO uboLight(LightBindingPoint, sizeof(Light::SpotLight));
    Graphics::UBO uboCamera(CameraBindingPoint, sizeof(Data::UniformCamera));

    // LIGHT
    fireflyShader.updateBindingPointUBO(Graphics::UBO_keys::STRUCT_BINDING_POINT_LIGHT, uboLight.bindingPoint());
    directionalLightShader.updateBindingPointUBO(Graphics::UBO_keys::STRUCT_BINDING_POINT_LIGHT, uboLight.bindingPoint());
    spotLightShader.updateBindingPointUBO(Graphics::UBO_keys::STRUCT_BINDING_POINT_LIGHT, uboLight.bindingPoint());

    // CAM
    fireflyShader.updateBindingPointUBO(Graphics::UBO_keys::STRUCT_BINDING_POINT_CAMERA, uboCamera.bindingPoint());
    directionalLightShader.updateBindingPointUBO(Graphics::UBO_keys::STRUCT_BINDING_POINT_CAMERA, uboCamera.bindingPoint());
    spotLightShader.updateBindingPointUBO(Graphics::UBO_keys::STRUCT_BINDING_POINT_CAMERA, uboCamera.bindingPoint());

    // Samples for SSAO ------------------------------------------------------------------------------

    int kernelSize = 32;

    // random floats between 0.0 - 1.0
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    for (int i = 0; i < kernelSize; ++i)
    {
        glm::vec3 sample(
                randomFloats(generator) * 2.0f - 1.0f,
                randomFloats(generator) * 2.0f - 1.0f,
                randomFloats(generator)
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.f;
        scale = glm::lerp<float>(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    int noiseSizeX = 4;
    int noiseSizeY = 4;

    std::vector<glm::vec3> ssaoNoise;
    for (int i = 0; i < noiseSizeX * noiseSizeY; i++){
        glm::vec3 noise(
                randomFloats(generator) * 2.0 - 1.0,
                randomFloats(generator) * 2.0 - 1.0,
                0.0f);
        ssaoNoise.push_back(noise);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, noiseSizeX, noiseSizeY, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);

    Graphics::Texture ssaoNoiseTex(4, 4, Graphics::TexParams(GL_RGB16F, GL_RGB, GL_FLOAT, GL_REPEAT, GL_NEAREST));
    ssaoNoiseTex.updateData(ssaoNoise.data());
    ssaoShader.updateUniform(Graphics::UBO_keys::SSAO_SAMPLES, ssaoKernel);
    ssaoShader.updateUniform(Graphics::UBO_keys::SSAO_NOISE_SIZE, glm::vec2(noiseSizeX, noiseSizeY));

    float occlusionIntensity = 2.0;
    float occlusionRadius = 3.0;

    float ssaoBlur = 4.0f;

    //Water--------------------------------------------------------------
    Graphics::GeometricFBO waterReflectionFBO(dimViewport);
    Graphics::PostFxFBO waterTextures(dimViewport, 2);
    Graphics::Texture waterNormals("../assets/textures/water/normals.jpg");
    float noiseAmplitudeWaves = 0.001f;
    float specularAmplitudeWaves = 0.001f;
    float fresnelBias = 0.001f;
    float fresnelAmplitude = 3;


    float scaleMeshTransform(1);
    glm::vec3 translateMeshTransform(0);


    bool drawSplines = true;
    bool isSplinePickerEnabled = false;

    float fogDensity = 1;
    glm::vec3 fogColor = glm::vec3(0.99, 0.91, 0.95);


    //*********************************************************************************************
    //***************************************** MAIN LOOP *****************************************
    //*********************************************************************************************

    // Identity matrix
    glm::mat4 objectToWorld;

    do {
        glm::mat4 previousMVP = camera.getProjectionMatrix() * camera.getViewMatrix();

        timeGLFW = static_cast<float>(glfwGetTime());

        userInput.update(window);
        cameraController.update(timeGLFW);

        //****************************************** UPLOAD UNIFORMS *******************************************

        //-------------------------------------Compute Matrices
        // Get camera matrices
        const glm::mat4& projection = camera.getProjectionMatrix();
        const glm::mat4& worldToView = camera.getViewMatrix();
        glm::mat4 mv            = worldToView * objectToWorld;
        glm::mat4 mvp           = projection * mv;
        glm::mat4 vp            = projection * worldToView;
        glm::mat4 mvInverse     = glm::inverse(mv);
        glm::mat4 mvNormal      = glm::transpose(mvInverse);
        glm::mat4 screenToView  = glm::inverse(projection);

        // For skybox
        glm::mat4 unTranslatedMV = glm::mat4(glm::mat3(worldToView));
        glm::mat4 screenToWorldUnTranslated = glm::inverse(unTranslatedMV) * screenToView;

        // For water
        glm::vec3 reflectedCamPos       = camera.getEye() * glm::vec3(1,-1,1) ;
        glm::vec3 reflectedFrontCam     = camera.getFront() * glm::vec3(1,-1,1);
        glm::mat4 reflectionViewMatrix  = glm::lookAt(reflectedCamPos, reflectedCamPos + reflectedFrontCam, glm::vec3(0,1,0));
        glm::mat4 reflectedVP           = projection * reflectionViewMatrix;
        glm::mat4 mvNormalReflected     = glm::transpose(glm::inverse(reflectionViewMatrix));

        glm::mat4 unTranslatedMV_reflected              = glm::mat4(glm::mat3(reflectionViewMatrix));
        glm::mat4 screenToWorldUnTranslated_reflected   = glm::inverse(unTranslatedMV_reflected) * screenToView;


        // Light space matrices
        // Directional light
        // Orthogonal projection matrix: parallel rays
        glm::mat4 projDirLight = glm::ortho<float>(-dirLightOrthoProjectionDim, dirLightOrthoProjectionDim,
                                                   -dirLightOrthoProjectionDim, dirLightOrthoProjectionDim,
                                                   -dirLightOrthoProjectionDim, dirLightOrthoProjectionDim);
        // From world to light
        // The "box" fallows the camera position
        glm::mat4 worldToDirLight = glm::lookAt(camera.getEye(),
                                                camera.getEye() + lightHandler._directionalLight._pos,
                                                glm::vec3(0.f, 1.f, 0.f));
        // From object to light (MV for light)
        glm::mat4 objectToDirLight = worldToDirLight /** objectToWorld*/;
        // From object to shadow map screen space (MVP for light)
        glm::mat4 objectToDirLightScreen = projDirLight * objectToDirLight;
        // From world to shadow map screen space
        glm::mat4 worldToDirLightScreen = projDirLight * worldToDirLight;

        //-------------------------------------Upload Uniforms
        mainShader.updateUniform(Graphics::UBO_keys::MVP, mvp);
        mainShader.updateUniform(Graphics::UBO_keys::MV, mv);
        mainShader.updateUniform(Graphics::UBO_keys::MV_NORMAL, mvNormal);
        mainShader.updateUniform(Graphics::UBO_keys::CAMERA_POSITION, camera.getEye());
        debugShapesShader.updateUniform(Graphics::UBO_keys::MVP, mvp);
        debugShapesShader.updateUniform(Graphics::UBO_keys::MV_INVERSE, mvInverse);

        mainShader.updateUniform(Graphics::UBO_keys::TIME, timeGLFW);
        mainShader.updateUniform(Graphics::UBO_keys::SPECULAR_POWER, lightHandler._specularPower);

        // SHADOW
        spotLightShader.updateUniform(Graphics::UBO_keys::SHADOW_BIAS, shadowBias);
        spotLightShader.updateUniform(Graphics::UBO_keys::SHADOW_POISSON_SAMPLE_COUNT, int(shadowPoissonSampleCount));
        spotLightShader.updateUniform(Graphics::UBO_keys::SHADOW_POISSON_SPREAD, shadowPoissonSpread);

        directionalLightShader.updateUniform(Graphics::UBO_keys::WORLD_TO_LIGHT_SCREEN, worldToDirLightScreen);
        directionalLightShader.updateUniform(Graphics::UBO_keys::SHADOW_BIAS, shadowBiasDirLight);
        directionalLightShader.updateUniform(Graphics::UBO_keys::SHADOW_POISSON_SAMPLE_COUNT, int(shadowPoissonSampleCount));
        directionalLightShader.updateUniform(Graphics::UBO_keys::SHADOW_POISSON_SPREAD, shadowPoissonSpread);
        directionalLightShader.updateUniform(Graphics::UBO_keys::MVP, mvp);

        fireflyShader.updateUniform(Graphics::UBO_keys::MVP, mvp);
        float windowratio = (float)width / (float)height;
        fireflyShader.updateUniform(Graphics::UBO_keys::WINDOW_RATIO, windowratio);
        fireflyShader.updateUniform(Graphics::UBO_keys::TIME, timeGLFW);
        fireflyShader.updateUniform(Graphics::UBO_keys::MULT_INTENSITY, multIntensity);


        ambientShader.updateUniform(Graphics::UBO_keys::AMBIENT_INTENSITY, ambient * ambientIntensity);

        // FX
        gammaShader.updateUniform(Graphics::UBO_keys::GAMMA, gamma);
        sobelShader.updateUniform(Graphics::UBO_keys::SOBEL_INTENSITY, sobelIntensity);
        blurShader.updateUniform(Graphics::UBO_keys::BLUR_SAMPLE_COUNT, (int)sampleCount);
        circleConfusionShader.updateUniform(Graphics::UBO_keys::SCREEN_TO_VIEW, screenToView);
        circleConfusionShader.updateUniform(Graphics::UBO_keys::FOCUS, focus);

        cameraMotionBlurShader.updateUniform(Graphics::UBO_keys::PREVIOUS_MVP, previousMVP);
        cameraMotionBlurShader.updateUniform(Graphics::UBO_keys::SCREEN_TO_VIEW, screenToView);
        cameraMotionBlurShader.updateUniform(Graphics::UBO_keys::MV_INVERSE, mvInverse);
        cameraMotionBlurShader.updateUniform(Graphics::UBO_keys::MOTION_BLUR_SAMPLE_COUNT, (int) motionBlurSampleCount);

        ssaoShader.updateUniform(Graphics::UBO_keys::SSAO_PROJECTION, projection);
        ssaoShader.updateUniform(Graphics::UBO_keys::SSAO_OCCLUSION_INTENSITY, occlusionIntensity);
        ssaoShader.updateUniform(Graphics::UBO_keys::SSAO_OCCLUSION_RADIUS, occlusionRadius);

        //****************************************** RENDER *******************************************

        // WATER
        waterReflectionShader.updateUniform(Graphics::UBO_keys::MVP, reflectedVP);
        waterReflectionShader.updateUniform(Graphics::UBO_keys::MV, reflectionViewMatrix);
        waterReflectionShader.updateUniform(Graphics::UBO_keys::MV_NORMAL, mvNormalReflected);
        waterReflectionShader.updateUniform(Graphics::UBO_keys::CAMERA_POSITION, reflectedCamPos);
        waterReflectionShader.updateUniform(Graphics::UBO_keys::WATER_IS_REFLECTION, 1);

        waterRenderShader.updateUniform(Graphics::UBO_keys::SPECULAR_POWER, lightHandler._specularPower);
        waterRenderShader.updateUniform(Graphics::UBO_keys::MVP, mvp);
        waterRenderShader.updateUniform(Graphics::UBO_keys::MV, mv);
        waterRenderShader.updateUniform(Graphics::UBO_keys::MV_NORMAL, mvNormal);
        waterRenderShader.updateUniform(Graphics::UBO_keys::CAMERA_POSITION, camera.getEye());
        waterRenderShader.updateUniform(Graphics::UBO_keys::TIME, timeGLFW);
        waterRenderShader.updateUniform(Graphics::UBO_keys::WATER_NOISE_AMPLITUDE, noiseAmplitudeWaves);
        waterRenderShader.updateUniform(Graphics::UBO_keys::WATER_SPECULAR_AMPLITUDE, specularAmplitudeWaves);
        waterRenderShader.updateUniform(Graphics::UBO_keys::NORMAL_MAP_ACTIVE, isNormalMapActive);
        waterRenderShader.updateUniform(Graphics::UBO_keys::WATER_LIGHT_RAY, lightHandler._directionalLight._pos);
        waterRenderShader.updateUniform(Graphics::UBO_keys::WATER_FRESNEL_AMPLITUDE, fresnelAmplitude);
        waterRenderShader.updateUniform(Graphics::UBO_keys::WATER_FRESNEL_BIAS, fresnelBias);

        //****************************************** RENDER *******************************************

        //******************************************************* FIRST PASS (Geometric pass)

        // Default states
        glEnable(GL_DEPTH_TEST);
        // Clear the front buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //----------------------- MAIN SCENE (Used also for water refraction) -------------
        // Render scene into Geometric buffer
        mainShader.useProgram();
        gBufferFBO.bind();
        gBufferFBO.clear();
        scene.updateCameraPosition(camera.getEye());
        scene.draw(vp);
        gBufferFBO.unbind();

        // ------- SSAO ------
        // Compute ssao from geometric buffer
        fxFBO.bind();
        fxFBO.changeCurrentTexture(0);
        fxFBO.clearColor();

        ssaoShader.useProgram();
        gBufferFBO.position().bind(GL_TEXTURE0);    // position
        gBufferFBO.depth().bind(GL_TEXTURE1);       // depth
        gBufferFBO.normal().bind(GL_TEXTURE2);      // normal
        ssaoNoiseTex.bind(GL_TEXTURE3);             // noise

        quadVAO.bind();
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

//        blur on ssao texture
        // Use blur program shader
        blurShader.useProgram();
        blurShader.updateUniform(Graphics::UBO_keys::BLUR_SAMPLE_COUNT, int(ssaoBlur));
        blurShader.updateUniform(Graphics::UBO_keys::BLUR_DIRECTION, glm::ivec2(1,0));
        blurShader.updateUniform(Graphics::UBO_keys::BLUR_SIGMA, 8.f);

        // Write into Vertical Blur Texture
        fxFBO.changeCurrentTexture(1);
        // Clear the content of texture
        fxFBO.clearColor();
        // Read the texture processed by the Sobel operator
        fxFBO.texture(0).bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        fxFBO.changeCurrentTexture(0);
        // Clear the content of texture
        fxFBO.clearColor();
        // Write into Horizontal Blur Texture
        blurShader.updateUniform(Graphics::UBO_keys::BLUR_DIRECTION, glm::ivec2(0,1));

        // Read the texture processed by the Vertical Blur
        fxFBO.texture(1).bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // We mix SSAO and diffuse color into gbuffer
        gBufferFBO.bind();
        quadVAO.bind();
        ssaoMixBeautyShader.useProgram();
        gBufferFBO.color().bind(GL_TEXTURE0);
        fxFBO.texture(0).bind(GL_TEXTURE1);

        // We need to disable depth writing
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        // We change the gBuffer FBO attachments to avoid color attachments overwriting
        gBufferFBO.setColorWritingOnly();
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // Fall back on default behavior
        gBufferFBO.setDefaultWriting();
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        gBufferFBO.unbind();


        //----------------------- WATER -------------
        //----------------------- REFLECTION SCENE -------------
        // Render scene for water (flipped Camera)
        waterReflectionShader.useProgram();
        glEnable(GL_CLIP_DISTANCE0);
        waterReflectionFBO.bind();
        waterReflectionFBO.clear();
        scene.draw(reflectedVP);
        waterReflectionFBO.unbind();
        glDisable(GL_CLIP_DISTANCE0);

        // ------- FOG ------
        fxFBO.bind();
        quadVAO.bind();
        fxFBO.changeCurrentTexture(1);
        fxFBO.clearColor();
        fogShader.useProgram();
        fogShader.updateUniform(Graphics::UBO_keys::FOG_DENSITY, fogDensity);
        fogShader.updateUniform(Graphics::UBO_keys::FOG_TEXTURE, 0);
        fogShader.updateUniform(Graphics::UBO_keys::FOG_DEPTH, 1);
        fogShader.updateUniform(Graphics::UBO_keys::FOG_COLOR, fogColor);
        fogShader.updateUniform(Graphics::UBO_keys::FOG_NEAR, camera.getNearFar().x);
        fogShader.updateUniform(Graphics::UBO_keys::FOG_FAR, camera.getNearFar().y);

        waterReflectionFBO.color().bind(GL_TEXTURE0);
        waterReflectionFBO.depth().bind(GL_TEXTURE1);
        glDisable(GL_DEPTH_TEST);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        glEnable(GL_DEPTH_TEST);
        fxFBO.unbind();


        //----------------------- REFLECTION SCENE + SKYBOX -------------
        // Render skybox texture combined with reflection: mask with depth buffer
        waterTextures.bind();
        waterTextures.changeCurrentTexture(0);
        waterTextures.clearColor();
        skybox.updateUniforms(screenToWorldUnTranslated_reflected, 0, 1, 2);
        skybox.useProgramShader();

        skybox.bindTexture(GL_TEXTURE0); // cubeMap
        waterReflectionFBO.depth().bind(GL_TEXTURE1);
        fxFBO.texture(1).bind(GL_TEXTURE2);

        quadVAO.bind();
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        waterTextures.unbind();


        //----------------------- REFLECTION - REFRACTION - Wave Animation into main scene (gbuffer) -------------
        mainShader.useProgram();
        waterRenderShader.updateUniform(Graphics::UBO_keys::DIFFUSE, 0);
        waterRenderShader.updateUniform(Graphics::UBO_keys::NORMAL_MAP, 1);
        waterRenderShader.updateUniform(Graphics::UBO_keys::WATER_REFRACTION_TEXTURE, 2);


        gBufferFBO.bind();
        waterRenderShader.useProgram();
        waterTextures.texture(0).bind(GL_TEXTURE0);
        waterNormals.bind(GL_TEXTURE1);
        gBufferFBO.color().bind(GL_TEXTURE2);
        scene.drawWater(reflectedVP);
        gBufferFBO.unbind();

        //******************************************************* SECOND PASS (Shadow Pass)
        // Update Camera pos and screenToWorld matrix to all light shaders
        Data::UniformCamera uCamera(camera.getEye(), glm::inverse(mvp), mvInverse);
        uboCamera.updateBuffer(&uCamera, sizeof(Data::UniformCamera));
        beautyFBO.bind();
        beautyFBO.clearColor();

        // ------------------------------------ Spot light Shadow pass
        for(auto& light : lightHandler._spotLights){
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);

            shadowMapFBO.bind();
            shadowMapFBO.clearDepth();
            // Set the viewport corresponding to shadow texture resolution
            glViewport(0, 0, shadowMapFBO.resolution().x, shadowMapFBO.resolution().y);

            // Spot Light
            // From light space to shadow map screen space
            glm::mat4 proj = glm::perspective(glm::radians(light._falloff*2.f), 1.0f, 0.1f, 100.f);
            glm::mat4 worldToLight = glm::lookAt(light._pos, light._pos + light._dir,glm::vec3(0.f, 1.f, 0.f));

            // From object to light (MV for light)
            glm::mat4 objectToLight = worldToLight /** objectToWorld*/;
            // From object to shadow map screen space (MVP for light)
            glm::mat4 objectToLightScreen = proj * objectToLight;
            // From world to shadow map screen space
            glm::mat4 worldToLightScreen = proj * worldToLight;

            // Render the scene
            shadowShader.updateUniform(Graphics::UBO_keys::SHADOW_MVP, objectToLightScreen);
            shadowShader.updateUniform(Graphics::UBO_keys::SHADOW_MV, objectToLight);
            shadowShader.useProgram();
            scene.draw(worldToLightScreen);
            shadowMapFBO.unbind();

            // ------------------------------------ Spot Lights Draw
            beautyFBO.bind();

            // Set a full screen viewport
            glViewport( 0, 0, width, height );
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            // Setup additive blending
            glBlendFunc(GL_ONE, GL_ONE);

            spotLightShader.useProgram();
            spotLightShader.updateUniform(Graphics::UBO_keys::WORLD_TO_LIGHT_SCREEN, worldToLightScreen);
            quadVAO.bind();

            fxFBO.texture(4).bind(GL_TEXTURE0);
            gBufferFBO.normal().bind(GL_TEXTURE1);
            gBufferFBO.depth().bind(GL_TEXTURE2);
            shadowMapFBO.shadowTexture().bind(GL_TEXTURE3);

            uboLight.updateBuffer(&light, sizeof(Light::SpotLight));
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        }

        //------------------------------------ Directional Lights Shadow pass
        // Fallback on shadow pass parameters
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        // Dir light Shadow pass
        directionalLightShader.updateUniform(Graphics::UBO_keys::SHADOW_BIAS, shadowBiasDirLight);
        shadowMapFBO.bind();
        shadowMapFBO.clearDepth();
        glViewport(0, 0, shadowMapFBO.resolution().x, shadowMapFBO.resolution().y);

        // Render the scene
        shadowShader.updateUniform(Graphics::UBO_keys::SHADOW_MVP, objectToDirLightScreen);
        shadowShader.updateUniform(Graphics::UBO_keys::SHADOW_MV, objectToDirLightScreen);
        shadowShader.useProgram();
        scene.draw(worldToDirLightScreen, false);
        shadowMapFBO.unbind();

        //-------------------------------------Light Draw
        beautyFBO.bind();
        glViewport( 0, 0, width, height);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        // Setup additive blending
        glBlendFunc(GL_ONE, GL_ONE);

        directionalLightShader.useProgram();
        quadVAO.bind();

        gBufferFBO.color().bind(GL_TEXTURE0);
        gBufferFBO.normal().bind(GL_TEXTURE1);
        gBufferFBO.depth().bind(GL_TEXTURE2);
        shadowMapFBO.shadowTexture().bind(GL_TEXTURE3);

        uboLight.updateBuffer(&lightHandler._directionalLight, sizeof(Light::DirectionalLight));
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);


        //------------------------------------- Post FX Draw

        // Fallback to default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Disable blending
        glDisable(GL_BLEND);
        glViewport( 0, 0, width, height );

        // Clear default framebuffer color buffer
        glClear(GL_COLOR_BUFFER_BIT);
        // Disable depth test
        glDisable(GL_DEPTH_TEST);


        // Set quad as vao: deferred
        fxFBO.bind();
        quadVAO.bind();

        // ------- FOG ------
        fogShader.useProgram();
        fogShader.updateUniform(Graphics::UBO_keys::FOG_DENSITY, fogDensity);
        fogShader.updateUniform(Graphics::UBO_keys::FOG_TEXTURE, 0);
        fogShader.updateUniform(Graphics::UBO_keys::FOG_DEPTH, 1);
        fogShader.updateUniform(Graphics::UBO_keys::FOG_COLOR, fogColor);
        fogShader.updateUniform(Graphics::UBO_keys::FOG_NEAR, camera.getNearFar().x);
        fogShader.updateUniform(Graphics::UBO_keys::FOG_FAR, camera.getNearFar().y);
        fogShader.updateUniform(Graphics::UBO_keys::FOG_DEPTH, 1);

        fxFBO.changeCurrentTexture(2);
        fxFBO.clearColor();

        beautyFBO.beauty().bind(GL_TEXTURE0);
        gBufferFBO.depth().bind(GL_TEXTURE1);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // ------- SKYBOX ------
        // Render skybox texture combined with beauty: mask with depth buffer
        // All in one pass
        fxFBO.changeCurrentTexture(3);
        fxFBO.clearColor();
        skybox.updateUniforms(screenToWorldUnTranslated, 0, 1, 2);
        skybox.useProgramShader();

        skybox.bindTexture(GL_TEXTURE0); // cubeMap
        gBufferFBO.depth().bind(GL_TEXTURE1);
        fxFBO.texture(2).bind(GL_TEXTURE2);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);


        // ------------------------------------ Fireflies
        fireflyShader.useProgram(); // point light shaders
        quadVAO.bind(); // Bind quad vao
        glEnable(GL_BLEND);

        gBufferFBO.color().bind(GL_TEXTURE0);
        gBufferFBO.normal().bind(GL_TEXTURE1);
        gBufferFBO.depth().bind(GL_TEXTURE2);
        // gBufferFBO.shadow().bind(GL_TEXTURE3);

        // Sun
        uboLight.updateBuffer(&lightHandler._pointLights[0], sizeof(Light::PointLight));
        fireflyShader.updateUniform(Graphics::UBO_keys::IS_SUN, true);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        fireflyShader.updateUniform(Graphics::UBO_keys::IS_SUN, false);
        int cptLights = 0;
        for(size_t i = 1; i < lightHandler._pointLights.size(); ++i){
            std::vector<glm::vec2> littleQuadVertices;
            if(lightHandler.isOnScreen(mvp, littleQuadVertices, lightHandler._pointLights[i]._pos, lightHandler._pointLights[i]._color, lightHandler._pointLights[i]._intensity, lightHandler._pointLights[i]._attenuation, lightHandler._pointLights[i]._type, timeGLFW)){
                
                if(lightHandler._pointLights[i]._type == 5 ) lightHandler._pointLights[i].computeRisingFireflies(timeGLFW);
                if(lightHandler._pointLights[i]._type == 2 ) lightHandler._pointLights[i].computeRandomFireflies(timeGLFW);
                
                //quad size reduction and frustum according to the light position, intensity, color and attenuation
                quadVerticesVbo.updateData(littleQuadVertices);
                quadIdsVbo.updateData(quadIds);
                
                cptLights++;

                uboLight.updateBuffer(&lightHandler._pointLights[i], sizeof(Light::PointLight));
                glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
            }
        }
        quadVerticesVbo.updateData(quadVertices);
        quadIdsVbo.updateData(quadIds);

        glDisable(GL_BLEND);

        // ------- AMBIENT LIGHT ------
        ambientShader.useProgram();

        fxFBO.changeCurrentTexture(1);
        fxFBO.clearColor();

        gBufferFBO.color().bind(GL_TEXTURE0); // color
        fxFBO.texture(3).bind(GL_TEXTURE1); // beauty

        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);


        // ------- SOBEL ------
        fxFBO.changeCurrentTexture(0);
        fxFBO.clearColor();

        sobelShader.useProgram();
        fxFBO.texture(1).bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);


        // ------- BLUR ------
        if(sampleCount > 0){
            // Use blur program shader
            blurShader.useProgram();
            blurShader.updateUniform(Graphics::UBO_keys::BLUR_DIRECTION, glm::ivec2(1,0));
            blurShader.updateUniform(Graphics::UBO_keys::BLUR_SIGMA, 1.f);

            // Write into Vertical Blur Texture
            fxFBO.changeCurrentTexture(1);
            // Clear the content of texture
            fxFBO.clearColor();
            // Read the texture processed by the Sobel operator
            fxFBO.texture(0).bind(GL_TEXTURE0);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

            // Write into Horizontal Blur Texture
            blurShader.updateUniform(Graphics::UBO_keys::BLUR_DIRECTION, glm::ivec2(0,1));
            fxFBO.changeCurrentTexture(2);
            // Clear the content of texture
            fxFBO.clearColor();
            // Read the texture processed by the Vertical Blur
            fxFBO.texture(1).bind(GL_TEXTURE0);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        }



        // ------- COC ------
        // Use circle of confusion program shader
        circleConfusionShader.useProgram();

        // Write into Circle of Confusion Texture
        fxFBO.changeCurrentTexture(1);
        // Clear the content of  texture
        fxFBO.clearColor();
        // Read the depth texture
        gBufferFBO.depth().bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);


        // ------- DOF ------
        // Attach Depth of Field texture to framebuffer
        fxFBO.changeCurrentTexture(3);
        // Only the color buffer is used
        fxFBO.clearColor();

        // Use the Depth of Field shader
        depthOfFieldShader.useProgram();
        fxFBO.texture(0).bind(GL_TEXTURE0); // Color
        fxFBO.texture(1).bind(GL_TEXTURE1); // CoC
        fxFBO.texture(2).bind(GL_TEXTURE2); // Blur
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);


        // ------- CAMERA MOTION BLUR ------
        fxFBO.changeCurrentTexture(1);
        cameraMotionBlurShader.useProgram();
        fxFBO.texture(3).bind(GL_TEXTURE0); // last pass
        gBufferFBO.depth().bind(GL_TEXTURE1); // depth
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        quadVAO.bind();


        // ------- GAMMA ------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        quadVAO.bind();
        gammaShader.useProgram();
        fxFBO.texture(1).bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        //------------------------------------ Debug Shape Drawing
        debugScene.draw(mvp);
        picker.drawPickedObject(debugShapesShader);

        if(drawSplines){
            Graphics::DebugDrawer::drawSpline(cameraController.positions(), cameraController.positions().size()*10, debugShapesShader, glm::vec3(1,0,0));
            Graphics::DebugDrawer::drawSpline(cameraController.viewTargets(), cameraController.positions().size()*10, debugShapesShader, glm::vec3(0,1,0));
        }

        if(drawFBOTextures){
            int screenNumber = 7;
            glDisable(GL_DEPTH_TEST);

            // Select shader
            blitShader.useProgram();

            // --------------- Color Buffer
            glViewport( 0, 0, width/screenNumber, height/screenNumber );

            quadVAO.bind();
            gBufferFBO.color().bind(GL_TEXTURE0);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

            // --------------- Normal Buffer
            glViewport( width/screenNumber, 0, width/screenNumber, height/screenNumber );

            quadVAO.bind();
            gBufferFBO.normal().bind(GL_TEXTURE0);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

            // --------------- Depth Buffer
            glViewport( 2*width/screenNumber, 0, width/screenNumber, height/screenNumber );

            quadVAO.bind();
            gBufferFBO.depth().bind(GL_TEXTURE0);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

            // --------------- Position Buffer
            glViewport( 3*width/screenNumber, 0, width/screenNumber, height/screenNumber );

            quadVAO.bind();
            gBufferFBO.position().bind(GL_TEXTURE0);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

            // --------------- Beauty Buffer
            glViewport( 4*width/screenNumber, 0, width/screenNumber, height/screenNumber );

            quadVAO.bind();
            beautyFBO.beauty().bind(GL_TEXTURE0);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

            // --------------- Circle of confusion Buffer
            glViewport( 5*width/screenNumber, 0, width/screenNumber, height/screenNumber );

            quadVAO.bind();
            fxFBO.texture(1).bind(GL_TEXTURE0);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

            // --------------- Blur Buffer
            glViewport( 6*width/screenNumber, 0, width/screenNumber, height/screenNumber );

            quadVAO.bind();
            gBufferFBO.depth().bind(GL_TEXTURE0);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        }

        //****************************************** EVENTS *******************************************

#ifdef IMGUI_DRAW
        // Draw UI
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0, 0, width, height);
        bool userClick = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS;

        gui.init(window);
        gui.updateMbut(userClick);

        if(!gui.isCursorInPanelIMGUI()){

            if(isSplinePickerEnabled)
                splinePicker.pick(gui.getCursorPosition(), camera, userClick);
            else
                picker.pickObject(gui.getCursorPosition(), gui.getCursorSpeed(), camera, userClick) ;
        }

        gui.displayMeshTransform = picker.isPicked();
        if(!picker.isPicked())
            translateMeshTransform = glm::vec3(0);

        if(glfwGetKey(window, GLFW_KEY_T)) picker.switchMode(Gui::PickerMode::TRANSLATION);
        if(glfwGetKey(window, GLFW_KEY_Y)) picker.switchMode(Gui::PickerMode::SCALE);
        if(glfwGetKey(window, GLFW_KEY_R)) picker.switchMode(Gui::PickerMode::ROTATION);


        if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !castShadowKeyPressed){
            picker.toggleShadow();
            castShadowKeyPressed = true;
        }
        if(glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && castShadowKeyPressed){
            castShadowKeyPressed = false;
        }


        if(glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !keypressedDrawGui){
            displayGui = !(displayGui);
            keypressedDrawGui = true;
        }
        if(glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE && keypressedDrawGui){
            keypressedDrawGui = false;
        }


        if(displayGui){

            gui.addLabel("FPS", &fps);
            gui.addLabel("Lights", cptLights);
            
            if(gui.addButton("Menu", gui.displayMenu)){
                gui.setWindowWidth(guiExpandWidth);
                gui.setWindowHeight(guiExpandHeight);
            }
            else{
                gui.setWindowWidth(guiMinimalWidth);
                gui.setWindowHeight(guiMinimalHeight);
            }


            if(gui.displayMenu){

                gui.addLabel("Lights", cptLights);
                gui.addSeparatorLine();
                gui.addIndent();

                if(gui.addButton("Camera switch"))
                    cameraController.setSpectator(!cameraController.isSpectator());
                if(gui.addButton("IsNormalMapActive"))
                    mainShader.updateUniform(Graphics::UBO_keys::NORMAL_MAP_ACTIVE, (isNormalMapActive = isNormalMapActive ? 0 : 1));
                if(gui.addButton("Draw FBO textures"))
                    drawFBOTextures = !drawFBOTextures;
                if(gui.addButton("Draw splines"))
                    drawSplines = !drawSplines;   

                gui.addSeparatorLine();

                if(gui.addButton("Post-FX parameters", gui.displayPostFxParameters) ){
                    gui.addSlider("Gamma", &gamma, 1, 8, 0.01);
                    gui.addSlider("Sobel Intensity", &sobelIntensity, 0, 4, 0.01);
                    gui.addSlider("Blur Sample Count", &sampleCount, 0, 32, 1);
                    gui.addSlider("Motion Blur Intensity", &motionBlurSampleCount, 0, 128, 1);
                    gui.addSlider("Focus Near", &focus[0], 0, 10, 0.01);
                    gui.addSlider("Focus Position", &focus[1], 0, 100, 0.01);
                    gui.addSlider("Focus Far", &focus[2], 0, 100, 0.01);
                    gui.addSlider("Occlusion Intensity", &occlusionIntensity, 0, 10, 0.01);
                    gui.addSlider("Occlusion Radius", &occlusionRadius, 0, 30, 0.01);
                    gui.addSlider("Fog density", &fogDensity, 0, 3, 0.00001);
                    imgui3Slider("X", &fogColor.x, 0,1, 0.001, 1);
                    imgui3Slider("Y", &fogColor.y, 0,1, 0.001, 2);
                    imgui3Slider("Z", &fogColor.z, 0,1, 0.001, 3);
                    gui.addSeparatorLine();
                }

                if(gui.addButton("General Lights Parameters", gui.displayGeneralLightParameters)){
                    gui.addSlider("Specular Power", &lightHandler._specularPower, 0, 100, 0.1);
                    gui.addSlider("Attenuation", &lightHandler._lightAttenuation, 0, 16, 0.1);
                    gui.addSlider("Intensity", &lightHandler._lightIntensity, 0, 10, 0.1);
                    gui.addSlider("Threshold", &lightHandler._lightAttenuationThreshold, 0, 0.5, 0.0001);
                    gui.addSlider("Ambient Intensity", &ambientIntensity, 0, 1, 0.0001);
                    gui.addSlider("Firefly Intensity", &multIntensity, 0, 2, 0.0001);
                    gui.addSlider("Ambient.r", &ambient.x, 0, 1, 0.0001);
                    gui.addSlider("Ambient.g", &ambient.y, 0, 1, 0.0001);
                    gui.addSlider("Ambient.b", &ambient.z, 0, 1, 0.0001);
                    gui.addSliderSun(lightHandler);
                    gui.addSeparatorLine();
                }

                if(gui.addButton("Point Lights Parameters", gui.displayPointLightParameters))
                    gui.addSliderPointLights(lightHandler);

                if(gui.addButton("Spot Lights Parameters", gui.displaySpotLightParameters)){
                    gui.addSlider("Shadow Bias", &shadowBias, 0, 0.01, 0.00000001);
                    gui.addSliderSpotLights(lightHandler);
                }

                if(gui.addButton("Directional Lights Parameters", gui.displayDirectionalLightParameters)){
                    gui.addSlider("Shadow Bias DirLight", &shadowBiasDirLight, 0, 0.1, 0.000001);
                    gui.addSliderDirectionalLights(lightHandler, -1, 1);
                    gui.addSlider("Ortho box dim", &dirLightOrthoProjectionDim, 1, 1000, 1);
                }

                if(gui.addButton("Camera Spline", gui.displayCameraSplineParameters)){
                    gui.addSlider("Camera splines velocity", &(cameraController.velocitySplines()), 0.0, 1.0, 0.001);
                    gui.addSlider("Camera angles velocity", &userInput.getVelocityRotate(), 0.0, 0.2, 0.001);
                    gui.addSliderSpline(cameraController.viewTargets());
                    if(gui.addButton("Add Spline"))
                        cameraController.viewTargets().add(cameraController.viewTargets()[cameraController.viewTargets().size()-1]);
                }

                if(gui.addButton("Water", gui.displayWaterParams)) {
                    gui.addSlider("Waves", &noiseAmplitudeWaves, 0.0, 1, 0.00000001);
                    gui.addSlider("Waves specular", &specularAmplitudeWaves, 0.0, 1, 0.00000001);
                    gui.addSlider("Fresnel Amplitude", &fresnelAmplitude, 0.0, 20, 0.00000001);
                    gui.addSlider("Fresnel Bias", &fresnelBias, 0.0, 1, 0.00000001);
                }

                if(gui.addButton("Bounding Box"))
                    debugScene.toggle();

                if(gui.addButton("Mesh Transform", gui.displayMeshTransform)){
                    gui.addSlider("Scale", &scaleMeshTransform, 0, 50, 0.1);
                    gui.addSeparatorLine();

                    gui.addLabel("Translation");
                    imgui3Slider("X", &translateMeshTransform.x, -10, 10, 0.1, 1);
                    imgui3Slider("Y", &translateMeshTransform.y, -10, 10, 0.1, 2);
                    imgui3Slider("Z", &translateMeshTransform.z, -10, 10, 0.1, 3);

                    if(gui.addButton("Apply transform"))
                        picker.transformPickedObject(translateMeshTransform, glm::vec3(scaleMeshTransform));

                    if(gui.addButton("Reset")){
                        translateMeshTransform = glm::vec3(0);
                        scaleMeshTransform = 1;
                    }
                }

                if(gui.addButton("Save to assets/luminolGL.json"))
                    scene.save("../assets/luminolGL.json");

                if(gui.addButton(std::string("Save Scene to " + scenePath + " and splines").c_str())){
                    scene.save(scenePath);
                    cameraController.positions().save(splineCamPositions);
                    cameraController.viewTargets().save(splineCamTargets);
                    cameraController.speeds().save(splineCamSpeeds);
                }
 
                if(gui.addButton("Spline Picker", gui.displaySplinePicker)){
                    gui.addIndent();
                    std::string statePicker = "Current state: " + Gui::SplineStateString.at(splinePicker.state());
                    statePicker += isSplinePickerEnabled ? " enabled" : " disabled";
                    gui.addLabel(statePicker.c_str());

                    isSplinePickerEnabled = gui.addButton("Toggle Activation") == !isSplinePickerEnabled;

                    if(gui.addButton("Position picking"))
                        splinePicker.setState(Gui::SplineState::position);
                    if(gui.addButton("Target picking"))
                        splinePicker.setState(Gui::SplineState::target);
                    if(gui.addButton("Veclocity picking"))
                        splinePicker.setState(Gui::SplineState::velocity);
                    gui.addSlider("Spline Y Plane", &splinePicker.yPlaneIntersection(), -100, 100, 1);

                    if(splinePicker.state() != Gui::SplineState::velocity){
                        Geometry::Spline3D& currentSplinePicker = Gui::SplineState::position == splinePicker.state()? splinePicker.positions() : splinePicker.targets();

                        gui.addLabel(std::string("Spline " + Gui::SplineStateString.at(splinePicker.state())).c_str());

                        if(gui.addButton("Clear"))
                            currentSplinePicker.clear();

                        for(size_t k = 0; k < currentSplinePicker.size(); ++k){
                            gui.addLabel(std::string("#" + std::to_string(k)).c_str());
                            gui.addSlider("Y", &currentSplinePicker[k].y, -100, 100, 1);
                            if(gui.addButton("Remove"))
                                currentSplinePicker.erase(currentSplinePicker.begin()+ k);
                            gui.addSeparatorLine();
                        }
                    }
                    gui.addUnindent();
                }
                gui.addUnindent();
            }

            gui.scrollAreaEnd();
        }

        glDisable(GL_BLEND);
#endif
        // Check for errors
        checkErrorGL("End loop");
        glfwSwapBuffers(window);
        glfwPollEvents();

        double newTime = glfwGetTime();
        fps = float(1.f/ (newTime - timeGLFW));
    }
    while( glfwGetKey( window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && !glfwWindowShouldClose(window));

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return EXIT_SUCCESS;
}
