//
// Created by mehdi on 14/02/16.
//

#include "graphics/VertexBufferObject.h"
#include <iostream>

namespace Graphics
{

    VertexBufferObject::VertexBufferObject(DataType dataType, GLuint attribArray) : _dataType(dataType), _attribArray(attribArray) {
        glGenBuffers(1, &_glId);
        _target = _dataType == ELEMENT_ARRAY_BUFFER ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
    }

    void VertexBufferObject::init() {
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
        glVertexAttribPointer( _attribArray, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0 );
        glVertexAttribDivisor( _attribArray, 1 );
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

    void VertexBufferObject::setAttribArray(GLuint value){
        _attribArray = value;
    }

    void VertexBufferObject::unbindAll(){
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}