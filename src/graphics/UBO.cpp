#include "graphics/UBO.hpp"
#include <cstring>
#include <iostream>

using namespace Graphics;

UBO::UBO(GLuint bindingPointIndex, int sizeBuffer):_bindingPointIndex(bindingPointIndex), _sizeBuffer(sizeBuffer){
    glGenBuffers(1, &_bufferId);
    reserveBuffer(_sizeBuffer);
}

UBO::~UBO() {
    glDeleteBuffers(1, &_bufferId);
}

void UBO::reserveBuffer(int size){
    glBindBuffer(GL_UNIFORM_BUFFER, _bufferId);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::updateBuffer(const GLvoid* data, int sizeofObject) {
    if(sizeofObject > _sizeBuffer){
        _sizeBuffer = sizeofObject;
        reserveBuffer(_sizeBuffer);
        std::cout << "UBO buffer has been resized: UBO " << this << std::endl;
    }

    glBindBufferBase(GL_UNIFORM_BUFFER, _bindingPointIndex, _bufferId);
    GLvoid* uboData = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    std::memcpy(uboData, data, sizeofObject);
    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

GLuint UBO::bindingPoint() const{
    return _bindingPointIndex;
}