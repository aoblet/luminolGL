//
// Created by mehdi on 16/02/16.
//

#include "graphics/VertexArrayObject.h"
#include <iostream>

namespace Graphics
{
    VertexArrayObject::VertexArrayObject() {
        glGenVertexArrays(1, &_glId);
    }

    void VertexArrayObject::addVBO(VertexBufferObject *vbo) {
        _vbos.push_back(vbo);
    }

    void VertexArrayObject::bind() {
        glBindVertexArray(_glId);
    }

    void VertexArrayObject::init() {
        bind();
        for(auto vbo : _vbos){
            vbo->init();
        }
    }

    void VertexArrayObject::unbindAll() {
        glBindVertexArray(0);
    }
}