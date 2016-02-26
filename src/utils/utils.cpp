//
// Created by mehdi on 18/02/16.
//

#include "utils/utils.h"
#include <iostream>

#include <GL/glew.h>

void printMat4(const glm::mat4 &matrix) {
    std::cout << "[";
    std::cout << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << " " << matrix[0][3] <<std::endl;
    std::cout << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << " " << matrix[1][3] <<std::endl;
    std::cout << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << " " << matrix[2][3] <<std::endl;
    std::cout << matrix[3][0] << " " << matrix[3][1] << " " << matrix[3][2] << " " << matrix[2][3] <<std::endl;
    std::cout << "]";
    std::cout << std::endl;
}

void printVec3(const glm::vec3 &vec) {
    std::cout << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]" << std::endl;
}

void printVec4(const glm::vec4 &vec) {
    std::cout << "[" << vec.x << ", " << vec.y << ", " << vec.z  << ", " << vec.w << "]" << std::endl;
}

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
