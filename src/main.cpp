#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>

#include "stb/stb_image.h"
#include "imgui/imgui.h"
#include "imgui/imguiRenderGL3.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <libgen.h>
#include <graphics/GeometricBuffer.hpp>

#include "geometry/Spline3D.h"
#include "graphics/ShaderProgram.hpp"
#include "graphics/Texture.h"
#include "graphics/TextureHandler.h"
#include "graphics/VertexDescriptor.h"
#include "graphics/VertexBufferObject.h"
#include "graphics/VertexArrayObject.h"
#include "graphics/UBO.hpp"
#include "graphics/Mesh.h"
#include "graphics/MeshInstance.h"
#include "graphics/Scene.h"

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

// Shader utils
int check_compile_error(GLuint shader, const char ** sourceBuffer);

// OpenGL utils
bool checkError(const char* title);

enum LightType{
    POINT,
    DIRECTIONNAL,
    SPOT
};

struct Light
{
    glm::vec3 _pos;
    int _padding;
    glm::vec3 _color;
    float _intensity;
    float _attenuation;

    Light(glm::vec3 pos=glm::vec3(0,0,0), glm::vec3 color=glm::vec3(0,0,0), float intensity=0.2, float attenuation=0.2){
        update(pos, color, intensity, attenuation);
    }

    void update(glm::vec3 pos, glm::vec3 color, float intensity, float attenuation){
        _pos = pos;
        _color = color;
        _intensity = intensity;
        _attenuation = attenuation;
    }
};

struct SpotLight
{
    glm::vec3 _pos;
    int _padding1; //16

    glm::vec3 _color;

    float _intensity; //32

    float _attenuation;
    int _padding2;
    int _padding3;
    int _padding4; //48

    glm::vec3 _dir;

    float _angle; //64

    float _falloff; //68

    SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float intensity, float attenuation, float angle, float falloff){
        update(pos, dir, color, intensity, attenuation, angle, falloff);
    }

    SpotLight(){};

    void update(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float intensity, float attenuation, float angle, float falloff){
        _pos = pos;
        _dir = dir;
        _color = color;
        _intensity = intensity;
        _attenuation = attenuation;
        _angle = angle;
        _falloff = falloff;
    }
};

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

struct GUIStates
{
    bool panLock;
    bool turnLock;
    bool zoomLock;
    int lockPositionX;
    int lockPositionY;
    int camera;
    double time;
    bool playing;
    static const float MOUSE_PAN_SPEED;
    static const float MOUSE_ZOOM_SPEED;
    static const float MOUSE_TURN_SPEED;
};
const float GUIStates::MOUSE_PAN_SPEED = 0.001f;
const float GUIStates::MOUSE_ZOOM_SPEED = 0.05f;
const float GUIStates::MOUSE_TURN_SPEED = 0.005f;
void init_gui_states(GUIStates & guiStates);

int main( int argc, char **argv )
{
    int width = 1300, height= 700;
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

    Graphics::Mesh cubeMesh(Graphics::Mesh::genCube());

    Graphics::MeshInstance cubeInstances(&cubeMesh);

    int cubeInstanceWidth = 10;
    int cubeInstanceHeight = 10;

    std::vector<glm::mat4> cubeInstanceTransform;

    int k = 0;
    for(int i = 0; i < cubeInstanceHeight; ++i){
        for(int j = 0; j < cubeInstanceWidth; ++j){
            cubeInstances.addInstance(glm::vec3(i * 2, 1.5f, j * 2));
            cubeInstanceTransform.push_back(cubeInstances.getTransformationMatrix(k));
            ++k;
        }
    }

    if (!checkError("VAO/VBO")){
        std::cerr << "Error : cube vao" << std::endl;
        return -1;
    }

    // Create Sphere -------------------------------------------------------------------------------------------------------------------------------

    Graphics::Mesh sphereMesh(Graphics::Mesh::genSphere(10,10,0.2));
    Graphics::MeshInstance sphereInstances(&sphereMesh);

    int sphereInstanceWidth = 10;
    int sphereInstanceHeight = 10;


    for(int i = 0; i < sphereInstanceWidth; ++i){
        for(int j = 0; j < sphereInstanceHeight; ++j){
            sphereInstances.addInstance(glm::vec3(i * 2, 2.5f, j * 2));
        }
    }


    if (!checkError("VAO/VBO")){
        std::cerr << "Error : sphere vao" << std::endl;
        return -1;
    }

    // unbind everything
    Graphics::VertexArrayObject::unbindAll();
    Graphics::VertexBufferObject::unbindAll();

    // Create Plane -------------------------------------------------------------------------------------------------------------------------------


    Graphics::Mesh planeMesh(Graphics::Mesh::genPlane(100,100,150));
    Graphics::MeshInstance planeInstances(&planeMesh);
    planeInstances.addInstance(glm::vec3(0,1,0));

    std::vector<glm::mat4> planeTransform = {planeInstances.getTransformationMatrix(0)};

    if (!checkError("VAO/VBO")){
        std::cerr << "Error : plane vao" << std::endl;
        return -1;
    }

    // Create Quad for FBO -------------------------------------------------------------------------------------------------------------------------------

    int   quad_triangleCount = 2;
    int   quad_triangleList[] = {0, 1, 2, 2, 1, 3};
    float quad_vertices[] =  {-1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0};

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

    // unbind everything
    Graphics::VertexArrayObject::unbindAll();
    Graphics::VertexBufferObject::unbindAll();

    // Create Debug Shape -------------------------------------------------------------------------------------------------------------------------------

//    std::vector<int> debugId = cubeMesh.getElementIndex();
    std::vector<int> debugId = {0,1,2,3,4,5,6,7};

    std::vector<glm::vec3> debugVertices = planeMesh.getBoundingBox().getVector();

    Graphics::VertexBufferObject debugVerticesVbo(Graphics::VEC3);
    Graphics::VertexBufferObject debugIdsVbo(Graphics::ELEMENT_ARRAY_BUFFER);
    Graphics::VertexBufferObject debugInstanceTransformVbo(Graphics::INSTANCE_TRANSFORMATION_BUFFER, 1);

    Graphics::VertexArrayObject debugVAO;
    debugVAO.addVBO(&debugVerticesVbo);
    debugVAO.addVBO(&debugIdsVbo);
    debugVAO.addVBO(&debugInstanceTransformVbo);
    debugVAO.init();

    debugVerticesVbo.updateData(debugVertices);
    debugIdsVbo.updateData(debugId);
    debugInstanceTransformVbo.updateData(planeTransform);

    // unbind everything
    Graphics::VertexArrayObject::unbindAll();
    Graphics::VertexBufferObject::unbindAll();

    if (!checkError("VAO/VBO")){
        std::cerr << "Error : debug vao" << std::endl;
        return -1;
    }

    // Create Scene -------------------------------------------------------------------------------------------------------------------------------

    Graphics::Scene scene;

    const std::string cubeInstanceName = "cube_instance";
    const std::string sphereInstanceName = "sphere_instance";
    const std::string planeInstanceName = "plane_instance";
    scene.addMeshInstance(&cubeInstances, cubeInstanceName);
    scene.addMeshInstance(&sphereInstances, sphereInstanceName);
    scene.addMeshInstance(&planeInstances, planeInstanceName);

    scene.init();

    if (!checkError("Scene")){
        std::cerr << "Error : scene" << std::endl;
        return -1;
    }

    // My GL Textures -------------------------------------------------------------------------------------------------------------------------------

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

    int shadowTexWidth = 4096;
    int shadowTexHeight = 4096;
    std::string shadowBufferTexture = "shadow_buffer_texture";
    texHandler.add(Graphics::Texture(shadowTexWidth, shadowTexHeight, Graphics::FRAMEBUFFER_DEPTH), shadowBufferTexture);
    if (!checkError("Texture")){
        std::cout << "Error : shadow_buffer_texture" << std::endl;
        return -1;
    }

    std::string beautyBufferTexture = "beauty_buffer_texture";
    texHandler.add(Graphics::Texture(width, height, Graphics::FRAMEBUFFER_RGBA), beautyBufferTexture);
    if (!checkError("Texture")){
        std::cout << "Error : beauty_buffer_texture" << std::endl;
        return -1;
    }

    const int fxTextureCount = 4;
    std::string fxBufferTexture = "fx_texture_";
    for(int i = 0; i < fxTextureCount; ++i){
        std::string currentFxBufferTexture = fxBufferTexture + std::to_string(i);
        texHandler.add(Graphics::Texture(width, height, Graphics::FRAMEBUFFER_RGBA), currentFxBufferTexture);
        if (!checkError("Texture")){
            std::cout << "Error : fx_texture" << i << std::endl;
            return -1;
        }
    }

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

    float lightAttenuation = 8;
    float lightIntensity = 1;
    float lightAttenuationThreshold = 0.01;

    std::vector<Light> pointLights;

    std::vector<Light> directionnalLights;
    directionnalLights.push_back(Light(glm::vec3(-1,-1,-1), glm::vec3(0,0.5,1), 0.3));

    std::vector<SpotLight> spotLights;
    spotLights.push_back(SpotLight(glm::vec3(-4,5,-4), glm::vec3(1,-1,1), glm::vec3(1,0.5,0), 1, 0, 60, 66));

    // My Uniforms -------------------------------------------------------------------------------------------------------------------------------
    const std::string UNIFORM_NAME_MVP              = "MVP";
    const std::string UNIFORM_NAME_MV               = "MV";
    const std::string UNIFORM_NAME_MV_INVERSE       = "MVInverse";
    const std::string UNIFORM_NAME_TIME             = "Time";
    const std::string UNIFORM_NAME_SLIDER           = "Slider";
    const std::string UNIFORM_NAME_SLIDER_MULT      = "SliderMult";
    const std::string UNIFORM_NAME_SPECULAR_POWER   = "SpecularPower";
    const std::string UNIFORM_NAME_INSTANCE_NUMBER  = "InstanceNumber";

    const std::string UNIFORM_NAME_SHADOW_MVP       = "ShadowMVP";
    const std::string UNIFORM_NAME_SHADOW_MV        = "ShadowMV";
    const std::string UNIFORM_NAME_SHADOW_BIAS      = "ShadowBias";
    const std::string UNIFORM_NAME_WOLRD_TO_LIGHT_SCREEN = "WorldToLightScreen";
    const std::string UNIFORM_NAME_SCREEN_TO_VIEW  = "ScreenToView";

    const std::string UNIFORM_NAME_FOCUS            = "Focus";
    const std::string UNIFORM_NAME_SHADOW_BUFFER    = "ShadowBuffer";
    const std::string UNIFORM_NAME_GAMMA            = "Gamma";
    const std::string UNIFORM_NAME_SOBEL_INTENSITY  = "SobelIntensity";
    const std::string UNIFORM_NAME_BLUR_SAMPLE_COUNT= "SampleCount";
    const std::string UNIFORM_NAME_BLUR_DIRECTION   = "BlurDirection";

    const std::string UNIFORM_NAME_DOF_COLOR        = "Color";
    const std::string UNIFORM_NAME_DOF_COC          = "CoC";
    const std::string UNIFORM_NAME_DOF_BLUR         = "Blur";

    const std::string UNIFORM_NAME_COLOR_BUFFER     = "ColorBuffer";
    const std::string UNIFORM_NAME_NORMAL_BUFFER    = "NormalBuffer";
    const std::string UNIFORM_NAME_DEPTH_BUFFER     = "DepthBuffer";
    const std::string UNIFORM_NAME_DIFFUSE          = "Diffuse";
    const std::string UNIFORM_NAME_SPECULAR         = "Specular";
    const std::string UNIFORM_NAME_NORMAL_MAP       = "NormalMap";
    const std::string UNIFORM_NAME_CAMERA_POSITION  = "CamPos";
    const std::string UNIFORM_NAME_NORMAL_MAP_ACTIVE = "IsNormalMapActive";


    // ---------------------- For Geometry Shading
    float t = 0;
    float SliderValue = 0.3;
    float SliderMult = 80;
    float specularPower = 20;
    float instanceNumber = 100;
    int isNormalMapActive = 1;

    mainShader.updateUniform(UNIFORM_NAME_DIFFUSE, 0);
    mainShader.updateUniform(UNIFORM_NAME_SPECULAR, 1);
    mainShader.updateUniform(UNIFORM_NAME_NORMAL_MAP, 2);
    mainShader.updateUniform(UNIFORM_NAME_INSTANCE_NUMBER, int(instanceNumber));
    mainShader.updateUniform(UNIFORM_NAME_NORMAL_MAP_ACTIVE, isNormalMapActive);
    shadowShader.updateUniform(UNIFORM_NAME_INSTANCE_NUMBER, int(instanceNumber));

    if (!checkError("Uniforms")){
        std::cerr << "Error : geometry uniforms" << std::endl;
        return -1;
    }

    // ---------------------- For Light Pass Shading
    directionalLightShader.updateUniform(UNIFORM_NAME_COLOR_BUFFER, 0);
    spotLightShader.updateUniform(UNIFORM_NAME_COLOR_BUFFER, 0);
    pointLightShader.updateUniform(UNIFORM_NAME_COLOR_BUFFER, 0);

    directionalLightShader.updateUniform(UNIFORM_NAME_NORMAL_BUFFER, 1);
    spotLightShader.updateUniform(UNIFORM_NAME_NORMAL_BUFFER, 1);
    pointLightShader.updateUniform(UNIFORM_NAME_NORMAL_BUFFER, 1);

    directionalLightShader.updateUniform(UNIFORM_NAME_DEPTH_BUFFER, 2);
    spotLightShader.updateUniform(UNIFORM_NAME_DEPTH_BUFFER, 2);
    pointLightShader.updateUniform(UNIFORM_NAME_DEPTH_BUFFER, 2);


    if (!checkError("Uniforms"))
        return(1);

    GLint blurDirectionLocation = glGetUniformLocation(blurShader.id(), "BlurDirection");
    // ---------------------- FX Variables
    float shadowBias = 0.00001;

    float gamma = 1.22;
    float sobelIntensity = 0.5;
    int sampleCount = 8; // blur
    glm::vec3 focus(0, 1, 10);


    // ---------------------- FX uniform update
    // For shadow pass shading
    spotLightShader.updateUniform(UNIFORM_NAME_SHADOW_BUFFER, 3);

    gammaShader.updateUniform(UNIFORM_NAME_GAMMA, gamma);
    sobelShader.updateUniform(UNIFORM_NAME_SOBEL_INTENSITY, sobelIntensity);
    blurShader.updateUniform(UNIFORM_NAME_BLUR_SAMPLE_COUNT, sampleCount);
    blurShader.updateUniform(UNIFORM_NAME_BLUR_DIRECTION, glm::ivec2(1,0));

    // ---------------------- For coc Correction
    circleConfusionShader.updateUniform(UNIFORM_NAME_FOCUS, focus);

    // ---------------------- For dof Correction
    depthOfFieldShader.updateUniform(UNIFORM_NAME_DOF_COLOR, 0);
    depthOfFieldShader.updateUniform(UNIFORM_NAME_DOF_COC, 1);
    depthOfFieldShader.updateUniform(UNIFORM_NAME_DOF_BLUR, 2);


    if (!checkError("Uniforms")){
        std::cout << "Error : post_fx Uniforms" << std::endl;
        return -1;
    }

    // My FBO -------------------------------------------------------------------------------------------------------------------------------

    // Framebuffer object handle
    Graphics::GeometricBuffer gBufferFBO(width, height);


    // Create Shadow & Texture FBO -------------------------------------------------------------------------------------------------------------------------------

    // Create shadow FBO
    GLuint shadowFbo;
    glGenFramebuffers(1, &shadowFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);

    // Create a render buffer since we don't need to read shadow color
    // in a texture
    GLuint shadowRenderBuffer;
    glGenRenderbuffers(1, &shadowRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, shadowRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, shadowTexWidth, shadowTexHeight);
    // Attach the renderbuffer
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, shadowRenderBuffer);

    // Attach the shadow texture to the depth attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texHandler[shadowBufferTexture].glId(), 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "Error on building shadow framebuffer\n");
        return( EXIT_FAILURE );
    }

    // Fall back to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Create Beauty FBO -------------------------------------------------------------------------------------------------------------------------------

    // Create beauty FBO
    GLuint beautyFbo;
    // Texture handles
    GLuint beautyDrawBuffer;

    // Create Framebuffer Object
    glGenFramebuffers(1, &beautyFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, beautyFbo);
    // Initialize DrawBuffers
    beautyDrawBuffer = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &beautyDrawBuffer);

    // Attach textures to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texHandler[beautyBufferTexture].glId(), 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "Error on building framebuffer\n");
        return( EXIT_FAILURE );
    }

    // Back to the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create FBO & textures For Post Processing -------------------------------------------------------------------------------------------------------------------------------

    // Create Fx Framebuffer Object
    GLuint fxFbo;
    GLuint fxDrawBuffers[1];
    glGenFramebuffers(1, &fxFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fxFbo);
    fxDrawBuffers[0] = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, fxDrawBuffers);

    // Attach first fx texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, texHandler[fxBufferTexture+std::to_string(0)].glId(), 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "Error on building framebuffern");
        return( EXIT_FAILURE );
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create UBO For Light Structures -------------------------------------------------------------------------------------------------------------------------------
    // Create two ubo for light and camera
    const GLuint LightBindingPoint = 0;
    const GLuint CameraBindingPoint = 1;

    Graphics::UBO uboLight(LightBindingPoint, sizeof(SpotLight));
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
    GUIStates guiStates;
    init_gui_states(guiStates);

    // GUI vars -------------------------------------------------------------------------------------------------------------------------------

    int logScroll = 0;

    //*********************************************************************************************
    //***************************************** MAIN LOOP *****************************************
    //*********************************************************************************************
    do
    {
        t = glfwGetTime();
        userInput.update(window);
        cameraController.update(t);


        int leftButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT );
        int rightButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT );
        int middleButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_MIDDLE);

        // Camera movements
        int altPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);

        // Get camera matrices
        glm::mat4 projection = camera.getProjectionMatrix();
        glm::mat4 worldToView = camera.getViewMatrix();
        glm::mat4 objectToWorld;
        glm::mat4 mvp = projection * worldToView * objectToWorld;
        glm::mat4 mv = worldToView * objectToWorld;
        glm::mat4 mvInverse = glm::inverse(mv);
        glm::mat4 screenToView = glm::inverse(projection);
        glm::mat4 vp = camera.getProjectionMatrix() * camera.getViewMatrix();


        // Light space matrices
        // From light space to shadow map screen space
        glm::mat4 proj = glm::perspective(glm::radians(spotLights[0]._falloff*2.f), 1.0f, 0.1f, 100.f);
        // From world to light
        glm::mat4 worldToLight = glm::lookAt(spotLights[0]._pos, spotLights[0]._pos + spotLights[0]._dir, glm::vec3(0.f, 1.f, 0.f));
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

        mainShader.updateUniform(UNIFORM_NAME_MVP, mvp);
        mainShader.updateUniform(UNIFORM_NAME_MV, mv);
        mainShader.updateUniform(UNIFORM_NAME_CAMERA_POSITION, camera.getEye());
        debugShapesShader.updateUniform(UNIFORM_NAME_MVP, mvp);
        debugShapesShader.updateUniform(UNIFORM_NAME_MV_INVERSE, mvInverse);


        // Upload value
        mainShader.updateUniform(UNIFORM_NAME_TIME, t);
        mainShader.updateUniform(UNIFORM_NAME_SLIDER, SliderValue);
        mainShader.updateUniform(UNIFORM_NAME_SLIDER_MULT, SliderMult);
        mainShader.updateUniform(UNIFORM_NAME_SPECULAR_POWER, specularPower);
        mainShader.updateUniform(UNIFORM_NAME_INSTANCE_NUMBER, int(instanceNumber));


        // Update scene uniforms
        shadowShader.updateUniform(UNIFORM_NAME_INSTANCE_NUMBER, int(instanceNumber));
        shadowShader.updateUniform(UNIFORM_NAME_SHADOW_MVP, objectToLightScreen);
        shadowShader.updateUniform(UNIFORM_NAME_SHADOW_MV, objectToLight);

        spotLightShader.updateUniform(UNIFORM_NAME_WOLRD_TO_LIGHT_SCREEN, worldToLightScreen);
        spotLightShader.updateUniform(UNIFORM_NAME_SHADOW_BIAS, shadowBias);
        gammaShader.updateUniform(UNIFORM_NAME_GAMMA, gamma);
        sobelShader.updateUniform(UNIFORM_NAME_SOBEL_INTENSITY, sobelIntensity);
        blurShader.updateUniform(UNIFORM_NAME_BLUR_SAMPLE_COUNT, sampleCount);
        circleConfusionShader.updateUniform(UNIFORM_NAME_SCREEN_TO_VIEW, screenToView);
        circleConfusionShader.updateUniform(UNIFORM_NAME_FOCUS, focus);

        //******************************************************* FIRST PASS

        //-------------------------------------Bind gbuffer
        gBufferFBO.bind();

        // Clear the gbuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //-------------------------------------Render Scene

        scene.draw(vp);

        //-------------------------------------Unbind the frambuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //******************************************************* SECOND PASS

        //-------------------------------------Shadow pass
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);

        // Set the viewport corresponding to shadow texture resolution
        glViewport(0, 0, shadowTexWidth, shadowTexHeight);

        // Clear only the depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);

        // Render the scene
        shadowShader.useProgram();

        scene.draw(worldToLightScreen);

        // Fallback to default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Revert to window size viewport
        glViewport(0, 0, width, height);

        //-------------------------------------Light Draw

        glBindFramebuffer(GL_FRAMEBUFFER, beautyFbo);
        // Clear the gbuffer
        glClear(GL_COLOR_BUFFER_BIT);

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

        //------------------------------------ Directionnal Lights

        //directionnal light shaders
        directionalLightShader.useProgram();

        // Bind quad vao
        quadVAO.bind();

        gBufferFBO.color().bind(GL_TEXTURE0);
        gBufferFBO.normal().bind(GL_TEXTURE1);
        gBufferFBO.depth().bind(GL_TEXTURE2);

        for(size_t i = 0; i < directionnalLights.size(); ++i){
            uboLight.updateBuffer(&directionnalLights[i], sizeof(Light));
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        }

//        ------------------------------------ Spot Lights

        // spot light shaders
        spotLightShader.useProgram();

        // Bind quad vao
        quadVAO.bind();
        gBufferFBO.color().bind(GL_TEXTURE0);
        gBufferFBO.normal().bind(GL_TEXTURE1);
        gBufferFBO.depth().bind(GL_TEXTURE2);
        texHandler[shadowBufferTexture].bind(GL_TEXTURE3);

        for(size_t i = 0; i < spotLights.size(); ++i){
            uboLight.updateBuffer(&spotLights[i], sizeof(SpotLight));
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        }

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
        // Set quad as vao
        quadVAO.bind();

        // ------- SOBEL ------
        glBindFramebuffer(GL_FRAMEBUFFER, fxFbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, texHandler[fxBufferTexture+std::to_string(0)].glId(), 0);
        glClear(GL_COLOR_BUFFER_BIT);

        quadVAO.bind();
        sobelShader.useProgram();
        texHandler[beautyBufferTexture].bind(GL_TEXTURE0);

        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // ------- BLUR ------
        if(sampleCount > 0){
            // Use blur program shader
            blurShader.useProgram();

            glProgramUniform2i(blurShader.id(), blurDirectionLocation, 1,0);
            // Write into Vertical Blur Texture
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, texHandler[fxBufferTexture+std::to_string(1)].glId(), 0);
            // Clear the content of texture
            glClear(GL_COLOR_BUFFER_BIT);
            // Read the texture processed by the Sobel operator
            texHandler[fxBufferTexture+std::to_string(0)].bind(GL_TEXTURE0);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

            glProgramUniform2i(blurShader.id(), blurDirectionLocation, 0,1);

            // Write into Horizontal Blur Texture
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, texHandler[fxBufferTexture+std::to_string(2)].glId(), 0);
            // Clear the content of texture
            glClear(GL_COLOR_BUFFER_BIT);
            // Read the texture processed by the Vertical Blur
            texHandler[fxBufferTexture+std::to_string(1)].bind(GL_TEXTURE0);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        }

        // ------- COC ------
        // Use circle of confusion program shader
        circleConfusionShader.useProgram();

        // Write into Circle of Confusion Texture
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, texHandler[fxBufferTexture+std::to_string(1)].glId(), 0);
        // Clear the content of  texture
        glClear(GL_COLOR_BUFFER_BIT);
        // Read the depth texture
        gBufferFBO.depth().bind(GL_TEXTURE0);

        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);


        // ------- DOF ------
        // Attach Depth of Field texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, texHandler[fxBufferTexture+std::to_string(3)].glId(), 0);

        // Only the color buffer is used
        glClear(GL_COLOR_BUFFER_BIT);
        // Use the Depth of Field shader

        depthOfFieldShader.useProgram();

        texHandler[fxBufferTexture+std::to_string(0)].bind(GL_TEXTURE0); // Color
        texHandler[fxBufferTexture+std::to_string(1)].bind(GL_TEXTURE1); // CoC
        texHandler[fxBufferTexture+std::to_string(2)].bind(GL_TEXTURE2); //Blur

        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // ------- GAMMA ------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        gammaShader.useProgram();
        texHandler[fxBufferTexture+std::to_string(3)].bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        //-------------------------------------Debug Draw

        //------------------------------------ Debug Shape Drawing

        debugShapesShader.useProgram();
        debugVAO.bind();

        glPointSize(2);

        glDrawElementsInstanced(GL_POINTS, debugVertices.size(), GL_UNSIGNED_INT, (void*)0, cubeInstanceTransform.size());

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
        texHandler[beautyBufferTexture].bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // --------------- Circle of confusion Buffer
        glViewport( 4*width/screenNumber, 0, width/screenNumber, height/screenNumber );

        quadVAO.bind();
        texHandler[fxBufferTexture+std::to_string(1)].bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // --------------- Blur Buffer
        glViewport( 5*width/screenNumber, 0, width/screenNumber, height/screenNumber );

        quadVAO.bind();
        texHandler[fxBufferTexture+std::to_string(2)].bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        //****************************************** EVENTS *******************************************
#if 1
        // Draw UI
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0, 0, width, height);

        unsigned char mbut = 0;
        int mscroll = 0;
        double mousex; double mousey;
        glfwGetCursorPos(window, &mousex, &mousey);
        mousex*=DPI;
        mousey*=DPI;
        mousey = height - mousey;

        if( leftButton == GLFW_PRESS )
            mbut |= IMGUI_MBUT_LEFT;

        imguiBeginFrame(mousex, mousey, mbut, mscroll);
        char lineBuffer[512];
        // imguiBeginScrollArea("aogl", width - 210, height - 310, 200, 300, &logScroll);

        float xwidth = 400;
        float ywidth = 550;

        imguiBeginScrollArea("aogl", width - xwidth - 10, height - ywidth - 10, xwidth, ywidth, &logScroll);
        sprintf(lineBuffer, "FPS %f", fps);
        imguiLabel(lineBuffer);

        if(imguiButton("Camera Spectator"))
            cameraController.setSpectator(!cameraController.isSpectator());
        imguiSlider("Camera splines velocity", &(cameraController.velocitySplines()), 0.0, 1.0, 0.001);
        imguiSlider("Camera angles velocity", &userInput.getVelocityRotate(), 0.0, 0.2, 0.001);

        imguiSeparatorLine();
        if(imguiButton("IsNormalMapActive"))
            mainShader.updateUniform(UNIFORM_NAME_NORMAL_MAP_ACTIVE, (isNormalMapActive = isNormalMapActive ? 0 : 1));

        imguiSlider("Slider", &SliderValue, 0.0, 1.0, 0.001);
        imguiSlider("SliderMultiply", &SliderMult, 0.0, 1000.0, 0.1);
        imguiSlider("InstanceNumber", &instanceNumber, 100, 100000, 1);
        imguiSlider("Specular Power", &specularPower, 0, 100, 0.1);
        imguiSlider("Attenuation", &lightAttenuation, 0, 16, 0.1);
        imguiSlider("Intensity", &lightIntensity, 0, 10, 0.1);
        imguiSlider("Threshold", &lightAttenuationThreshold, 0, 0.5, 0.0001);
        imguiSlider("Shadow Bias", &shadowBias, 0, 0.001, 0.00000001);
        imguiSlider("Gamma", &gamma, 1, 8, 0.01);
        imguiSlider("Sobel Intensity", &sobelIntensity, 0, 4, 0.01);
        float sample = sampleCount;
        imguiSlider("Blur Sample Count", &sample, 0, 32, 1);
        sampleCount = sample;

        imguiSlider("Focus Near", &focus[0], 0, 10, 0.01);
        imguiSlider("Focus Position", &focus[1], 0, 100, 0.01);
        imguiSlider("Focus Far", &focus[2], 0, 100, 0.01);

        imguiLabel("Spline Handler");
        for(size_t i = 0; i < cameraController.positions().size(); ++i){
            imguiLabel("point");
            imguiSlider("x", &cameraController.positions()[i].x, -100, 100, 0.1);
            imguiSlider("y", &cameraController.positions()[i].y, -100, 100, 0.1);
            imguiSlider("z", &cameraController.positions()[i].z, -100, 100, 0.1);
        }

        if(imguiButton("Add Spline")){
            cameraController.positions().add(cameraController.positions()[cameraController.positions().size()-1]);
        }

        for(size_t i = 0; i < spotLights.size(); ++i){
            imguiSlider("pos.x", &spotLights[i]._pos.x, -50, 50, 0.001);
            imguiSlider("pos.y", &spotLights[i]._pos.y, -50, 50, 0.001);
            imguiSlider("pos.z", &spotLights[i]._pos.z, -50, 50, 0.001);

            imguiSlider("dir.x", &spotLights[i]._dir.x, -1, 1, 0.001);
            imguiSlider("dir.y", &spotLights[i]._dir.y, -1, 1, 0.001);
            imguiSlider("dir.z", &spotLights[i]._dir.z, -1, 1, 0.001);

            imguiSlider("angle", &spotLights[i]._angle, 0, 180, 0.001);
            imguiSlider("falloff", &spotLights[i]._falloff, 0, 180, 0.001);
            imguiSlider("intensity", &spotLights[i]._intensity, 0, 10, 0.001);
            imguiSlider("attenuation", &spotLights[i]._attenuation, 0, 10, 0.001);
        }

        imguiEndScrollArea();
        imguiEndFrame();
        imguiRenderGLDraw(width, height);

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

void init_gui_states(GUIStates & guiStates)
{
    guiStates.panLock = false;
    guiStates.turnLock = false;
    guiStates.zoomLock = false;
    guiStates.lockPositionX = 0;
    guiStates.lockPositionY = 0;
    guiStates.camera = 0;
    guiStates.time = 0.0;
    guiStates.playing = false;
}
























