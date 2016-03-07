//
// Created by mehdi on 16/02/16.
//

#include "graphics/VertexArrayObject.h"
#include <iostream>
#include <utils/utils.h>
#include <glog/logging.h>

namespace Graphics
{
    VertexArrayObject::VertexArrayObject(bool initGL) : _isInGPU(initGL){
        if(_isInGPU)
            glGenVertexArrays(1, &_glId);
    }

    VertexArrayObject::VertexArrayObject(VertexArrayObject &&other) {
        _glId = other._glId;
        other._glId = 0;
        std::swap(_vbos, other._vbos);
        std::swap(_isInGPU, other._isInGPU);
    }

    VertexArrayObject::VertexArrayObject(const VertexArrayObject &other) : VertexArrayObject(other._isInGPU) {
        _vbos = other._vbos;
    }

    VertexArrayObject::~VertexArrayObject() {
        glDeleteVertexArrays(1, &_glId);
        _glId = 0;
    }

    void VertexArrayObject::addVBO(VertexBufferObject *vbo) {
        _vbos.push_back(vbo);
    }

    void VertexArrayObject::bind() {
        glBindVertexArray(_glId);
    }

    void VertexArrayObject::initGL() {
        if(_isInGPU){
            DLOG(WARNING) << "VAO has been already initialized, skip glGenVertexArrays call";
            return;
        }
        glGenVertexArrays(1, &_glId);
        _isInGPU = true;
    }

    void VertexArrayObject::init() {
        if(!_isInGPU)
            initGL();

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



    const std::vector<VertexBufferObject *> &VertexArrayObject::vbos() const {
        return _vbos;
    }

    void VertexArrayObject::clearVBOs() {
        _vbos.clear();
    }

}