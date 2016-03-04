#pragma once

#include <vector>
#include "graphics/VertexBufferObject.h"
#include "graphics/VertexArrayObject.h"
#include "graphics/ShaderProgram.hpp"
#include "graphics/ModelMeshInstanced.hpp"
#include "geometry/Transformation.h"
#include "geometry/BoundingBox.h"

namespace Graphics{
    /**
     * Small helper class for scene meshes debugging.
     * Cannot be used inside scene draw call, the debug shader is different.
     */
    class DebugBoundingBoxes {
        VertexArrayObject _VAO;
        VertexBufferObject _verticesVBO;
        VertexBufferObject _indexesVBO;
        VertexBufferObject _transformVBO;
        ShaderProgram _debugShader;
        std::vector<int> _idsVertices;
        const std::vector<ModelMeshInstanced>& _meshes;
        bool _isDrawing;

    public:
        /**
         * Toggle the draw activation.
         */
        void toggle();
        DebugBoundingBoxes(const std::vector<ModelMeshInstanced>& meshes);
        void draw(const glm::mat4& mvp);
    };
}