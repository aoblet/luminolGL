//
// Created by mehdi on 05/03/16.
//

#include "graphics/DebugRay.h"
#include <glog/logging.h>

namespace Graphics
{
    bool DebugRay::_isInit = false;
    DebugRay DebugRay::_ray;

    void DebugRay::draw(const glm::vec3 &point1, const glm::vec3 &point2, const ShaderProgram &program) {
        if(!_isInit) init();

        _ray._points.clear();
        _ray._points.push_back(point1);
        _ray._points.push_back(point2);
        _ray._verticesVBO.updateData(_ray._points);

        program.useProgram();
        _ray._VAO.bind();
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)0);
    }

    void DebugRay::init() {
        if(_isInit) return;
        _isInit = true;

        _ray._VAO.addVBO(&_ray._verticesVBO);
        _ray._VAO.addVBO(&_ray._indexesVBO);
        _ray._VAO.init();
        std::vector<int> ids = {0,1};
        _ray._indexesVBO.updateData(ids);
    }

    DebugRay::DebugRay() :
        _VAO(),
        _verticesVBO(Graphics::VEC3),
        _indexesVBO(Graphics::ELEMENT_ARRAY_BUFFER)
    {
    }
}