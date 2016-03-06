//
// Created by mehdi on 05/03/16.
//

#include "graphics/DebugDrawer.h"
#include <glog/logging.h>
#include <graphics/UBO_keys.hpp>

namespace Graphics
{
    bool DebugDrawer::_isInit = false;
    DebugDrawer DebugDrawer::_drawer;

    void DebugDrawer::init() {
        if(_isInit) return;
        _isInit = true;

        _drawer._VAO.addVBO(&_drawer._verticesVBO);
        _drawer._VAO.addVBO(&_drawer._indexesVBO);
        _drawer._VAO.addVBO(&_drawer._transformVBO);
        _drawer._VAO.init();
        std::vector<int> ids = {0,1};
        std::vector<glm::mat4> trans = {glm::mat4()};

        _drawer._indexesVBO.updateData(ids);
        _drawer._transformVBO.updateData(trans);

        Graphics::VertexArrayObject::unbindAll();
        Graphics::VertexBufferObject::unbindAll();
    }

    DebugDrawer::DebugDrawer() :
            _VAO(),
            _verticesVBO(Graphics::VEC3),
            _indexesVBO(Graphics::ELEMENT_ARRAY_BUFFER),
            _transformVBO(Graphics::INSTANCE_TRANSFORMATION_BUFFER, 1)
    { }

    void DebugDrawer::drawRay(const glm::vec3 &point1, const glm::vec3 &point2, ShaderProgram &program, const glm::vec3 &color, float lineWidth) {
        if(!_isInit) init();

        _drawer._points.clear();
        _drawer._points.push_back(point1);
        _drawer._points.push_back(point2);
        _drawer._verticesVBO.updateData(_drawer._points);

        program.useProgram();
        program.updateUniform(UBO_keys::DEBUG_COLOR, color);

        glLineWidth(lineWidth);

        _drawer._VAO.bind();
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)0);
    }


    void DebugDrawer::drawPoint(const glm::vec3 &point, ShaderProgram &program, const glm::vec3 &color, float pointSize) {
        if(!_isInit) init();

        glPointSize(pointSize);

        _drawer._points.clear();
        _drawer._points.push_back(point);
        _drawer._verticesVBO.updateData(_drawer._points);

        program.useProgram();
        program.updateUniform(UBO_keys::DEBUG_COLOR, color);
        _drawer._VAO.bind();
        glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, (void*)0);
    }


    void DebugDrawer::drawAxis(const glm::mat4 &trans, ShaderProgram &program, float scale, float lineWidth) {
        glLineWidth(lineWidth);
        glm::vec3 xAxis = glm::vec3(trans * glm::vec4(1,0,0,0));
        glm::vec3 yAxis = glm::vec3(trans * glm::vec4(0,1,0,0));
        glm::vec3 zAxis = glm::vec3(trans * glm::vec4(0,0,1,0));

        glm::vec3 origin = glm::vec3(trans *  glm::vec4(0,0,0,1));

        drawRay(origin, origin + scale * xAxis, program, glm::vec3(1, 0, 0), lineWidth);
        drawRay(origin, origin + scale * yAxis, program, glm::vec3(0, 1, 0), lineWidth);
        drawRay(origin, origin + scale * zAxis, program, glm::vec3(0, 0, 1), lineWidth);
    }


}