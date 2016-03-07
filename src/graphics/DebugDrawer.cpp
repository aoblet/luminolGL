//
// Created by mehdi on 05/03/16.
//

#include "graphics/DebugDrawer.h"
#include <glog/logging.h>
#include <graphics/UBO_keys.hpp>
#include <glm/gtx/transform.hpp>

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
        std::vector<int> ids = {0,1,2};
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

    void DebugDrawer::drawTriangle(const glm::vec3 &point1, const glm::vec3 &point2, const glm::vec3 &point3, ShaderProgram &program, const glm::vec3 &color) {
        if(!_isInit) init();

        _drawer._points.clear();
        _drawer._points.push_back(point1);
        _drawer._points.push_back(point2);
        _drawer._points.push_back(point3);
        _drawer._verticesVBO.updateData(_drawer._points);

        program.useProgram();
        program.updateUniform(UBO_keys::DEBUG_COLOR, color);

        _drawer._VAO.bind();
        glDrawElements(GL_TRIANGLE_STRIP, 3, GL_UNSIGNED_INT, (void*)0);
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

    void DebugDrawer::drawPyramid(const glm::mat4 &trans, ShaderProgram &program, float scale, const glm::vec3 &color) {
        if(!_isInit) init();

        glm::vec3 xAxis = glm::vec3(trans * glm::vec4(1,0,0,0));
        glm::vec3 yAxis = glm::vec3(trans * glm::vec4(0,1,0,0));
        glm::vec3 zAxis = glm::vec3(trans * glm::vec4(0,0,1,0));

        glm::vec3 origin = glm::vec3(trans *  glm::vec4(0,0,0,1));

        std::vector<glm::vec3> pyramid;
        pyramid.push_back(yAxis / (scale * 0.3f));
        pyramid.push_back((  xAxis + zAxis) / scale);
        pyramid.push_back((  xAxis - zAxis) / scale);
        pyramid.push_back((- xAxis - zAxis) / scale);
        pyramid.push_back((- xAxis + zAxis) / scale);

        drawTriangle(origin + scale * yAxis + pyramid[0],
                     origin + scale * yAxis + pyramid[1],
                     origin + scale * yAxis + pyramid[2],
                     program, color);

        drawTriangle(origin + scale * yAxis + pyramid[0],
                     origin + scale * yAxis + pyramid[2],
                     origin + scale * yAxis + pyramid[3],
                     program, color);

        drawTriangle(origin + scale * yAxis + pyramid[0],
                     origin + scale * yAxis + pyramid[3],
                     origin + scale * yAxis + pyramid[4],
                     program, color);

        drawTriangle(origin + scale * yAxis + pyramid[0],
                     origin + scale * yAxis + pyramid[4],
                     origin + scale * yAxis + pyramid[1],
                     program, color);
    }


    void DebugDrawer::drawCube(const glm::mat4 &trans, ShaderProgram &program, float scale, const glm::vec3 &color) {
        if(!_isInit) init();

        std::vector<glm::vec3> cube;

        glm::vec3 origin = glm::vec3(trans *  glm::vec4(0,0,0,1));
        cube.push_back(origin + glm::vec3(+scale,-scale,+scale));
        cube.push_back(origin + glm::vec3(-scale,-scale,+scale));
        cube.push_back(origin + glm::vec3(-scale,+scale,+scale));
        cube.push_back(origin + glm::vec3(-scale,+scale,-scale));
        cube.push_back(origin + glm::vec3(+scale,+scale,-scale));
        cube.push_back(origin + glm::vec3(+scale,+scale,+scale));
        cube.push_back(origin + glm::vec3(-scale,-scale,-scale));
        cube.push_back(origin + glm::vec3(+scale,-scale,-scale));

        drawTriangle(cube[0], cube[1], cube[2], program, color);
        drawTriangle(cube[0], cube[2], cube[5], program, color);

        drawTriangle(cube[1], cube[2], cube[3], program, color);
        drawTriangle(cube[1], cube[3], cube[6], program, color);

        drawTriangle(cube[3], cube[4], cube[6], program, color);
        drawTriangle(cube[6], cube[4], cube[7], program, color);

        drawTriangle(cube[0], cube[5], cube[4], program, color);
        drawTriangle(cube[0], cube[4], cube[7], program, color);

        drawTriangle(cube[2], cube[3], cube[4], program, color);
        drawTriangle(cube[2], cube[4], cube[5], program, color);

        drawTriangle(cube[0], cube[1], cube[6], program, color);
        drawTriangle(cube[0], cube[6], cube[7], program, color);
    }

    void DebugDrawer::drawAxis(const glm::mat4 &trans, ShaderProgram &program, float scale, float lineWidth) {
        if(!_isInit) init();

        glLineWidth(lineWidth);

        glm::vec3 xAxis = glm::vec3(trans * glm::vec4(1,0,0,0));
        glm::vec3 yAxis = glm::vec3(trans * glm::vec4(0,1,0,0));
        glm::vec3 zAxis = glm::vec3(trans * glm::vec4(0,0,1,0));

        glm::vec3 origin = glm::vec3(trans *  glm::vec4(0,0,0,1));

        drawRay(origin, origin + scale * xAxis, program, glm::vec3(1, 0, 0), lineWidth);
        drawRay(origin, origin + scale * yAxis, program, glm::vec3(0, 1, 0), lineWidth);
        drawRay(origin, origin + scale * zAxis, program, glm::vec3(0, 0, 1), lineWidth);
    }

    void DebugDrawer::drawTranslateAxis(const glm::mat4 &trans, ShaderProgram &program, float scale, float lineWidth) {
        if(!_isInit) init();

        drawAxis(trans, program, scale, lineWidth);

        drawPyramid(trans * glm::rotate(glm::radians(-90.f), glm::vec3(0, 0, 1)), program, scale, glm::vec3(1,0,0));
        drawPyramid(trans, program, scale, glm::vec3(0,1,0));
        drawPyramid(trans * glm::rotate(glm::radians(90.f), glm::vec3(1, 0, 0)), program, scale, glm::vec3(0,0,1));
    }

    void DebugDrawer::drawScaleAxis(const glm::mat4 &trans, ShaderProgram &program, float scale, float lineWidth) {
        drawAxis(trans, program, scale, lineWidth);

        drawCube(trans * glm::translate(scale*glm::vec3(1,0,0)), program, scale/20.f, glm::vec3(1,0,0));
        drawCube(trans * glm::translate(scale*glm::vec3(0,1,0)), program, scale/20.f, glm::vec3(0,1,0));
        drawCube(trans * glm::translate(scale*glm::vec3(0,0,1)), program, scale/20.f, glm::vec3(0,0,1));
    }

    void DebugDrawer::drawRotationAxis(const glm::mat4 &trans, ShaderProgram &program, float scale, float lineWidth) {
        if(!_isInit) init();

        drawAxis(trans, program, scale, lineWidth);
    }

    void DebugDrawer::drawBoundingBox(const Geometry::BoundingBox &box, const glm::mat4 &trans, ShaderProgram &program, const glm::vec3 &color) {

        if(!_isInit) init();

        auto boxCopy = trans * box;

        auto& points = boxCopy.getVector();

        drawRay(points[0], points[3], program, color);
        drawRay(points[3], points[2], program, color);
        drawRay(points[2], points[1], program, color);
        drawRay(points[1], points[0], program, color);
        drawRay(points[0], points[4], program, color);
        drawRay(points[4], points[7], program, color);
        drawRay(points[7], points[6], program, color);
        drawRay(points[6], points[5], program, color);
        drawRay(points[5], points[4], program, color);
        drawRay(points[4], points[7], program, color);
        drawRay(points[7], points[3], program, color);
        drawRay(points[3], points[2], program, color);
        drawRay(points[2], points[6], program, color);
        drawRay(points[6], points[5], program, color);
        drawRay(points[5], points[1], program, color);
    }
}