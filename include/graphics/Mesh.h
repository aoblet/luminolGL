//
// Created by mehdi on 16/02/16.
//

#ifndef LUMINOLGL_MESH_H
#define LUMINOLGL_MESH_H

#include <vector>
#include "graphics/VertexDescriptor.h"
#include "graphics/Texture.h"
#include "view/CameraFreefly.hpp"
#include "geometry/BoundingBox.h"

namespace Graphics
{
    class Mesh {
    private:
        unsigned int _vertexCount;
        unsigned int _triangleCount;
        std::vector<VertexDescriptor> _vertices;
        std::vector<int> _elementIndex;
        std::vector<Texture*> _textures;
        Geometry::BoundingBox _boundaries;
    public:
        Mesh();
        Mesh(Mesh&& mesh);
        const std::vector<VertexDescriptor>& getVertices() const;
        const std::vector<int>& getElementIndex() const;
        int getVertexCount() const;
        int getTriangleCount() const;
        bool isVisible(const View::CameraFreefly& camera) const;
        void computeBoundingBox();
        const Geometry::BoundingBox& getBoundingBox();
        static Mesh genCube();
        static Mesh genPlane(float width = 1.f, float height = 1.f, float textureLoop = 1);
    };
}




#endif //LUMINOLGL_MESH_H
