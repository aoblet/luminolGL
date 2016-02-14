#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>

#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "stb/stb_image.h"
#include "imgui/imgui.h"
#include "imgui/imguiRenderGL3.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "geometry/Spline3D.h"

#include "graphics/Texture.h"
#include "graphics/TextureHandler.h"

#ifndef DEBUG_PRINT
#define DEBUG_PRINT 1
#endif

#if DEBUG_PRINT == 0
#define debug_print(FORMAT, ...) ((void)0)
#else
#ifdef _MSC_VER
#define debug_print(FORMAT, ...) \
    fprintf(stderr, "%s() in %s, line %i: " FORMAT "\n", \
        __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#else
#define debug_print(FORMAT, ...) \
    fprintf(stderr, "%s() in %s, line %i: " FORMAT "\n", \
        __func__, __FILE__, __LINE__, __VA_ARGS__)
#endif
#endif

// Font buffers
extern const unsigned char DroidSans_ttf[];
extern const unsigned int DroidSans_ttf_len;

// Shader utils
int check_link_error(GLuint program);
int check_compile_error(GLuint shader, const char ** sourceBuffer);
GLuint compile_shader(GLenum shaderType, const char * sourceBuffer, int bufferSize);
GLuint compile_shader_from_file(GLenum shaderType, const char * fileName);

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

    Light(glm::vec3 pos = glm::vec3(0,0,0), glm::vec3 color = glm::vec3(1,1,1), float intensity = 1, float attenuation = 2){
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

    UniformCamera(glm::vec3 pos, glm::mat4 screenToWorld, glm::mat4 viewToWorld){
        _pos = pos;
        _screenToWorld = screenToWorld;
        _viewToWorld = viewToWorld;
    }
};

struct Camera
{
    float radius;
    float theta;
    float phi;
    glm::vec3 o;
    glm::vec3 eye;
    glm::vec3 up;
};

void camera_defaults(Camera & c);
void camera_zoom(Camera & c, float factor);
void camera_turn(Camera & c, float phi, float theta);
void camera_trav(Camera & c, float x, float y);

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


void printVec3(glm::vec3 vec){
    std::cout << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]" << std::endl;
}

int main( int argc, char **argv )
{
    Geometry::Spline3D spline;
    spline.add(glm::vec3(0,0,0));
    spline.add(glm::vec3(10,10,10));
    spline.add(glm::vec3(20,0,20));
    spline.add(glm::vec3(30,10,30));
    spline.add(glm::vec3(40,0,0));


    int width = 1800, height= 900;
    float widthf = (float) width, heightf = (float) height;

    float fps = 0.f;

    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        exit( EXIT_FAILURE );
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
        exit( EXIT_FAILURE );
    }
    glfwMakeContextCurrent(window);

    // Init glew
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit( EXIT_FAILURE );
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
        exit(EXIT_FAILURE);
    }

    GLuint vertShaderId[4];
    GLuint fragShaderId[12];
    GLuint programObject[12];

    // -------------------- Shader0 for Geometry, Normals, and so on
    vertShaderId[0] = compile_shader_from_file(GL_VERTEX_SHADER, "../shaders/aogl.vert");
    fragShaderId[0] = compile_shader_from_file(GL_FRAGMENT_SHADER, "../shaders/aogl.frag");
    GLuint geomShaderId = compile_shader_from_file(GL_GEOMETRY_SHADER, "../shaders/aogl.geom");
    programObject[0] = glCreateProgram();
    glAttachShader(programObject[0], vertShaderId[0]);
    glAttachShader(programObject[0], geomShaderId);
    glAttachShader(programObject[0], fragShaderId[0]);
    glLinkProgram(programObject[0]);
    if (check_link_error(programObject[0]) < 0)
        exit(1);

    // -------------------- Shader1 for Debug Drawing

    vertShaderId[1] = compile_shader_from_file(GL_VERTEX_SHADER, "../shaders/blit.vert");
    fragShaderId[1] = compile_shader_from_file(GL_FRAGMENT_SHADER, "../shaders/blit.frag");
    programObject[1] = glCreateProgram();
    glAttachShader(programObject[1], vertShaderId[1]);
    glAttachShader(programObject[1], fragShaderId[1]);
    glLinkProgram(programObject[1]);
    if (check_link_error(programObject[1]) < 0)
        exit(1);

    // -------------------- Shader2 for Point Light
    fragShaderId[2] = compile_shader_from_file(GL_FRAGMENT_SHADER, "../shaders/pointLight.frag");
    programObject[2] = glCreateProgram();
    glAttachShader(programObject[2], vertShaderId[1]);
    glAttachShader(programObject[2], fragShaderId[2]);
    glLinkProgram(programObject[2]);
    if (check_link_error(programObject[2]) < 0)
        exit(1);

    // -------------------- Shader3 for Directionnal Light
    fragShaderId[3] = compile_shader_from_file(GL_FRAGMENT_SHADER, "../shaders/directionnalLight.frag");
    programObject[3] = glCreateProgram();
    glAttachShader(programObject[3], vertShaderId[1]);
    glAttachShader(programObject[3], fragShaderId[3]);
    glLinkProgram(programObject[3]);
    if (check_link_error(programObject[3]) < 0)
        exit(1);

    // -------------------- Shader4 for Spot Light
    fragShaderId[4] = compile_shader_from_file(GL_FRAGMENT_SHADER, "../shaders/spotLight.frag");
    programObject[4] = glCreateProgram();
    glAttachShader(programObject[4], vertShaderId[1]);
    glAttachShader(programObject[4], fragShaderId[4]);
    glLinkProgram(programObject[4]);
    if (check_link_error(programObject[4]) < 0)
        exit(1);

    // -------------------- Shader5 for Debug Shapes

    vertShaderId[2] = compile_shader_from_file(GL_VERTEX_SHADER, "../shaders/debug.vert");
    fragShaderId[5] = compile_shader_from_file(GL_FRAGMENT_SHADER, "../shaders/debug.frag");
    programObject[5] = glCreateProgram();
    glAttachShader(programObject[5], vertShaderId[2]);
    glAttachShader(programObject[5], fragShaderId[5]);
    glLinkProgram(programObject[5]);
    if (check_link_error(programObject[5]) < 0)
        exit(1);

    // -------------------- Shader6 for Debug Shapes

    vertShaderId[3] = compile_shader_from_file(GL_VERTEX_SHADER, "../shaders/shadow.vert");
    fragShaderId[6] = compile_shader_from_file(GL_FRAGMENT_SHADER, "../shaders/shadow.frag");
    programObject[6] = glCreateProgram();
    glAttachShader(programObject[6], vertShaderId[3]);
    glAttachShader(programObject[6], fragShaderId[6]);
    glLinkProgram(programObject[6]);
    if (check_link_error(programObject[6]) < 0)
        exit(1);

    // -------------------- Shader7 for Gamma Correction

    fragShaderId[7] = compile_shader_from_file(GL_FRAGMENT_SHADER, "../shaders/gammaCorrection.frag");
    programObject[7] = glCreateProgram();
    glAttachShader(programObject[7], vertShaderId[1]);
    glAttachShader(programObject[7], fragShaderId[7]);
    glLinkProgram(programObject[7]);
    if (check_link_error(programObject[7]) < 0)
        exit(1);

    // -------------------- Shader8 for Sobel

    fragShaderId[8] = compile_shader_from_file(GL_FRAGMENT_SHADER, "../shaders/sobel.frag");
    programObject[8] = glCreateProgram();
    glAttachShader(programObject[8], vertShaderId[1]);
    glAttachShader(programObject[8], fragShaderId[8]);
    glLinkProgram(programObject[8]);
    if (check_link_error(programObject[8]) < 0)
        exit(1);

    // -------------------- Shader9 for Blur

    fragShaderId[9] = compile_shader_from_file(GL_FRAGMENT_SHADER, "../shaders/blur.frag");
    programObject[9] = glCreateProgram();
    glAttachShader(programObject[9], vertShaderId[1]);
    glAttachShader(programObject[9], fragShaderId[9]);
    glLinkProgram(programObject[9]);
    if (check_link_error(programObject[9]) < 0)
        exit(1);

    // -------------------- Shader10 for Circle of Confusion

    fragShaderId[10] = compile_shader_from_file(GL_FRAGMENT_SHADER, "../shaders/coc.frag");
    programObject[10] = glCreateProgram();
    glAttachShader(programObject[10], vertShaderId[1]);
    glAttachShader(programObject[10], fragShaderId[10]);
    glLinkProgram(programObject[10]);
    if (check_link_error(programObject[10]) < 0)
        exit(1);

    // -------------------- Shader11 for Circle of Confusion

    fragShaderId[11] = compile_shader_from_file(GL_FRAGMENT_SHADER, "../shaders/dof.frag");
    programObject[11] = glCreateProgram();
    glAttachShader(programObject[11], vertShaderId[1]);
    glAttachShader(programObject[11], fragShaderId[11]);
    glLinkProgram(programObject[11]);
    if (check_link_error(programObject[11]) < 0)
        exit(1);

    // Viewport 
    glViewport( 0, 0, width, height );

    // Create Vao & vbo -------------------------------------------------------------------------------------------------------------------------------

    GLuint vao[4];
    glGenVertexArrays(4, vao);

    GLuint vbo[12];
    glGenBuffers(12, vbo);

    // Create Cube -------------------------------------------------------------------------------------------------------------------------------
    int cube_triangleCount = 12;
    int cube_triangleList[] = {0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
    float cube_uvs[] = {0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f,  };
    float cube_vertices[] = {-0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
    float cube_normals[] = {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };

    // Bind the VAO
    glBindVertexArray(vao[0]);

    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_triangleList), cube_triangleList, GL_STATIC_DRAW);

    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    // Bind normals and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);

    // Bind uv coords and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_uvs), cube_uvs, GL_STATIC_DRAW);

    // Unbind everything
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



    // Create Plane -------------------------------------------------------------------------------------------------------------------------------

    int plane_triangleCount = 2;
    float textureLoop = 15;
    int plane_triangleList[] = {0, 1, 2, 2, 1, 3};
    float plane_uvs[] = {0.f, 0.f, 0.f, textureLoop, textureLoop, 0.f, textureLoop, textureLoop};
    float plane_vertices[] = {-5.0, 0, 5.0, 5.0, 0, 5.0, -5.0, 0, -5.0, 5.0, 0, -5.0};
    float plane_normals[] = {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};

    // Bind the VAO
    glBindVertexArray(vao[1]);

    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_triangleList), plane_triangleList, GL_STATIC_DRAW);

    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);

    // Bind normals and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_normals), plane_normals, GL_STATIC_DRAW);

    // Bind uv coords and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_uvs), plane_uvs, GL_STATIC_DRAW);

    // Unbind everything
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // My GL Textures -------------------------------------------------------------------------------------------------------------------------------

    std::cout << "--------------- TEXTURES --------------- " << std::endl;
    std::cout << std::endl;

    Graphics::TextureHandler texHandler;

    std::string TexBricksDiff = "bricks_diff";
    std::string TexBricksSpec = "bricks_spec";
    texHandler.add(Graphics::Texture("../assets/textures/spnza_bricks_a_diff.tga"), TexBricksDiff);

    if (!checkError("Texture")){
        std::cout << "Error : bricks_diff" << std::endl;
        return -1;
    }

    texHandler.add(Graphics::Texture("../assets/textures/spnza_bricks_a_spec.tga"), TexBricksSpec);

    if (!checkError("Texture")){
        std::cout << "Error : bricks_spec" << std::endl;
        return -1;
    }

    std::string colorBufferTexture = "color_buffer_texture";
    texHandler.add(Graphics::Texture(width, height, Graphics::FRAMEBUFFER_RGBA), colorBufferTexture);

    if (!checkError("Texture")){
        std::cout << "Error : color_buffer_texture" << std::endl;
        return -1;
    }

    std::string normalBufferTexture = "normal_buffer_texture";
    Graphics::TexParams params(GL_RGBA8, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST, false);
    texHandler.add(Graphics::Texture(width, height, params), normalBufferTexture);

    if (!checkError("Texture")){
        std::cout << "Error : color_buffer_texture" << std::endl;
        return -1;
    }

    std::string depthBufferTexture = "depth_buffer_texture";
    texHandler.add(Graphics::Texture(width, height, Graphics::FRAMEBUFFER_DEPTH), depthBufferTexture);

    if (!checkError("Texture")){
        std::cout << "Error : depth_buffer_texture" << std::endl;
        return -1;
    }

    int shadowTexWidth = 2048;
    int shadowTexHeight = 2048;
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

    std::cout << std::endl;
    std::cout << "---------------------------------------- " << std::endl;
    std::cout << std::endl;


    // My Lights -------------------------------------------------------------------------------------------------------------------------------

    float lightAttenuation = 8;
    float lightIntensity = 1;
    float lightAttenuationThreshold = 0.01;

    std::vector<Light> pointLights;

    std::vector<Light> directionnalLights;
    directionnalLights.push_back(Light(glm::vec3(-1,-1,-1), glm::vec3(0,0.5,1), 0.2));

    std::vector<SpotLight> spotLights;
    spotLights.push_back(SpotLight(glm::vec3(-4,5,-4), glm::vec3(1,-1,1), glm::vec3(1,0.5,0), 1, 0, 60, 66));

    // My Uniforms -------------------------------------------------------------------------------------------------------------------------------


    // ---------------------- For Geometry Shading
    GLint mvpLocation = glGetUniformLocation(programObject[0], "MVP");
    GLint mvLocation = glGetUniformLocation(programObject[0], "MV");

    GLint mvInverseLocation = glGetUniformLocation(programObject[1], "MVInverse");

    float t = 0;
    GLint timeLocation = glGetUniformLocation(programObject[0], "Time");

    float SliderValue = 0.3;
    GLint sliderLocation = glGetUniformLocation(programObject[0], "Slider");

    float SliderMult = 80;
    GLint sliderMultLocation = glGetUniformLocation(programObject[0], "SliderMult");

    float specularPower = 20;
    GLint specularPowerLocation = glGetUniformLocation(programObject[0], "SpecularPower");

    GLint diffuseLocation = glGetUniformLocation(programObject[0], "Diffuse");
    glProgramUniform1i(programObject[0], diffuseLocation, 0);

    GLint specularLocation = glGetUniformLocation(programObject[0], "Specular");
    glProgramUniform1i(programObject[0], specularLocation, 1);

    float instanceNumber = 100;
    GLint instanceNumberLocation = glGetUniformLocation(programObject[0], "InstanceNumber");
    glProgramUniform1i(programObject[0], instanceNumberLocation, int(instanceNumber));

    GLint instanceNumberShadowLocation = glGetUniformLocation(programObject[6], "InstanceNumber");
    glProgramUniform1i(programObject[6], instanceNumberShadowLocation, int(instanceNumber));

    if (!checkError("Uniforms")){
        std::cout << "Error : geometry uniforms" << std::endl;
        return -1;
    }

    // ---------------------- For Light Pass Shading

    for(int i = 2; i < 5; ++i){
        GLint colorBufferLocation = glGetUniformLocation(programObject[i], "ColorBuffer");
        glProgramUniform1i(programObject[i], colorBufferLocation, 0);

        GLint normalBufferLocation = glGetUniformLocation(programObject[i], "NormalBuffer");
        glProgramUniform1i(programObject[i], normalBufferLocation, 1);

        GLint depthBufferLocation = glGetUniformLocation(programObject[i], "DepthBuffer");
        glProgramUniform1i(programObject[i], depthBufferLocation, 2);
    }

    if (!checkError("Uniforms")){
        std::cout << "Error : light_pass uniforms" << std::endl;
        return -1;
    }

    // ---------------------- For Debug Shading

    GLint mvpDebugLocation = glGetUniformLocation(programObject[5], "MVP");

    if (!checkError("Uniforms")){
        std::cout << "Error : debug uniforms" << std::endl;
        return -1;
    }

    // ---------------------- For Shadow Pass Shading

    GLint shadowMVPLocation = glGetUniformLocation(programObject[6], "ShadowMVP");
    GLint shadowMVLocation = glGetUniformLocation(programObject[6], "ShadowMV");

    GLint shadowWorldToLightScreenLocation = glGetUniformLocation(programObject[4], "WorldToLightScreen");

    GLint shadowBufferLocation = glGetUniformLocation(programObject[4], "ShadowBuffer");
    glProgramUniform1i(programObject[4], shadowBufferLocation, 3);

    float shadowBias = 0.00001;
    GLint shadowBiasLocation = glGetUniformLocation(programObject[4], "ShadowBias");

    if (!checkError("Uniforms")){
        std::cout << "Error : shadow_pass uniforms" << std::endl;
        return -1;
    }

    // ---------------------- For Gamma Correction

    float gamma = 1.22;
    GLint gammaLocation = glGetUniformLocation(programObject[7], "Gamma");
    glProgramUniform1f(programObject[7], gammaLocation, gamma);


    // ---------------------- For Sobel Correction

    float sobelIntensity = 0.5;
    GLint sobelIntensityLocation = glGetUniformLocation(programObject[8], "SobelIntensity");
    glProgramUniform1f(programObject[8], sobelIntensityLocation, sobelIntensity);

    // ---------------------- For Blur Correction

    int sampleCount = 8;
    GLint sampleCountLocation = glGetUniformLocation(programObject[9], "SampleCount");
    glProgramUniform1i(programObject[9], sampleCountLocation, sampleCount);

    int blurDirection[2] = {1,0};
    GLint blurDirectionLocation = glGetUniformLocation(programObject[9], "BlurDirection");
    glProgramUniform2i(programObject[9], blurDirectionLocation, blurDirection[0], blurDirection[1]);

    // ---------------------- For coc Correction

    GLint screenToViewLocation = glGetUniformLocation(programObject[10], "ScreenToView");

    glm::vec3 focus(0, 1, 10);
    GLint focusLocation = glGetUniformLocation(programObject[10], "Focus");
    glProgramUniform3f(programObject[10], focusLocation, focus[0], focus[1], focus[2]);

    // ---------------------- For dof Correction

    GLint colorBufferDOFLocation = glGetUniformLocation(programObject[11], "Color");
    glProgramUniform1i(programObject[11], colorBufferDOFLocation, 0);

    GLint coCBufferDOFLocation = glGetUniformLocation(programObject[11], "CoC");
    glProgramUniform1i(programObject[11], coCBufferDOFLocation, 1);

    GLint blurBufferDOFLocation = glGetUniformLocation(programObject[11], "Blur");
    glProgramUniform1i(programObject[11], blurBufferDOFLocation, 2);


    if (!checkError("Uniforms")){
        std::cout << "Error : post_fx Uniforms" << std::endl;
        return -1;
    }

    // My FBO -------------------------------------------------------------------------------------------------------------------------------

    // Framebuffer object handle
    GLuint gbufferFbo;
    // 2 draw buffers for color and normal
    GLuint gbufferDrawBuffers[2];

    // Create Framebuffer Object
    glGenFramebuffers(1, &gbufferFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gbufferFbo);
    // Initialize DrawBuffers
    gbufferDrawBuffers[0] = GL_COLOR_ATTACHMENT0;
    gbufferDrawBuffers[1] = GL_COLOR_ATTACHMENT1;
    glDrawBuffers(2, gbufferDrawBuffers);

    // Attach textures to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texHandler[colorBufferTexture].glId(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texHandler[normalBufferTexture].glId(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texHandler[depthBufferTexture].glId(), 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "Error on building framebuffer\n");
        exit( EXIT_FAILURE );
    }

    // Back to the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


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
        exit( EXIT_FAILURE );
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
        exit( EXIT_FAILURE );
    }

    // Back to the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Create Quad for FBO -------------------------------------------------------------------------------------------------------------------------------

    int   quad_triangleCount = 2;
    int   quad_triangleList[] = {0, 1, 2, 2, 1, 3};
    float quad_vertices[] =  {-1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0};

    // Quad
    glBindVertexArray(vao[2]);
    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[8]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_triangleList), quad_triangleList, GL_STATIC_DRAW);
    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[9]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create Debug Shape -------------------------------------------------------------------------------------------------------------------------------

    std::vector<int> verticesId;
    verticesId.push_back(0);
    verticesId.push_back(1);
    verticesId.push_back(2);
    verticesId.push_back(3);

    std::vector<glm::vec3> vertices;
    vertices.push_back(glm::vec3(1));
    vertices.push_back(glm::vec3(2));
    vertices.push_back(glm::vec3(3));
    vertices.push_back(glm::vec3(4));

    // Quad
    glBindVertexArray(vao[3]);
    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[10]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * verticesId.size(), verticesId.data(), GL_STATIC_DRAW);
    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[11]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
        fprintf(stderr, "Error on building framebuffer");
        exit( EXIT_FAILURE );
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create UBO For Light Structures -------------------------------------------------------------------------------------------------------------------------------

    // Create two ubo for light and camera
    GLuint ubo[2];
    glGenBuffers(2, ubo);

    // LIGHT
    GLuint PointLightUniformIndex = glGetUniformBlockIndex(programObject[2], "Light");
    GLuint DirectionnalLightUniformIndex = glGetUniformBlockIndex(programObject[3], "Light");
    GLuint SpotLightUniformIndex = glGetUniformBlockIndex(programObject[4], "Light");

    GLuint LightBindingPoint = 0;

    glUniformBlockBinding(programObject[2], PointLightUniformIndex, LightBindingPoint);
    glUniformBlockBinding(programObject[3], DirectionnalLightUniformIndex, LightBindingPoint);
    glUniformBlockBinding(programObject[4], SpotLightUniformIndex, LightBindingPoint);

    glBindBuffer(GL_UNIFORM_BUFFER, ubo[0]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotLight), 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, LightBindingPoint, ubo[0], 0, sizeof(SpotLight));

    // CAM
    GLuint CamUniformIndex1 = glGetUniformBlockIndex(programObject[2], "Camera");
    GLuint CamUniformIndex2 = glGetUniformBlockIndex(programObject[3], "Camera");
    GLuint CamUniformIndex3 = glGetUniformBlockIndex(programObject[4], "Camera");

    GLuint CameraBindingPoint = 1;

    glUniformBlockBinding(programObject[2], CamUniformIndex1, CameraBindingPoint);
    glUniformBlockBinding(programObject[3], CamUniformIndex2, CameraBindingPoint);
    glUniformBlockBinding(programObject[4], CamUniformIndex3, CameraBindingPoint);

    glBindBuffer(GL_UNIFORM_BUFFER, ubo[1]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformCamera), 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, CameraBindingPoint, ubo[1], 0, sizeof(UniformCamera));

    // Viewer Structures ----------------------------------------------------------------------------------------------------------------------
    Camera camera;
    camera_defaults(camera);
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

//        camera.eye = spline.cubicInterpolation(glm::mod(t*0.05f,1.f));

        // Mouse states
        int leftButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT );
        int rightButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT );
        int middleButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_MIDDLE );

        if( leftButton == GLFW_PRESS )
            guiStates.turnLock = true;
        else
            guiStates.turnLock = false;

        if( rightButton == GLFW_PRESS )
            guiStates.zoomLock = true;
        else
            guiStates.zoomLock = false;

        if( middleButton == GLFW_PRESS )
            guiStates.panLock = true;
        else
            guiStates.panLock = false;

        // Camera movements
        int altPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
        if (!altPressed && (leftButton == GLFW_PRESS || rightButton == GLFW_PRESS || middleButton == GLFW_PRESS))
        {
            double x; double y;
            glfwGetCursorPos(window, &x, &y);
            guiStates.lockPositionX = x;
            guiStates.lockPositionY = y;
        }
        if (altPressed == GLFW_PRESS)
        {
            double mousex; double mousey;
            glfwGetCursorPos(window, &mousex, &mousey);
            int diffLockPositionX = mousex - guiStates.lockPositionX;
            int diffLockPositionY = mousey - guiStates.lockPositionY;
            if (guiStates.zoomLock)
            {
                float zoomDir = 0.0;
                if (diffLockPositionX > 0)
                    zoomDir = -1.f;
                else if (diffLockPositionX < 0 )
                    zoomDir = 1.f;
                camera_zoom(camera, zoomDir * GUIStates::MOUSE_ZOOM_SPEED);
            }
            else if (guiStates.turnLock)
            {
                camera_turn(camera, diffLockPositionY * GUIStates::MOUSE_TURN_SPEED,
                            diffLockPositionX * GUIStates::MOUSE_TURN_SPEED);

            }
            else if (guiStates.panLock)
            {
                camera_trav(camera, diffLockPositionX * GUIStates::MOUSE_PAN_SPEED,
                            diffLockPositionY * GUIStates::MOUSE_PAN_SPEED);
            }
            guiStates.lockPositionX = mousex;
            guiStates.lockPositionY = mousey;
        }

//        float speed = 0.005;
//        float travX = speed;
//        float travY = 0;
//
//        float turnX = 0.0025 * glm::sin(0.7*t);
//        float turnY = speed;
//
//        camera_trav(camera, travX, travY);
//        camera_turn(camera, turnX, turnY);

        // Get camera matrices
        glm::mat4 projection = glm::perspective(45.0f, widthf / heightf, 0.1f, 10000.f);
        glm::mat4 worldToView = glm::lookAt(camera.eye, camera.o, camera.up);
        glm::mat4 objectToWorld;
        glm::mat4 mvp = projection * worldToView * objectToWorld;
        glm::mat4 mv = worldToView * objectToWorld;
        glm::mat4 mvInverse = glm::inverse(mv);
        glm::mat4 screenToView = glm::inverse(projection);

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
        glUseProgram(programObject[0]);

        //-------------------------------------Upload Uniforms

        glProgramUniformMatrix4fv(programObject[0], mvpLocation, 1, 0, glm::value_ptr(mvp));
        glProgramUniformMatrix4fv(programObject[5], mvpDebugLocation, 1, 0, glm::value_ptr(mvp));
        glProgramUniformMatrix4fv(programObject[0], mvLocation, 1, 0, glm::value_ptr(mv));
        glProgramUniformMatrix4fv(programObject[1], mvInverseLocation, 1, 0, glm::value_ptr(mvInverse));

        // Upload value
        glProgramUniform1f(programObject[0], timeLocation, t);
        glProgramUniform1f(programObject[0], sliderLocation, SliderValue);
        glProgramUniform1f(programObject[0], sliderMultLocation, SliderMult);

        glProgramUniform1f(programObject[0], specularPowerLocation,specularPower);
        glProgramUniform1i(programObject[0], instanceNumberLocation, int(instanceNumber));

        glProgramUniform1i(programObject[6], instanceNumberShadowLocation, int(instanceNumber));

        // Update scene uniforms
        glProgramUniformMatrix4fv(programObject[6], shadowMVPLocation, 1, 0, glm::value_ptr(objectToLightScreen));
        glProgramUniformMatrix4fv(programObject[6], shadowMVLocation, 1, 0, glm::value_ptr(objectToLight));
        glProgramUniformMatrix4fv(programObject[4], shadowWorldToLightScreenLocation, 1, 0, glm::value_ptr(worldToLightScreen));

        glProgramUniform1f(programObject[4], shadowBiasLocation, shadowBias);

        glProgramUniform1f(programObject[7], gammaLocation, gamma);

        glProgramUniform1f(programObject[8], sobelIntensityLocation, sobelIntensity);

        glProgramUniform1i(programObject[9], sampleCountLocation, sampleCount);

        glProgramUniformMatrix4fv(programObject[10], screenToViewLocation, 1, 0, glm::value_ptr(screenToView));

        glProgramUniform3f(programObject[10], focusLocation, focus[0], focus[1], focus[2]);

        //******************************************************* FIRST PASS

        //-------------------------------------Bind gbuffer

        glBindFramebuffer(GL_FRAMEBUFFER, gbufferFbo);
        // Clear the gbuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //-------------------------------------Render Cubes

        glBindVertexArray(vao[0]);
        texHandler[TexBricksDiff].bind(GL_TEXTURE0);
        texHandler[TexBricksSpec].bind(GL_TEXTURE1);
        glDrawElementsInstanced(GL_TRIANGLES, cube_triangleCount * 3, GL_UNSIGNED_INT, (void*)0, int(instanceNumber));

        //-------------------------------------Render Plane

        glProgramUniform1i(programObject[0], instanceNumberLocation, -1);

        glBindVertexArray(vao[1]);
        texHandler[TexBricksDiff].bind(GL_TEXTURE0);
        texHandler[TexBricksSpec].bind(GL_TEXTURE1);
        glDrawElements(GL_TRIANGLES, plane_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        glBindTexture(GL_TEXTURE_2D, 0);

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
        glUseProgram(programObject[6]);

        //cubes
        glBindVertexArray(vao[0]);
        glDrawElementsInstanced(GL_TRIANGLES, cube_triangleCount * 3, GL_UNSIGNED_INT, (void*)0, int(instanceNumber));

        //plane
        glProgramUniform1i(programObject[6], instanceNumberShadowLocation, -1);

        glBindVertexArray(vao[1]);
        glDrawElements(GL_TRIANGLES, plane_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

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
        UniformCamera cam(camera.eye, glm::inverse(mvp), mvInverse);

        glBindBuffer(GL_UNIFORM_BUFFER, ubo[1]);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UniformCamera), &cam);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        //------------------------------------ Point Lights
//        // point light shaders
//        glUseProgram(programObject[2]);
//
//        // Bind quad vao
//        glBindVertexArray(vao[2]);
//
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);
//        glActiveTexture(GL_TEXTURE2);
//        glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);
//
//        unsigned int nbLightsByCircle[] = {6, 12, 18, 24, 30, 36, 42, 48, 54, 60, 66, 72, 78};
//        int counterCircle = 0;
//        unsigned int nbPointLights = 30;
//        float xOffset = glm::sqrt(float(instanceNumber))/2;
//        float zOffset = glm::sqrt(float(instanceNumber))/2;
//
//        float rayon = sqrt(xOffset*2 + zOffset*2);
//
//
//        int cptVisiblePointLight = 0;
//
//        std::vector<Light> lights;
//
//        for(size_t i = 0; i < nbPointLights; ++i){
//
//            Light light(glm::vec3(0,0,0), glm::vec3(1,1,1), lightIntensity, lightAttenuation);
//
//            if( i == nbLightsByCircle[counterCircle] ){
//              counterCircle++;
//              rayon += 3;
//            }
//
//
//            float coeff = rayon * sin(t);
//            float w = t + t;
//
////            coeff = 20;
////            w = 0;
//
//            light._pos = glm::vec3(
//                coeff * cos(i+ M_PI /nbPointLights) + xOffset,
//                pointLightsYOffset,
//                coeff * sin(i+ M_PI /nbPointLights) + zOffset);
//
//            light._color.x = cos(i);
//            light._color.y = sin(3*i);
//            light._color.y = cos(i*2);
//
//
//            glBindBuffer(GL_UNIFORM_BUFFER, ubo[0]);
//            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Light), &light);
//            glBindBuffer(GL_UNIFORM_BUFFER, 0);
//
////            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
//
//            lights.push_back(light);
//
//        }
//

//
        //------------------------------------ Directionnal Lights

        //directionnal light shaders
        glUseProgram(programObject[3]);

        // Bind quad vao
        glBindVertexArray(vao[2]);

        texHandler[colorBufferTexture].bind(GL_TEXTURE0);
        texHandler[normalBufferTexture].bind(GL_TEXTURE1);
        texHandler[depthBufferTexture].bind(GL_TEXTURE2);

        for(size_t i = 0; i < directionnalLights.size(); ++i){

            glBindBuffer(GL_UNIFORM_BUFFER, ubo[0]);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Light), &directionnalLights[i]);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        }

//        ------------------------------------ Spot Lights

        // spot light shaders
        glUseProgram(programObject[4]);

        // Bind quad vao
        glBindVertexArray(vao[2]);

        texHandler[colorBufferTexture].bind(GL_TEXTURE0);
        texHandler[normalBufferTexture].bind(GL_TEXTURE1);
        texHandler[depthBufferTexture].bind(GL_TEXTURE2);
        texHandler[shadowBufferTexture].bind(GL_TEXTURE3);

        for(size_t i = 0; i < spotLights.size(); ++i){
            glBindBuffer(GL_UNIFORM_BUFFER, ubo[0]);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SpotLight), &spotLights[i]);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

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
        glBindVertexArray(vao[2]);

        // ------- SOBEL ------
        glBindFramebuffer(GL_FRAMEBUFFER, fxFbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, texHandler[fxBufferTexture+std::to_string(0)].glId(), 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao[2]);

        glUseProgram(programObject[8]);

        texHandler[beautyBufferTexture].bind(GL_TEXTURE0);

        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // ------- BLUR ------
        if(sampleCount > 0){
            // Use blur program shader
            glUseProgram(programObject[9]);

            glProgramUniform2i(programObject[9], blurDirectionLocation, 1,0);
            // Write into Vertical Blur Texture
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, texHandler[fxBufferTexture+std::to_string(1)].glId(), 0);
            // Clear the content of texture
            glClear(GL_COLOR_BUFFER_BIT);
            // Read the texture processed by the Sobel operator
            texHandler[fxBufferTexture+std::to_string(0)].bind(GL_TEXTURE0);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

            glProgramUniform2i(programObject[9], blurDirectionLocation, 0,1);

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
        glUseProgram(programObject[10]);

        // Write into Circle of Confusion Texture
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, texHandler[fxBufferTexture+std::to_string(1)].glId(), 0);
        // Clear the content of  texture
        glClear(GL_COLOR_BUFFER_BIT);
        // Read the depth texture
        texHandler[depthBufferTexture].bind(GL_TEXTURE0);


        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);


        // ------- DOF ------
        // Attach Depth of Field texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, texHandler[fxBufferTexture+std::to_string(3)].glId(), 0);

        // Only the color buffer is used
        glClear(GL_COLOR_BUFFER_BIT);
        // Use the Depth of Field shader
        glUseProgram(programObject[11]);

        texHandler[fxBufferTexture+std::to_string(0)].bind(GL_TEXTURE0); // Color
        texHandler[fxBufferTexture+std::to_string(1)].bind(GL_TEXTURE1); // CoC
        texHandler[fxBufferTexture+std::to_string(2)].bind(GL_TEXTURE2); //Blur

        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // ------- GAMMA ------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(programObject[7]);
        texHandler[fxBufferTexture+std::to_string(3)].bind(GL_TEXTURE0);

        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        //-------------------------------------Debug Draw

        //------------------------------------ Debug Shape Drawing

        glUseProgram(programObject[5]);
        glPointSize(10);
        glBindVertexArray(vao[3]);

        int id = 0;

        vertices.clear();
        verticesId.clear();

        for(float i = 0; i < 1; i +=0.01){
            verticesId.push_back(id);
            ++id;
            vertices.push_back(spline.cubicInterpolation(i));
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo[11]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[10]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * verticesId.size(), verticesId.data(), GL_STATIC_DRAW);

        glDrawElements(GL_LINE_STRIP, vertices.size(), GL_UNSIGNED_INT, (void*)0);

        int screenNumber = 6;

        glDisable(GL_DEPTH_TEST);
        glViewport( 0, 0, width/screenNumber, height/screenNumber );

        // Select shader
        glUseProgram(programObject[1]);

        // --------------- Color Buffer

        // Bind quad VAO
        glBindVertexArray(vao[2]);

//        glActiveTexture(GL_TEXTURE0);
        // Bind gbuffer color texture
//        glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);
        texHandler[colorBufferTexture].bind(GL_TEXTURE0);
        // Draw quad
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // --------------- Normal Buffer
        glViewport( width/screenNumber, 0, width/screenNumber, height/screenNumber );

        // Bind quad VAO
        glBindVertexArray(vao[2]);

//        glActiveTexture(GL_TEXTURE0);
        // Bind gbuffer color texture
//        glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);
        texHandler[normalBufferTexture].bind(GL_TEXTURE0);
        // Draw quad
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // --------------- Depth Buffer
        glViewport( 2*width/screenNumber, 0, width/screenNumber, height/screenNumber );

        // Bind quad VAO
        glBindVertexArray(vao[2]);

//        glActiveTexture(GL_TEXTURE0);
        // Bind gbuffer color texture
//        glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);
        texHandler[depthBufferTexture].bind(GL_TEXTURE0);

        // Draw quad
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // --------------- Beauty Buffer
        glViewport( 3*width/screenNumber, 0, width/screenNumber, height/screenNumber );

        // Bind quad VAO
        glBindVertexArray(vao[2]);

        glActiveTexture(GL_TEXTURE0);
        // Bind gbuffer color texture
//        glBindTexture(GL_TEXTURE_2D, beautyTexture);
        texHandler[beautyBufferTexture].bind();
        // Draw quad
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // --------------- Circle of confusion Buffer
        glViewport( 4*width/screenNumber, 0, width/screenNumber, height/screenNumber );

        // Bind quad VAO
        glBindVertexArray(vao[2]);

//        glActiveTexture(GL_TEXTURE0);
        // Bind gbuffer color texture
//        glBindTexture(GL_TEXTURE_2D, fxTextures[1]);
        texHandler[fxBufferTexture+std::to_string(1)].bind(GL_TEXTURE0);
        // Draw quad
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // --------------- Blur Buffer
        glViewport( 5*width/screenNumber, 0, width/screenNumber, height/screenNumber );

        // Bind quad VAO
        glBindVertexArray(vao[2]);

//        glActiveTexture(GL_TEXTURE0);
        // Bind gbuffer color texture
//        glBindTexture(GL_TEXTURE_2D, fxTextures[2]);
        texHandler[fxBufferTexture+std::to_string(2)].bind(GL_TEXTURE0);
        // Draw quad
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
        for(size_t i = 0; i < spline.size(); ++i){
            imguiLabel("point");
            imguiSlider("x", &spline[i].x, -100, 100, 0.1);
            imguiSlider("y", &spline[i].y, -100, 100, 0.1);
            imguiSlider("z", &spline[i].z, -100, 100, 0.1);
        }

        if(imguiButton("Add Spline")){
            spline.add(spline[spline.size()-1]);
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

    exit( EXIT_SUCCESS );
}

// No windows implementation of strsep
char * strsep_custom(char **stringp, const char *delim)
{
    register char *s;
    register const char *spanp;
    register int c, sc;
    char *tok;
    if ((s = *stringp) == NULL)
        return (NULL);
    for (tok = s; ; ) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
    return 0;
}

int check_compile_error(GLuint shader, const char ** sourceBuffer)
{
    // Get error log size and print it eventually
    int logLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 1)
    {
        char * log = new char[logLength];
        glGetShaderInfoLog(shader, logLength, &logLength, log);
        char *token, *string;
        string = strdup(sourceBuffer[0]);
        int lc = 0;
        while ((token = strsep_custom(&string, "\n")) != NULL) {
            printf("%3d : %s\n", lc, token);
            ++lc;
        }
        fprintf(stderr, "Compile : %s", log);
        delete[] log;
    }
    // If an error happend quit
    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
        return -1;
    return 0;
}

int check_link_error(GLuint program)
{
    // Get link error log size and print it eventually
    int logLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 1)
    {
        char * log = new char[logLength];
        glGetProgramInfoLog(program, logLength, &logLength, log);
        fprintf(stderr, "Link : %s \n", log);
        delete[] log;
    }
    int status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
        return -1;
    return 0;
}


GLuint compile_shader(GLenum shaderType, const char * sourceBuffer, int bufferSize)
{
    GLuint shaderObject = glCreateShader(shaderType);
    const char * sc[1] = { sourceBuffer };
    glShaderSource(shaderObject,
                   1,
                   sc,
                   NULL);
    glCompileShader(shaderObject);
    check_compile_error(shaderObject, sc);
    return shaderObject;
}

GLuint compile_shader_from_file(GLenum shaderType, const char * path)
{
    FILE * shaderFileDesc = fopen( path, "rb" );
    if (!shaderFileDesc)
        return 0;
    fseek ( shaderFileDesc , 0 , SEEK_END );
    long fileSize = ftell ( shaderFileDesc );
    rewind ( shaderFileDesc );
    char * buffer = new char[fileSize + 1];
    fread( buffer, 1, fileSize, shaderFileDesc );
    buffer[fileSize] = '\0';
    GLuint shaderObject = compile_shader(shaderType, buffer, fileSize );
    delete[] buffer;
    return shaderObject;
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

void camera_compute(Camera & c)
{
    c.eye.x = cos(c.theta) * sin(c.phi) * c.radius + c.o.x;
    c.eye.y = cos(c.phi) * c.radius + c.o.y ;
    c.eye.z = sin(c.theta) * sin(c.phi) * c.radius + c.o.z;
    c.up = glm::vec3(0.f, c.phi < M_PI ?1.f:-1.f, 0.f);
}

void camera_defaults(Camera & c)
{
    c.phi = 3.14/2.f;
    c.theta = 3.14/2.f;
    c.radius = 10.f;
    camera_compute(c);
}

void camera_zoom(Camera & c, float factor)
{
    c.radius += factor * c.radius ;
    if (c.radius < 0.1)
    {
        c.radius = 10.f;
        c.o = c.eye + glm::normalize(c.o - c.eye) * c.radius;
    }
    camera_compute(c);
}

void camera_turn(Camera & c, float phi, float theta)
{
    c.theta += 1.f * theta;
    c.phi   -= 1.f * phi;
    if (c.phi >= (2 * M_PI) - 0.1 )
        c.phi = 0.00001;
    else if (c.phi <= 0 )
        c.phi = 2 * M_PI - 0.1;
    camera_compute(c);
}

void camera_trav(Camera & c, float x, float y)
{
    glm::vec3 up(0.f, c.phi < M_PI ?1.f:-1.f, 0.f);
    glm::vec3 fwd = glm::normalize(c.o - c.eye);
    glm::vec3 side = glm::normalize(glm::cross(fwd, up));
    c.up = glm::normalize(glm::cross(side, fwd));
    c.o[0] += up[0] * y * c.radius * 2;
    c.o[1] += up[1] * y * c.radius * 2;
    c.o[2] += up[2] * y * c.radius * 2;
    c.o[0] -= side[0] * x * c.radius * 2;
    c.o[1] -= side[1] * x * c.radius * 2;
    c.o[2] -= side[2] * x * c.radius * 2;
    camera_compute(c);
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
























