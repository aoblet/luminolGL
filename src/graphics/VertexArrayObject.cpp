//
// Created by mehdi on 16/02/16.
//

#include "graphics/VertexArrayObject.h"
#include <iostream>
#include <utils/utils.h>

namespace Graphics
{
    VertexArrayObject::VertexArrayObject() {
        glGenVertexArrays(1, &_glId);
    }

    VertexArrayObject::VertexArrayObject(VertexArrayObject &&other) {
        std::swap(_glId, other._glId);
        _vbos = std::move(other._vbos);
    }

    void VertexArrayObject::addVBO(VertexBufferObject *vbo) {
        _vbos.push_back(vbo);
    }

    void VertexArrayObject::bind() {
        glBindVertexArray(_glId);
    }

    void VertexArrayObject::init() {
        bind();
        for(auto& vbo : _vbos){
            vbo->init();
        }
    }

    GLuint VertexArrayObject::glId() {
        return _glId;
    }

    void VertexArrayObject::unbindAll() {
        glBindVertexArray(0);
    }

    VertexArrayObject::~VertexArrayObject() {
        glDeleteVertexArrays(1, &_glId);
    }

    VertexArrayObject::VertexArrayObject(const VertexArrayObject &other):VertexArrayObject() {
        _vbos = other._vbos;
    }
}