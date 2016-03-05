//
// Created by mehdi on 16/02/16.
//

#include "graphics/VertexArrayObject.h"
#include <iostream>
#include <utils/utils.h>
#include <logging.h>

namespace Graphics
{
    VertexArrayObject::VertexArrayObject() {
//        glGenVertexArrays(1, &_glId);
    }

    VertexArrayObject::VertexArrayObject(VertexArrayObject &&other) {
        _glId = other._glId;
        other._glId = 0;
        _vbos = other._vbos;
    }

    void VertexArrayObject::addVBO(VertexBufferObject *vbo) {
        _vbos.push_back(vbo);
    }

    void VertexArrayObject::bind() {
        glBindVertexArray(_glId);
    }

    void VertexArrayObject::init() {
        glGenVertexArrays(1, &_glId);
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
        _glId = 0;
    }

    VertexArrayObject::VertexArrayObject(const VertexArrayObject &other):VertexArrayObject() {
        _vbos = other._vbos;
    }

    const std::vector<VertexBufferObject *> &VertexArrayObject::vbos() const {
        return _vbos;
    }

    void VertexArrayObject::clearVBOs() {
        _vbos.clear();
    }
}