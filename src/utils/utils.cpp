//
// Created by mehdi on 18/02/16.
//

#include "utils/utils.h"
#include <iostream>

#include <GL/glew.h>

bool checkErrorGL(const char* title) {
    int error;
    if((error = glGetError()) == GL_NO_ERROR)
        return true;

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
    fprintf(stdout, "OpenGL Error(%s): %s\n", errorString.c_str(), title);
    return false;
}
