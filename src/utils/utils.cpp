//
// Created by mehdi on 18/02/16.
//

#include "utils/utils.h"
#include <iostream>
#include <glog/logging.h>
#include <GL/glew.h>

void checkErrorGL(const std::string& title) {
    int error;
    if((error = glGetError()) == GL_NO_ERROR)
        return;

    std::string errorString;
    switch(error){
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
    DLOG(ERROR) << "OpenGL Error(" << errorString.c_str() << ") " << title;
    throw;
}


void initContextWindowGL(const std::string& windowTitle, GLFWwindow*& window, int &DPI, const glm::ivec2& viewport) {
    window = nullptr;

    // Initialise GLFW
    if(!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

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
    DPI = 1;
# endif

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(viewport.x/DPI, viewport.y/DPI, "aogl", 0, 0);
    if(!window){
        glfwTerminate();
        throw std::runtime_error("Failed to open GLFW window");
    }
    glfwMakeContextCurrent(window);

    // Init glew
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    /* Problem: glewInit failed, something is seriously wrong. */
    if (err != GLEW_OK)
        throw std::runtime_error("glewInit Error: "+ std::string((char*)glewGetErrorString(err)));

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode( window, GLFW_STICKY_KEYS, GL_TRUE );

    // Enable vertical sync limit fps when > vSync(on cards that support it)
    glfwSwapInterval( 1 );
    glGetError(); // must be called at init: reset error to GL_NO_ERROR
}
