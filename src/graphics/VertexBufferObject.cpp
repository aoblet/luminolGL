//
// Created by mehdi on 14/02/16.
//

#include "graphics/VertexBufferObject.h"
#include <iostream>
#include <glog/logging.h>

namespace Graphics
{

    VertexBufferObject::VertexBufferObject(DataType dataType, GLuint attribArray, bool initGL) : _dataType(dataType), _attribArray(attribArray), _isInGPU(initGL) {
        if(initGL)
            glGenBuffers(1, &_glId);
        _target = _dataType == ELEMENT_ARRAY_BUFFER ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
    }

    VertexBufferObject::VertexBufferObject(VertexBufferObject &&other) {
        std::swap(_glId, other._glId);
        std::swap(_dataType, other._dataType);
        std::swap(_target, other._target);
        std::swap(_attribArray, other._attribArray);
        std::swap(_isInGPU, other._isInGPU);
        other._glId = 0;
    }

    VertexBufferObject::VertexBufferObject(const VertexBufferObject &other): VertexBufferObject(other._dataType, other._attribArray, other._isInGPU) {}

    void VertexBufferObject::initGL() {
        if(_isInGPU){
            DLOG(WARNING) << "VBO has been already initialized, skip glGenBuffers call";
            return;
        }
        glGenBuffers(1, &_glId);
        _isInGPU = true;
    }

    VertexBufferObject::~VertexBufferObject() {
        glDeleteBuffers(1, &_glId);
        _glId = 0;
    }

    void VertexBufferObject::init() {
        if(!_isInGPU)
            initGL();

        switch(_dataType){
            case VERTEX_DESCRIPTOR:
                initVboVertexDescriptor();
                break;

            case VEC3:
                initVboVec3();
                break;

            case VEC2:
                initVboVec2();
                break;

            case FLOAT:
                initVboFloat();
                break;

            case INT:
                initVboInt();
                break;

            case INSTANCE_BUFFER:
                initVboInstanceVec3();
                break;

            case INSTANCE_TRANSFORMATION_BUFFER:
                initVboInstanceMat4();
                break;

            default: // no need to init element array buffer
                break;
        }
    }

    void VertexBufferObject::initVboVertexDescriptor(){
        bind();
        // position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Graphics::VertexDescriptor), (void*)0);

        // normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Graphics::VertexDescriptor), (void*)(3*sizeof(GL_FLOAT)));

        // uv
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Graphics::VertexDescriptor), (void*)(6*sizeof(GL_FLOAT)));
    }

    void VertexBufferObject::initVboVec3(){
        bind();
        glEnableVertexAttribArray(_attribArray);
        glVertexAttribPointer(_attribArray, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    }

    void VertexBufferObject::initVboVec2(){
        bind();
        glEnableVertexAttribArray(_attribArray);
        glVertexAttribPointer(_attribArray, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void*)0);
    }

    void VertexBufferObject::initVboFloat(){
        bind();
        glEnableVertexAttribArray(_attribArray);
        glVertexAttribPointer(_attribArray, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    void VertexBufferObject::initVboInt(){
        bind();
        glEnableVertexAttribArray(_attribArray);
        glVertexAttribPointer(_attribArray, 1, GL_INT, GL_FALSE, 0, (void*)0);
    }


    void VertexBufferObject::initVboInstanceVec3() {
        bind();
        glEnableVertexAttribArray( _attribArray );
        glVertexAttribPointer( _attribArray, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0 );
        glVertexAttribDivisor( _attribArray, 1 );
    }

    void VertexBufferObject::initVboInstanceMat4() {
        bind();

        for( int c = 0; c < 4; ++c )
        {
            glEnableVertexAttribArray( _attribArray + c );
            glVertexAttribPointer( _attribArray + c, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(c * sizeof(glm::vec4)) );
            glVertexAttribDivisor( _attribArray + c, 1 );
        }
    }

    void VertexBufferObject::bind(){
        glBindBuffer(_target, _glId);
    }

    GLuint VertexBufferObject::glId(){
        return _glId;
    }

    void VertexBufferObject::updateData(const std::vector<VertexDescriptor>& data){
        bind();
        glBufferData(_target, data.size() * sizeof(Graphics::VertexDescriptor), data.data(), GL_STATIC_DRAW);
    }

    void VertexBufferObject::updateData(const std::vector<glm::vec3>& data){
        bind();
        glBufferData(_target, data.size() * 3 * sizeof(GL_FLOAT), data.data(), GL_STATIC_DRAW);
    }

    void VertexBufferObject::updateData(const std::vector<glm::vec2>& data){
        bind();
        glBufferData(_target, data.size() * 2 * sizeof(GL_FLOAT), data.data(), GL_STATIC_DRAW);
    }

    void VertexBufferObject::updateData(const std::vector<float>& data){
        bind();
        glBufferData(_target, data.size() * sizeof(GL_FLOAT), data.data(), GL_STATIC_DRAW);
    }

    void VertexBufferObject::updateData(const std::vector<int>& data){
        bind();
        glBufferData(_target, data.size() * sizeof(GL_INT), data.data(), GL_STATIC_DRAW);
    }


    void VertexBufferObject::updateData(const std::vector<glm::mat4> &data) {
        bind();
        glBufferData(_target, data.size() * sizeof(glm::mat4), data.data(), GL_STATIC_DRAW);
    }

    void VertexBufferObject::setAttribArray(GLuint value){
        _attribArray = value;
    }

    void VertexBufferObject::unbindAll(){
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

}