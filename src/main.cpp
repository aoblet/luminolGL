#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <cmath>

#include "imgui/imguiRenderGL3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <libgen.h>

#include "geometry/Spline3D.h"

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

#include "gui/Gui.hpp"
#include "gui/ObjectPicker.h"
#include "lights/Light.hpp"

#include "view/CameraFreefly.hpp"
#include "view/CameraController.hpp"

#include "gui/UserInput.hpp"

#include "utils/utils.h"

#include "data/SceneIOJson.hpp"
#include "data/UniformCamera.hpp"


#include <glog/logging.h>
#include <glm/ext.hpp>
#include <graphics/Skybox.hpp>

#define IMGUI_DRAW 1

// Font buffers IMGUI
extern const unsigned char DroidSans_ttf[];
extern const unsigned int DroidSans_ttf_len;

int main( int argc, char **argv ) {

    // GLOG debug level, 0 == all
    FLAGS_minloglevel = 0;

    int DPI;
    GLFWwindow * window = nullptr;
    glm::ivec2 dimViewport(1280, 720);
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

    View::CameraFreefly camera(glm::vec2(width, height), glm::vec2(0.01, 1000.f));
    camera.setEye(glm::vec3(10,10,-10));
    View::CameraController cameraController(camera, userInput, 0.05);

    cameraController.positions().add(glm::vec3(0,10,0));
    cameraController.positions().add(glm::vec3(10,10,0) );
    cameraController.positions().add(glm::vec3(10,10,10));
    cameraController.positions().add(glm::vec3(0,10,0));
    cameraController.viewTargets().add(glm::vec3(0, 0, 0));

    // SHADERS
    Graphics::ShaderProgram mainShader("../shaders/aogl.vert", "", "../shaders/aogl.frag");
    Graphics::ShaderProgram blitShader("../shaders/blit.vert", "", "../shaders/blit.frag");
    Graphics::ShaderProgram pointLightShader(blitShader.vShader(), "../shaders/pointLight.frag");
    Graphics::ShaderProgram indirectLightShader(blitShader.vShader(), "../shaders/indirectLight.frag");
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


    // Create Objects -------------------------------------------------------------------------------------------------------------------------------
    Graphics::ModelMeshInstanced planeInstances("../assets/models/primitives/plane.obj");
    planeInstances.addInstance(glm::vec3(0,0,0), glm::vec4(0,0,0,0), glm::vec3(500,1,500));

    checkErrorGL("VAO/VBO");

    Graphics::ModelMeshInstanced crysisModel("../assets/models/crysis/nanosuit.obj");
    crysisModel.addInstance(glm::vec3(5,0,2), glm::vec4(0,0,0,0), glm::vec3(1,1,1));
    crysisModel.addInstance(glm::vec3(-5,0,2), glm::vec4(0,0,0,0), glm::vec3(1,1,1));

    // Create Quad for FBO -------------------------------------------------------------------------------------------------------------------------------
    int   quad_triangleCount = 2;

    std::vector<glm::vec2> quadVertices = {
        glm::vec2(-1.0, -1.0),
        glm::vec2(1.0, -1.0),
        glm::vec2(-1.0, 1.0),
        glm::vec2(1.0, 1.0)
    };

    std::vector<int> quadIds = {0, 1, 2, 2, 1, 3};

    Graphics::VertexBufferObject quadVerticesVbo(Graphics::VEC2);
    Graphics::VertexBufferObject quadIdsVbo(Graphics::ELEMENT_ARRAY_BUFFER);

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
    std::vector<Graphics::ModelMeshInstanced> sceneMeshes;
    sceneMeshes.push_back(std::move(crysisModel));
    sceneMeshes.push_back(std::move(planeInstances));

    Data::SceneIOJson sceneIOJson;
    Graphics::Scene scene(&sceneIOJson, "", std::move(sceneMeshes));
    Graphics::DebugBoundingBoxes debugScene(scene.meshInstances());
    checkErrorGL("Scene");


    // My Lights -------------------------------------------------------------------------------------------------------------------------------
    Light::LightHandler lightHandler;
    lightHandler.setDirectionalLight(glm::vec3(-1, -1, -1), glm::vec3(0.6, 0.9, 1), 1);

    glm::vec3 ambient(1);
    float ambientIntensity = 0.2;

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

    pointLightShader.updateUniform(Graphics::UBO_keys::COLOR_BUFFER, 0);
    pointLightShader.updateUniform(Graphics::UBO_keys::NORMAL_BUFFER, 1);
    pointLightShader.updateUniform(Graphics::UBO_keys::DEPTH_BUFFER, 2);

    indirectLightShader.updateUniform(Graphics::UBO_keys::COLOR_BUFFER, 0);
    indirectLightShader.updateUniform(Graphics::UBO_keys::BEAUTY_BUFFER, 1);
    indirectLightShader.updateUniform(Graphics::UBO_keys::SSAO_BUFFER, 2);

    checkErrorGL("Uniforms");

    // ---------------------- FX Variables
    float shadowBias            = 0.00019;
    float shadowBiasDirLight    = 0.001;

    float gamma                 = 1.22;
    float sobelIntensity        = 0.15;
    float sampleCount           = 1; // blur
    float motionBlurSampleCount = 8; // motion blur
    float dirLightOrthoProjectionDim = 100;
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
    pointLightShader.updateBindingPointUBO(Graphics::UBO_keys::STRUCT_BINDING_POINT_LIGHT, uboLight.bindingPoint());
    directionalLightShader.updateBindingPointUBO(Graphics::UBO_keys::STRUCT_BINDING_POINT_LIGHT, uboLight.bindingPoint());
    spotLightShader.updateBindingPointUBO(Graphics::UBO_keys::STRUCT_BINDING_POINT_LIGHT, uboLight.bindingPoint());

    // CAM
    pointLightShader.updateBindingPointUBO(Graphics::UBO_keys::STRUCT_BINDING_POINT_CAMERA, uboCamera.bindingPoint());
    directionalLightShader.updateBindingPointUBO(Graphics::UBO_keys::STRUCT_BINDING_POINT_CAMERA, uboCamera.bindingPoint());
    spotLightShader.updateBindingPointUBO(Graphics::UBO_keys::STRUCT_BINDING_POINT_CAMERA, uboCamera.bindingPoint());

    // Samples for SSAO -------------------------------------------------------------------------------------------------------------------------------

    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // random floats between 0.0 - 1.0
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    for (int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(
                randomFloats(generator) * 2.0f - 1.0f,
                randomFloats(generator) * 2.0f - 1.0f,
                randomFloats(generator)
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.f;
        scale = glm::lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    int noiseSizeX = 4;
    int noiseSizeY = 4;

    std::vector<glm::vec3> ssaoNoise;
    for (GLuint i = 0; i < noiseSizeX * noiseSizeY; i++)
    {
        glm::vec3 noise(
                randomFloats(generator) * 2.0 - 1.0,
                randomFloats(generator) * 2.0 - 1.0,
                0.0f);
        ssaoNoise.push_back(noise);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, noiseSizeX, noiseSizeY, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);

    Graphics::Texture ssaoNoiseTex(4, 4, Graphics::TexParams(GL_RGB16F, GL_RGB, GL_FLOAT, GL_REPEAT, GL_NEAREST));
    ssaoNoiseTex.sendGL(ssaoNoise.data());

    ssaoShader.updateUniform(Graphics::UBO_keys::SSAO_SAMPLES, ssaoKernel);

    float occlusionIntensity = 1.0;
    float occlusionRadius = 1.0;

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
        glm::mat4 mv  = worldToView * objectToWorld;
        glm::mat4 mvp = projection * mv;
        glm::mat4 vp  = projection * worldToView;
        glm::mat4 mvInverse     = glm::inverse(mv);
        glm::mat4 mvNormal      = glm::transpose(mvInverse);
        glm::mat4 screenToView  = glm::inverse(projection);

        // For skybox
        glm::mat4 unTranslatedMV = glm::mat4(glm::mat3(worldToView));
        glm::mat4 screenToWorldUnTranslated = glm::inverse(unTranslatedMV) * screenToView;

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
        directionalLightShader.updateUniform(Graphics::UBO_keys::SHADOW_BIAS, shadowBias);
        directionalLightShader.updateUniform(Graphics::UBO_keys::SHADOW_POISSON_SAMPLE_COUNT, int(shadowPoissonSampleCount));
        directionalLightShader.updateUniform(Graphics::UBO_keys::SHADOW_POISSON_SPREAD, shadowPoissonSpread);

        indirectLightShader.updateUniform(Graphics::UBO_keys::AMBIENT_INTENSITY, ambient * ambientIntensity);

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


        //******************************************************* FIRST PASS (Geometric pass)

        // Default states
        glEnable(GL_DEPTH_TEST);
        // Clear the front buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Select shader
        mainShader.useProgram();

        // Render scene into Geometric buffer
        mainShader.useProgram();
        gBufferFBO.bind();
        gBufferFBO.clear();
        scene.draw(vp);
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

            gBufferFBO.color().bind(GL_TEXTURE0);
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
        scene.draw(worldToDirLightScreen);
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

        // ------------------------------------ Point Lights
        pointLightShader.useProgram(); // point light shaders
        quadVAO.bind(); // Bind quad vao

        gBufferFBO.color().bind(GL_TEXTURE0);
        gBufferFBO.normal().bind(GL_TEXTURE1);
        gBufferFBO.depth().bind(GL_TEXTURE2);
        // gBufferFBO.shadow().bind(GL_TEXTURE3);

        for(size_t i = 0; i < lightHandler._pointLights.size(); ++i){
            std::vector<glm::vec2> littleQuadVertices;
            if(lightHandler.isOnScreen(mvp, littleQuadVertices, lightHandler._pointLights[i]._pos, lightHandler._pointLights[i]._color, lightHandler._pointLights[i]._intensity, lightHandler._pointLights[i]._attenuation)){
                //quad size reduction and frustum according to the light position, intensity, color and attenuation
                quadVerticesVbo.updateData(littleQuadVertices);
                quadIdsVbo.updateData(quadIds);
                uboLight.updateBuffer(&lightHandler._pointLights[i], sizeof(Light::PointLight));
                glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
            }
        }
        quadVerticesVbo.updateData(quadVertices);
        quadIdsVbo.updateData(quadIds);

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

        // ------- SKYBOX ------
        // Render skybox texture combined with beauty: mask with depth buffer
        // All in one pass
        fxFBO.changeCurrentTexture(4);
        fxFBO.clearColor();
        skybox.updateUniforms(screenToWorldUnTranslated, 0, 1, 2);
        skybox.useProgramShader();

        skybox.bindTexture(GL_TEXTURE0); // cubeMap
        gBufferFBO.depth().bind(GL_TEXTURE1);
        beautyFBO.beauty().bind(GL_TEXTURE2);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // ------- SSAO ------

        fxFBO.changeCurrentTexture(0);
        ssaoShader.useProgram();
        gBufferFBO.position().bind(GL_TEXTURE0); // position
        gBufferFBO.depth().bind(GL_TEXTURE1); // depth
        gBufferFBO.normal().bind(GL_TEXTURE2); // normal
        ssaoNoiseTex.bind(GL_TEXTURE3); // noise
//        beautyFBO.beauty().bind(GL_TEXTURE4); // beauty
//        beautyFBO.beauty().bind(GL_TEXTURE4); // beauty
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        //blur on ssao texture
        if(sampleCount > 0){
            // Use blur program shader
            blurShader.useProgram();
            blurShader.updateUniform(Graphics::UBO_keys::BLUR_SAMPLE_COUNT, 3);
            blurShader.updateUniform(Graphics::UBO_keys::BLUR_DIRECTION, glm::ivec2(1,0));

            // Write into Vertical Blur Texture
            fxFBO.changeCurrentTexture(1);
            // Clear the content of texture
            fxFBO.clearColor();
            // Read the texture processed by the Sobel operator
            fxFBO.texture(0).bind(GL_TEXTURE0);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

            // Write into Horizontal Blur Texture
            blurShader.updateUniform(Graphics::UBO_keys::BLUR_DIRECTION, glm::ivec2(0,1));
            fxFBO.changeCurrentTexture(0);
            // Clear the content of texture
            fxFBO.clearColor();
            // Read the texture processed by the Vertical Blur
            fxFBO.texture(1).bind(GL_TEXTURE0);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        }

        // ------- INDIRECT LIGHT ------
        indirectLightShader.useProgram();

        fxFBO.changeCurrentTexture(1);
        fxFBO.clearColor();

        gBufferFBO.color().bind(GL_TEXTURE0); // color
//        beautyFBO.beauty().bind(GL_TEXTURE1); // beauty
        fxFBO.texture(4).bind(GL_TEXTURE1); // beauty
        fxFBO.texture(0).bind(GL_TEXTURE2); // ssao

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

        gammaShader.useProgram();
        fxFBO.texture(1).bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        //------------------------------------ Debug Shape Drawing
        debugScene.draw(mvp);
        picker.drawPickedObject(debugShapesShader);

        int screenNumber = 7;
        glDisable(GL_DEPTH_TEST);

        if(drawFBOTextures){
            int screenNumber = 6;
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
            fxFBO.texture(4).bind(GL_TEXTURE0);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        }

        //****************************************** EVENTS *******************************************

#ifdef IMGUI_DRAW
        // Draw UI
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0, 0, width, height);

        gui.init(window);
        gui.updateMbut(glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS);

        picker.pickObject(gui.getCursorPosition(), gui.getCursorSpeed(), scene, camera, glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS);

        if(glfwGetKey(window, GLFW_KEY_T)) picker.switchMode(Gui::PickerMode::TRANSLATION);
        if(glfwGetKey(window, GLFW_KEY_Y)) picker.switchMode(Gui::PickerMode::SCALE);
        if(glfwGetKey(window, GLFW_KEY_R)) picker.switchMode(Gui::PickerMode::ROTATION);

        gui.addLabel("FPS", &fps);

        if(gui.addButton("Menu", gui.displayMenu)){
            gui.setWindowWidth(guiExpandWidth);
            gui.setWindowHeight(guiExpandHeight);
        }
        else{
            gui.setWindowWidth(guiMinimalWidth);
            gui.setWindowHeight(guiMinimalHeight);
        }


        if(gui.displayMenu){

            gui.addSeparatorLine();
            gui.addIndent();

            if(gui.addButton("Camera switch"))
                cameraController.setSpectator(!cameraController.isSpectator());
            if(gui.addButton("IsNormalMapActive"))
                mainShader.updateUniform(Graphics::UBO_keys::NORMAL_MAP_ACTIVE, (isNormalMapActive = isNormalMapActive ? 0 : 1));
            if(gui.addButton("Draw FBO textures"))
                drawFBOTextures = !drawFBOTextures;
            
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
                gui.addSeparatorLine();
            }

            if(gui.addButton("General Lights Parameters", gui.displayGeneralLightParameters)){
                gui.addSlider("Specular Power", &lightHandler._specularPower, 0, 100, 0.1);
                gui.addSlider("Attenuation", &lightHandler._lightAttenuation, 0, 16, 0.1);
                gui.addSlider("Intensity", &lightHandler._lightIntensity, 0, 10, 0.1);
                gui.addSlider("Threshold", &lightHandler._lightAttenuationThreshold, 0, 0.5, 0.0001);

                gui.addSlider("Ambient Intensity", &ambientIntensity, 0, 1, 0.0001);

                gui.addSlider("Ambient.r", &ambient.x, 0, 1, 0.0001);
                gui.addSlider("Ambient.g", &ambient.y, 0, 1, 0.0001);
                gui.addSlider("Ambient.b", &ambient.z, 0, 1, 0.0001);
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

            if(gui.addButton("Bounding Box"))
                debugScene.toggle();

            gui.addUnindent();

        }

        gui.scrollAreaEnd();

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
