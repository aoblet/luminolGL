//
// Created by mehdi on 05/03/16.
//

#ifndef LUMINOLGL_DEBUGRAY_H
#define LUMINOLGL_DEBUGRAY_H

#include <vector>
#include "graphics/VertexBufferObject.h"
#include "graphics/VertexArrayObject.h"
#include "graphics/ShaderProgram.hpp"
#include "geometry/Transformation.h"

namespace Graphics
{
    class DebugDrawer {
        /**
         * Small helper class for scene debugging.
         */
        VertexArrayObject _VAO;
        VertexBufferObject _verticesVBO;
        VertexBufferObject _indexesVBO;
        VertexBufferObject _transformVBO;
        std::vector<glm::vec3> _points;

        static DebugDrawer _drawer;

        static bool _isInit;
        static void init();

        DebugDrawer();
    public:
        static void drawRay(const glm::vec3 &point1, const glm::vec3 &point2, ShaderProgram &program, const glm::vec3 &color = glm::vec3(1, 1, 1), float lineWidth = 1);
        static void drawPoint(const glm::vec3 &point, ShaderProgram &program, const glm::vec3 &color = glm::vec3(1, 1, 1), float pointSize = 1);
        static void drawAxis(const glm::mat4 &trans, ShaderProgram &program, float scale = 1, float lineWidth = 1);
    };
}




#endif //LUMINOLGL_DEBUGRAY_H
