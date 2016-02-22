#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <cmath>

#include "stb/stb_image.h"
#include "imgui/imgui.h"
#include "imgui/imguiRenderGL3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <libgen.h>
#include <graphics/BeautyFBO.hpp>
#include <graphics/PostFxFBO.hpp>

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

#include "gui/Gui.hpp"
#include "lights/Light.hpp"

#include "view/CameraFreefly.hpp"
#include "view/CameraController.hpp"

#include "gui/UserInput.hpp"


#ifndef DEBUG_PRINT
#define DEBUG_PRINT 1
#endif

#if DEBUG_PRINT == 0
#define debug_print(FORMAT, ...) ((void)0)
#else
#define debug_print(FORMAT, ...) \
    fprintf(stderr, "%s() in %s, line %i: " FORMAT "\n", \
        __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#endif

// Font buffers
extern const unsigned char DroidSans_ttf[];
extern const unsigned int DroidSans_ttf_len;

// OpenGL utils
bool checkError(const char* title);



struct UniformCamera
{
    glm::vec3 _pos;
    int _padding;
    glm::mat4 _screenToWorld;
    glm::mat4 _viewToWorld;

    UniformCamera(){}
    UniformCamera(glm::vec3 pos, glm::mat4 screenToWorld, glm::mat4 viewToWorld){
        update(pos, screenToWorld, viewToWorld);
    }

    void update(glm::vec3 pos, glm::mat4 screenToWorld, glm::mat4 viewToWorld){
        _pos = pos;
        _screenToWorld = screenToWorld;
        _viewToWorld = viewToWorld;
    }

};


void printVec3(glm::vec3 vec){
    std::cout << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]" << std::endl;
}


int main( int argc, char **argv )
{
    glm::ivec2 dimViewport(1300, 700);
    int& width = dimViewport.x, height= dimViewport.y;
    float fps = 0.f;

    GUI::UserInput userInput;
    View::CameraFreefly camera(glm::vec2(width, height), glm::vec2(0.01f, 1000.f));
    View::CameraController cameraController(camera, userInput, 0.05);

    cameraController.positions().add(glm::vec3(0,10,0)  );
    cameraController.positions().add(glm::vec3(10,10,0) );
    cameraController.positions().add(glm::vec3(10,10,10));
    cameraController.positions().add(glm::vec3(0,10,0)  );
    cameraController.viewTargets().add(glm::vec3(0, 0, 0));



    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return EXIT_FAILURE;
    }
    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    int const DPI = 2; // For retina screens only
#else
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    int const DPI = 1;
# endif

    // Open a window and create its OpenGL context
    GLFWwindow * window = glfwCreateWindow(width/DPI, height/DPI, "aogl", 0, 0);
    if( ! window )
    {
        fprintf( stderr, "Failed to open GLFW window\n" );
        glfwTerminate();
        return( EXIT_FAILURE );
    }
    glfwMakeContextCurrent(window);

    // Init glew
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return EXIT_FAILURE;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode( window, GLFW_STICKY_KEYS, GL_TRUE );

    // Enable vertical sync (on cards that support it)
    glfwSwapInterval( 1 );
    GLenum glerr = GL_NO_ERROR;
    glerr = glGetError();

    Gui::Gui gui(DPI, width, height);
    if (!imguiRenderGLInit(DroidSans_ttf, DroidSans_ttf_len))
    {
        fprintf(stderr, "Could not init GUI renderer.\n");
        return(EXIT_FAILURE);
    }

    Graphics::ShaderProgram mainShader("../shaders/aogl.vert", "../shaders/aogl.geom", "../shaders/aogl.frag");
    Graphics::ShaderProgram debugShader("../shaders/blit.vert", "", "../shaders/blit.frag");
    Graphics::ShaderProgram pointLightShader(debugShader.vShader(), "../shaders/pointLight.frag");
    Graphics::ShaderProgram directionalLightShader(debugShader.vShader(), "../shaders/directionnalLight.frag");
    Graphics::ShaderProgram spotLightShader(debugShader.vShader(), "../shaders/spotLight.frag");
    Graphics::ShaderProgram debugShapesShader("../shaders/debug.vert", "", "../shaders/debug.frag");
    Graphics::ShaderProgram shadowShader("../shaders/shadow.vert", "", "../shaders/shadow.frag");
    Graphics::ShaderProgram gammaShader(debugShader.vShader(), "../shaders/gammaCorrection.frag");
    Graphics::ShaderProgram sobelShader(debugShader.vShader(), "../shaders/sobel.frag");
    Graphics::ShaderProgram blurShader(debugShader.vShader(), "../shaders/blur.frag");
    Graphics::ShaderProgram circleConfusionShader(debugShader.vShader(), "../shaders/coc.frag");
    Graphics::ShaderProgram depthOfFieldShader(debugShader.vShader(), "../shaders/dof.frag");

    // Viewport
    glViewport( 0, 0, width, height );

    // Create Cube -------------------------------------------------------------------------------------------------------------------------------

    Graphics::VertexBufferObject cubeVerticesVbo(Graphics::VERTEX_DESCRIPTOR);
    Graphics::VertexBufferObject cubeIdsVbo(Graphics::ELEMENT_ARRAY_BUFFER);

    Graphics::VertexArrayObject cubeVAO;
    cubeVAO.addVBO(&cubeVerticesVbo);
    cubeVAO.addVBO(&cubeIdsVbo);
    cubeVAO.init();


    Graphics::Mesh cubeMesh(Graphics::Mesh::genCube());

    cubeVerticesVbo.updateData(cubeMesh.getVertices());
    cubeIdsVbo.updateData(cubeMesh.getElementIndex());

    if (!checkError("VAO/VBO")){
        std::cerr << "Error : cube vao" << std::endl;
        return -1;
    }

    // Create Plane -------------------------------------------------------------------------------------------------------------------------------

    Graphics::VertexBufferObject planeVerticesVbo(Graphics::VERTEX_DESCRIPTOR);
    Graphics::VertexBufferObject planeIdsVbo(Graphics::ELEMENT_ARRAY_BUFFER);

    Graphics::VertexArrayObject planeVAO;
    planeVAO.addVBO(&planeVerticesVbo);
    planeVAO.addVBO(&planeIdsVbo);
    planeVAO.init();

    Graphics::Mesh planeMesh(Graphics::Mesh::genPlane(100,100,150));

    planeVerticesVbo.updateData(planeMesh.getVertices());
    planeIdsVbo.updateData(planeMesh.getElementIndex());

    if (!checkError("VAO/VBO")){
        std::cerr << "Error : plane vao" << std::endl;
        return -1;
    }

    // Create Sphere -------------------------------------------------------------------------------------------------------------------------------

    Graphics::VertexBufferObject sphereVerticesVbo(Graphics::VERTEX_DESCRIPTOR);
    Graphics::VertexBufferObject sphereIdsVbo(Graphics::ELEMENT_ARRAY_BUFFER);

    Graphics::VertexArrayObject sphereVAO;
    sphereVAO.addVBO(&sphereVerticesVbo);
    sphereVAO.addVBO(&sphereIdsVbo);
    sphereVAO.init();

    Graphics::Mesh sphereMesh(Graphics::Mesh::genSphere(30,30,1,glm::vec3(2,0,2)));

    sphereVerticesVbo.updateData(sphereMesh.getVertices());
    sphereIdsVbo.updateData(sphereMesh.getElementIndex());

    if (!checkError("VAO/VBO")){
        std::cerr << "Error : sphere vao" << std::endl;
        return -1;
    }

    // Create Quad for FBO -------------------------------------------------------------------------------------------------------------------------------

    int   quad_triangleCount = 2;
    int   quad_triangleList[] = {0, 1, 2, 2, 1, 3};
    // float quad_vertices[] =  {-1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0};

    std::vector<glm::vec2> quadVertices;

    quadVertices.push_back(glm::vec2(-1.0, -1.0));
    quadVertices.push_back(glm::vec2(1.0, -1.0));
    quadVertices.push_back(glm::vec2(-1.0, 1.0));
    quadVertices.push_back(glm::vec2(1.0, 1.0));

    std::vector<int> quadIds(quad_triangleList, quad_triangleList + sizeof(quad_triangleList) / sizeof (quad_triangleList[0]));

    Graphics::VertexBufferObject quadVerticesVbo(Graphics::VEC2);
    Graphics::VertexBufferObject quadIdsVbo(Graphics::ELEMENT_ARRAY_BUFFER);

    Graphics::VertexArrayObject quadVAO;
    quadVAO.addVBO(&quadVerticesVbo);
    quadVAO.addVBO(&quadIdsVbo);
    quadVAO.init();

    quadVerticesVbo.updateData(quadVertices);
    quadIdsVbo.updateData(quadIds);

    // Create Debug Shape -------------------------------------------------------------------------------------------------------------------------------

    std::vector<int> debugId;
    debugId.push_back(0);
    debugId.push_back(1);
    debugId.push_back(2);
    debugId.push_back(3);

    std::vector<glm::vec3> debugVertices;
    debugVertices.push_back(glm::vec3(1));
    debugVertices.push_back(glm::vec3(2));
    debugVertices.push_back(glm::vec3(3));
    debugVertices.push_back(glm::vec3(4));

    Graphics::VertexBufferObject debugVerticesVbo(Graphics::VEC3);
    Graphics::VertexBufferObject debugIdsVbo(Graphics::ELEMENT_ARRAY_BUFFER);

    Graphics::VertexArrayObject debugVAO;
    debugVAO.addVBO(&debugVerticesVbo);
    debugVAO.addVBO(&debugIdsVbo);
    debugVAO.init();

    debugVerticesVbo.updateData(debugVertices);
    debugIdsVbo.updateData(debugId);

    // unbind everything
    Graphics::VertexArrayObject::unbindAll();
    Graphics::VertexBufferObject::unbindAll();

    if (!checkError("VAO/VBO")){
        std::cerr << "Error : debug vao" << std::endl;
        return -1;
    }

    // My GL Textures -------------------------------------------------------------------------------------------------------------------------------

    std::cout << "--------------- TEXTURES --------------- " << std::endl;
    std::cout << std::endl;

    Graphics::TextureHandler texHandler;

    std::string TexBricksDiff = "bricks_diff";
    texHandler.add(Graphics::Texture("../assets/textures/spnza_bricks_a_diff.tga"), TexBricksDiff);

    if (!checkError("Texture")){
        std::cout << "Error : bricks_diff" << std::endl;
        return -1;
    }

    std::string TexBricksSpec = "bricks_spec";
    texHandler.add(Graphics::Texture("../assets/textures/spnza_bricks_a_spec.tga"), TexBricksSpec);

    if (!checkError("Texture")){
        std::cout << "Error : bricks_spec" << std::endl;
        return -1;
    }

    std::string TexBricksNormal = "bricks_normal";
    texHandler.add(Graphics::Texture("../assets/textures/spnza_bricks_a_normal.tga"), TexBricksNormal);

    if (!checkError("Texture")){
        std::cout << "Error : bricks_normal" << std::endl;
        return -1;
    }

    std::cout << std::endl;
    std::cout << "---------------------------------------- " << std::endl;
    std::cout << std::endl;

    cubeMesh.attachTexture(&texHandler[TexBricksDiff], GL_TEXTURE0);
    cubeMesh.attachTexture(&texHandler[TexBricksSpec], GL_TEXTURE1);
    cubeMesh.attachTexture(&texHandler[TexBricksNormal], GL_TEXTURE2);

    planeMesh.attachTexture(&texHandler[TexBricksDiff], GL_TEXTURE0);
    planeMesh.attachTexture(&texHandler[TexBricksSpec], GL_TEXTURE1);
    planeMesh.attachTexture(&texHandler[TexBricksNormal], GL_TEXTURE2);

    sphereMesh.attachTexture(&texHandler[TexBricksDiff], GL_TEXTURE0);
    sphereMesh.attachTexture(&texHandler[TexBricksSpec], GL_TEXTURE1);
    sphereMesh.attachTexture(&texHandler[TexBricksNormal], GL_TEXTURE2);

    // My Lights -------------------------------------------------------------------------------------------------------------------------------

    Light::LightHandler lightHandler;

    lightHandler.addDirectionalLight(glm::vec3(-1,-1,-1), glm::vec3(0.8,0.8,0.8), 0);
    lightHandler.addSpotLight(glm::vec3(-4,5,-4), glm::vec3(1,-1,1), glm::vec3(1,0.5,0), 0.6, 0, 60, 66);
//    lightHandler.addPointLight(glm::vec3(2.5,0.5,4), glm::vec3(0.2,0.2,0.95), 0.9, 2.0);

    

    // ---------------------- For Geometry Shading
    float t = 0;
    float SliderValue = 0.3;
    float SliderMult = 80;
    float instanceNumber = 100;
    int isNormalMapActive = 1;

    mainShader.updateUniform(Graphics::UBO_keys::DIFFUSE, 0);
    mainShader.updateUniform(Graphics::UBO_keys::SPECULAR, 1);
    mainShader.updateUniform(Graphics::UBO_keys::NORMAL_MAP, 2);
    mainShader.updateUniform(Graphics::UBO_keys::INSTANCE_NUMBER, int(instanceNumber));
    mainShader.updateUniform(Graphics::UBO_keys::NORMAL_MAP_ACTIVE, isNormalMapActive);
    shadowShader.updateUniform(Graphics::UBO_keys::INSTANCE_NUMBER, int(instanceNumber));

    if (!checkError("Uniforms")){
        std::cerr << "Error : geometry uniforms" << std::endl;
        return -1;
    }

    // ---------------------- For Light Pass Shading
    directionalLightShader.updateUniform(Graphics::UBO_keys::COLOR_BUFFER, 0);
    spotLightShader.updateUniform(Graphics::UBO_keys::COLOR_BUFFER, 0);
    pointLightShader.updateUniform(Graphics::UBO_keys::COLOR_BUFFER, 0);

    directionalLightShader.updateUniform(Graphics::UBO_keys::NORMAL_BUFFER, 1);
    spotLightShader.updateUniform(Graphics::UBO_keys::NORMAL_BUFFER, 1);
    pointLightShader.updateUniform(Graphics::UBO_keys::NORMAL_BUFFER, 1);

    directionalLightShader.updateUniform(Graphics::UBO_keys::DEPTH_BUFFER, 2);
    spotLightShader.updateUniform(Graphics::UBO_keys::DEPTH_BUFFER, 2);
    pointLightShader.updateUniform(Graphics::UBO_keys::DEPTH_BUFFER, 2);


    if (!checkError("Uniforms"))
        return(1);

    GLint blurDirectionLocation = glGetUniformLocation(blurShader.id(), "BlurDirection");
    // ---------------------- FX Variables
    float shadowBias = 0.00013;

    float gamma = 1.22;
    float sobelIntensity = 0.5;
    float sampleCount = 9; // blur
    glm::vec3 focus(0, 1, 100);


    // ---------------------- FX uniform update
    // For shadow pass shading
    spotLightShader.updateUniform(Graphics::UBO_keys::SHADOW_BUFFER, 3);

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


    if (!checkError("Uniforms")){
        std::cout << "Error : post_fx Uniforms" << std::endl;
        return -1;
    }

    // My FBO -------------------------------------------------------------------------------------------------------------------------------
    Graphics::GeometricFBO gBufferFBO(dimViewport);

    //TODO: remove poisson shadow ?
    float shadowPoissonSampleCount = 1;
    float shadowPoissonSpread = 1;

    Graphics::ShadowMapFBO shadowMapFBO(glm::ivec2(2048));
    Graphics::BeautyFBO beautyFBO(dimViewport);
    Graphics::PostFxFBO fxFBO(dimViewport, 4);



    // Create UBO For Light Structures -------------------------------------------------------------------------------------------------------------------------------
    // Create two ubo for light and camera
    const GLuint LightBindingPoint = 0;
    const GLuint CameraBindingPoint = 1;

    Graphics::UBO uboLight(LightBindingPoint, sizeof(Light::SpotLight));
    Graphics::UBO uboCamera(CameraBindingPoint, sizeof(UniformCamera));

    // LIGHT
    pointLightShader.updateBindingPointUBO("Light", uboLight.bindingPoint());
    directionalLightShader.updateBindingPointUBO("Light", uboLight.bindingPoint());
    spotLightShader.updateBindingPointUBO("Light", uboLight.bindingPoint());

    // CAM
    pointLightShader.updateBindingPointUBO("Camera", uboCamera.bindingPoint());
    directionalLightShader.updateBindingPointUBO("Camera", uboCamera.bindingPoint());
    spotLightShader.updateBindingPointUBO("Camera", uboCamera.bindingPoint());

    // Viewer Structures ----------------------------------------------------------------------------------------------------------------------

    //*********************************************************************************************
    //***************************************** MAIN LOOP *****************************************
    //*********************************************************************************************
    do {
        t = glfwGetTime();
        userInput.update(window);
        cameraController.update(t);


        int leftButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT );
        // int rightButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT );
        // int middleButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_MIDDLE);

        // Camera movements
        // int altPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);

        // Get camera matrices
        glm::mat4 projection = camera.getProjectionMatrix();
        glm::mat4 worldToView = camera.getViewMatrix();
        glm::mat4 objectToWorld;
        glm::mat4 mvp = projection * worldToView * objectToWorld;
        glm::mat4 mv = worldToView * objectToWorld;
        glm::mat4 mvInverse = glm::inverse(mv);
        glm::mat4 screenToView = glm::inverse(projection);

        // Light space matrices
        // From light space to shadow map screen space
        glm::mat4 proj = glm::perspective(glm::radians(lightHandler._spotLights[0]._falloff*2.f), 1.0f, 0.1f, 100.f);
        // From world to light
        glm::mat4 worldToLight = glm::lookAt(lightHandler._spotLights[0]._pos, lightHandler._spotLights[0]._pos + lightHandler._spotLights[0]._dir, glm::vec3(0.f, 1.f, 0.f));
        // From object to light (MV for light)
        glm::mat4 objectToLight = worldToLight * objectToWorld;
        // From object to shadow map screen space (MVP for light)
        glm::mat4 objectToLightScreen = proj * objectToLight;
        // From world to shadow map screen space
        glm::mat4 worldToLightScreen = proj * worldToLight;

        //****************************************** RENDER *******************************************

        // Default states
        glEnable(GL_DEPTH_TEST);
        // Clear the front buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Select shader
        mainShader.useProgram();

        //-------------------------------------Upload Uniforms

        mainShader.updateUniform(Graphics::UBO_keys::MVP, mvp);
        mainShader.updateUniform(Graphics::UBO_keys::MV, mv);
        mainShader.updateUniform(Graphics::UBO_keys::CAMERA_POSITION, camera.getEye());
        debugShapesShader.updateUniform(Graphics::UBO_keys::MVP, mvp);
        debugShapesShader.updateUniform(Graphics::UBO_keys::MV_INVERSE, mvInverse);


        // Upload value
        mainShader.updateUniform(Graphics::UBO_keys::TIME, t);
        mainShader.updateUniform(Graphics::UBO_keys::SLIDER, SliderValue);
        mainShader.updateUniform(Graphics::UBO_keys::SLIDER_MULT, SliderMult);
        mainShader.updateUniform(Graphics::UBO_keys::SPECULAR_POWER, lightHandler._specularPower);
        mainShader.updateUniform(Graphics::UBO_keys::INSTANCE_NUMBER, int(instanceNumber));

        // Update scene uniforms
        shadowShader.updateUniform(Graphics::UBO_keys::INSTANCE_NUMBER, int(instanceNumber));
        shadowShader.updateUniform(Graphics::UBO_keys::SHADOW_MVP, objectToLightScreen);
        shadowShader.updateUniform(Graphics::UBO_keys::SHADOW_MV, objectToLight);

        spotLightShader.updateUniform(Graphics::UBO_keys::WORLD_TO_LIGHT_SCREEN, worldToLightScreen);
        spotLightShader.updateUniform(Graphics::UBO_keys::SHADOW_BIAS, shadowBias);
        spotLightShader.updateUniform(Graphics::UBO_keys::SHADOW_POISSON_SAMPLE_COUNT, int(shadowPoissonSampleCount));
        spotLightShader.updateUniform(Graphics::UBO_keys::SHADOW_POISSON_SPREAD, shadowPoissonSpread);
        gammaShader.updateUniform(Graphics::UBO_keys::GAMMA, gamma);
        sobelShader.updateUniform(Graphics::UBO_keys::SOBEL_INTENSITY, sobelIntensity);
        blurShader.updateUniform(Graphics::UBO_keys::BLUR_SAMPLE_COUNT, (int)sampleCount);
        circleConfusionShader.updateUniform(Graphics::UBO_keys::SCREEN_TO_VIEW, screenToView);
        circleConfusionShader.updateUniform(Graphics::UBO_keys::FOCUS, focus);

        //******************************************************* FIRST PASS
        //-------------------------------------Bind gbuffer
        gBufferFBO.bind();
        gBufferFBO.clear();


        //-------------------------------------Render Cubes
        cubeVAO.bind();
        cubeMesh.bindTextures();
        glDrawElementsInstanced(GL_TRIANGLES, cubeMesh.getVertexCount(), GL_UNSIGNED_INT, (void*)0, int(instanceNumber));

        //-------------------------------------Render Plane
        mainShader.updateUniform(Graphics::UBO_keys::INSTANCE_NUMBER, -1);

        planeVAO.bind();
        planeMesh.bindTextures();
        glDrawElements(GL_TRIANGLES, planeMesh.getVertexCount(), GL_UNSIGNED_INT, (void*)0);
        glBindTexture(GL_TEXTURE_2D, 0);

        //-------------------------------------Render Sphere
        mainShader.updateUniform(Graphics::UBO_keys::INSTANCE_NUMBER, -1);

        sphereVAO.bind();
        sphereMesh.bindTextures();
        glDrawElements(GL_TRIANGLES, sphereMesh.getVertexCount() * 1000, GL_UNSIGNED_INT, (void*)0);
        glBindTexture(GL_TEXTURE_2D, 0);

        //-------------------------------------Unbind the frambuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //******************************************************* SECOND PASS
        //-------------------------------------Shadow pass
        shadowMapFBO.bind();
        // Clear only the depth buffer
        shadowMapFBO.clearDepth();

        // Set the viewport corresponding to shadow texture resolution
        glViewport(0, 0, shadowMapFBO.resolution().x, shadowMapFBO.resolution().y);

        // Render the scene
        shadowShader.useProgram();
        //cubes
        cubeVAO.bind();
        glDrawElementsInstanced(GL_TRIANGLES, cubeMesh.getVertexCount(), GL_UNSIGNED_INT, (void*)0, int(instanceNumber));

        //plane
        shadowShader.updateUniform(Graphics::UBO_keys::INSTANCE_NUMBER, -1);

        planeVAO.bind();
        glDrawElements(GL_TRIANGLES, planeMesh.getVertexCount(), GL_UNSIGNED_INT, (void*)0);


        sphereVAO.bind();
        glDrawElements(GL_TRIANGLES, sphereMesh.getVertexCount() * 1000, GL_UNSIGNED_INT, (void*)0);

        // Fallback to default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Revert to window size viewport
        glViewport(0, 0, width, height);

        //-------------------------------------Light Draw
        beautyFBO.bind();
        beautyFBO.clearColor();

        // Set a full screen viewport
        glViewport( 0, 0, width, height );

        // Disable the depth test
        glDisable(GL_DEPTH_TEST);
        // Enable blending
        glEnable(GL_BLEND);
        // Setup additive blending
        glBlendFunc(GL_ONE, GL_ONE);

        // Update Camera pos and screenToWorld matrix to all light shaders
        UniformCamera uCamera(camera.getEye(), glm::inverse(mvp), mvInverse);
        uboCamera.updateBuffer(&uCamera, sizeof(UniformCamera));

        //directionnal light shaders
        directionalLightShader.useProgram();

        // Bind quad vao
        quadVAO.bind();

        gBufferFBO.color().bind(GL_TEXTURE0);
        gBufferFBO.normal().bind(GL_TEXTURE1);
        gBufferFBO.depth().bind(GL_TEXTURE2);

        for(size_t i = 0; i < lightHandler._directionnalLights.size(); ++i){
            uboLight.updateBuffer(&lightHandler._directionnalLights[i], sizeof(Light::DirectionalLight));
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        }

        // ------------------------------------ Spot Lights
        
        spotLightShader.useProgram(); // spot light shaders    
        quadVAO.bind(); // Bind quad vao

        gBufferFBO.color().bind(GL_TEXTURE0);
        gBufferFBO.normal().bind(GL_TEXTURE1);
        gBufferFBO.depth().bind(GL_TEXTURE2);
        shadowMapFBO.shadowTexture().bind(GL_TEXTURE3);

        for(size_t i = 0; i < lightHandler._spotLights.size(); ++i){
            uboLight.updateBuffer(&lightHandler._spotLights[i], sizeof(Light::SpotLight));
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        }


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

        // ------- SOBEL ------
        fxFBO.bind();
        fxFBO.changeCurrentTexture(0);
        fxFBO.clearColor();

        // Set quad as vao: deferred
        quadVAO.bind();
        sobelShader.useProgram();
        beautyFBO.beauty().bind(GL_TEXTURE0);
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

        // ------- GAMMA ------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        gammaShader.useProgram();
        fxFBO.texture(3).bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        //-------------------------------------Debug Draw

        //------------------------------------ Debug Shape Drawing

        debugShapesShader.useProgram();
        glPointSize(10);
        debugVAO.bind();

        int id = 0;

        debugVertices.clear();
        debugId.clear();

        for(float i = 0; i < 1; i +=0.01){
            debugId.push_back(id);
            ++id;
            debugVertices.push_back(cameraController.positions().cubicInterpolation(i));
        }


        debugVerticesVbo.updateData(debugVertices);
        debugIdsVbo.updateData(debugId);

        glDrawElements(GL_LINE_STRIP, debugVertices.size(), GL_UNSIGNED_INT, (void*)0);

        int screenNumber = 6;

        glDisable(GL_DEPTH_TEST);
        glViewport( 0, 0, width/screenNumber, height/screenNumber );

        // Select shader
        debugShader.useProgram();

        // --------------- Color Buffer

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

        // --------------- Beauty Buffer
        glViewport( 3*width/screenNumber, 0, width/screenNumber, height/screenNumber );

        quadVAO.bind();
        beautyFBO.beauty().bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // --------------- Circle of confusion Buffer
        glViewport( 4*width/screenNumber, 0, width/screenNumber, height/screenNumber );

        quadVAO.bind();
        fxFBO.texture(1).bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // --------------- Blur Buffer
        glViewport( 5*width/screenNumber, 0, width/screenNumber, height/screenNumber );
//        glViewport(0, 0, width, height);

        quadVAO.bind();
        fxFBO.texture(2).bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        //****************************************** EVENTS *******************************************
#if 1
        // Draw UI
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0, 0, width, height);

        gui.beginFrame();
        gui.getCursorPos(window);
        gui.updateFrame();

        bool leftButtonPress = false;
        if( leftButton == GLFW_PRESS ) leftButtonPress = true;

        gui.updateMbut(leftButtonPress);   


        std::map<std::string,float*> imguiParams = {
            { "FPS", &fps },
            { "Slider", &SliderValue },
            { "InstanceNumber", &instanceNumber },
            { "SliderMultiply", &SliderMult }, 
            { "Shadow Bias", &shadowBias }, 
            { "Gamma", &gamma }, 
            { "SobelIntensity", &sobelIntensity }, 
            { "BlurSampleCount", &sampleCount},
            { "FocusNear", &focus[0] }, 
            { "FocusPosition", &focus[1] }, 
            { "FocusFar", &focus[2] },
        };

        std::map<std::string, Graphics::ShaderProgram*> imguiShaders = {
            { "mainShader", &mainShader }
        };
        std::map<std::string,std::string> imguiUniforms = {
            {"UNIFORM_NAME_NORMAL_MAP_ACTIVE", Graphics::UBO_keys::NORMAL_MAP_ACTIVE}
        };

        gui.scrollArea(imguiParams, lightHandler, cameraController.viewTargets(), cameraController, userInput, imguiShaders, imguiUniforms);
        gui.scrollAreaEnd();

        glDisable(GL_BLEND);
#endif
        // Check for errors
        checkError("End loop");

        glfwSwapBuffers(window);
        glfwPollEvents();

        double newTime = glfwGetTime();
        fps = 1.f/ (newTime - t);
    } // Check if the ESC key was pressed
    while( glfwGetKey( window, GLFW_KEY_ESCAPE ) != GLFW_PRESS );

    //*********************************************************************************************
    //************************************* MAIN LOOP END *****************************************
    //*********************************************************************************************

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return EXIT_SUCCESS;
}

bool checkError(const char* title)
{
    int error;
    if((error = glGetError()) != GL_NO_ERROR)
    {
        std::string errorString;
        switch(error)
        {
            case GL_INVALID_ENUM:
                errorString = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                errorString = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                errorString = "GL_INVALID_OPERATION";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                errorString = "GL_OUT_OF_MEMORY";
                break;
            default:
                errorString = "UNKNOWN";
                break;
        }
        fprintf(stdout, "OpenGL Error(%s): %s\n", errorString.c_str(), title);
    }
    return error == GL_NO_ERROR;
}