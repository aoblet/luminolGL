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
    class DebugRay {
        /**
         * Small helper class for scene ray debugging.
         * Cannot be used inside scene draw call, the debug shader is different.
         */
        VertexArrayObject _VAO;
        VertexBufferObject _verticesVBO;
        VertexBufferObject _indexesVBO;
        std::vector<glm::vec3> _points;

        static DebugRay _ray;

        static bool _isInit;
        static void init();

        DebugRay();
    public:
        static void draw(const glm::vec3& point1, const glm::vec3& point2, const ShaderProgram& program);
    };
}




#endif //LUMINOLGL_DEBUGRAY_H
