//
// Created by mehdi on 05/03/16.
//

#ifndef LUMINOLGL_DEBUGRAY_H
#define LUMINOLGL_DEBUGRAY_H

#include <vector>
#include <geometry/BoundingBox.h>
#include <geometry/Spline3D.h>
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
        static void drawSpline(const Geometry::Spline3D& spline, int subdivisions, ShaderProgram &program, const glm::vec3 &color = glm::vec3(1, 1, 1), float lineWidth = 1);
        static void drawTriangle(const glm::vec3 &point1, const glm::vec3 &point2, const glm::vec3 &point3, ShaderProgram &program, const glm::vec3 &color = glm::vec3(1, 1, 1));
        static void drawPyramid(const glm::mat4 &trans, ShaderProgram &program, float scale = 1, const glm::vec3 &color = glm::vec3(1, 1, 1));
        static void drawCube(const glm::mat4 &trans, ShaderProgram &program, float scale = 1, const glm::vec3 &color = glm::vec3(1, 1, 1));
        static void drawPoint(const glm::vec3 &point, ShaderProgram &program, const glm::vec3 &color = glm::vec3(1, 1, 1), float pointSize = 1);
        static void drawAxis(const glm::mat4 &trans, ShaderProgram &program, float scale = 1, float lineWidth = 1);
        static void drawTranslateAxis(const glm::mat4 &trans, ShaderProgram &program, float scale = 1, float lineWidth = 1);
        static void drawScaleAxis(const glm::mat4 &trans, ShaderProgram &program, float scale = 1, float lineWidth = 1);
        static void drawRotationAxis(const glm::mat4 &trans, ShaderProgram &program, float scale = 1, float lineWidth = 1);
        static void drawBoundingBox(const Geometry::BoundingBox& box, const glm::mat4 &trans, ShaderProgram &program, const glm::vec3 &color = glm::vec3(1, 1, 1));
    };
}




#endif //LUMINOLGL_DEBUGRAY_H
