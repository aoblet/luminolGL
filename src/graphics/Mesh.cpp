//
// Created by mehdi on 16/02/16.
//

#include "graphics/Mesh.h"

namespace Graphics
{

    Mesh::Mesh() : _vertexCount(0), _triangleCount(0) { }

    Mesh::Mesh(Mesh &&mesh) :
            _vertexCount(mesh._vertexCount),
            _triangleCount(mesh._triangleCount),
            _vertices(std::move(mesh._vertices)),
            _elementIndex(std::move(mesh._elementIndex)),
            _textures(std::move(mesh._textures))
    {
    }

    const std::vector<VertexDescriptor> &Mesh::getVertices() const {
        return _vertices;
    }


    const std::vector<int> &Mesh::getElementIndex() const {
        return _elementIndex;
    }


    int Mesh::getVertexCount() const {
        return _vertexCount;
    }

    int Mesh::getTriangleCount() const {
        return _triangleCount;
    }


    bool Mesh::isVisible(const View::CameraFreefly& camera) const {
        return _boundaries.isVisible(camera);
    }

    void Mesh::computeBoundingBox() {
        _boundaries.compute(_vertices);
    }

    Mesh Mesh::genCube() {
        Mesh mesh;

        mesh._vertices = {
                Graphics::VertexDescriptor(-0.5, -0.5, 0.5, 0, 0, 1, 0.f, 0.f),
                Graphics::VertexDescriptor(0.5, -0.5, 0.5, 0, 0, 1, 0.f, 1.f),
                Graphics::VertexDescriptor(-0.5, 0.5, 0.5, 0, 0, 1, 1.f, 0.f),
                Graphics::VertexDescriptor(0.5, 0.5, 0.5, 0, 0, 1, 1.f, 1.f),
                Graphics::VertexDescriptor(-0.5, 0.5, 0.5, 0, 1, 0, 0.f, 0.f),
                Graphics::VertexDescriptor(0.5, 0.5, 0.5, 0, 1, 0, 0.f, 1.f),
                Graphics::VertexDescriptor(-0.5, 0.5, -0.5, 0, 1, 0, 1.f, 0.f),
                Graphics::VertexDescriptor(0.5, 0.5, -0.5, 0, 1, 0, 1.f, 1.f),
                Graphics::VertexDescriptor(-0.5, 0.5, -0.5, 0, 0, -1, 0.f, 0.f),
                Graphics::VertexDescriptor(0.5, 0.5, -0.5, 0, 0, -1, 0.f, 1.f),
                Graphics::VertexDescriptor(-0.5, -0.5, -0.5, 0, 0, -1, 1.f, 0.f),
                Graphics::VertexDescriptor(0.5, -0.5, -0.5, 0, 0, -1, 1.f, 1.f),
                Graphics::VertexDescriptor(-0.5, -0.5, -0.5, 0, -1, 0, 0.f, 0.f),
                Graphics::VertexDescriptor(0.5, -0.5, -0.5, 0, -1, 0, 0.f, 1.f),
                Graphics::VertexDescriptor(-0.5, -0.5, 0.5, 0, -1, 0, 1.f, 0.f),
                Graphics::VertexDescriptor(0.5, -0.5, 0.5, 0, -1, 0, 1.f, 1.f),
                Graphics::VertexDescriptor(0.5, -0.5, 0.5, 1, 0, 0, 0.f, 0.f),
                Graphics::VertexDescriptor(0.5, -0.5, -0.5, 1, 0, 0, 0.f, 1.f),
                Graphics::VertexDescriptor(0.5, 0.5, 0.5, 1, 0, 0, 1.f, 0.f),
                Graphics::VertexDescriptor(0.5, 0.5, 0.5, 1, 0, 0, 1.f, 0.f),
                Graphics::VertexDescriptor(0.5, 0.5, -0.5, 1, 0, 0, 1.f, 1.f),
                Graphics::VertexDescriptor(-0.5, -0.5, -0.5, -1, 0, 0, 0.f, 1.f),
                Graphics::VertexDescriptor(-0.5, -0.5, 0.5, -1, 0, 0, 1.f, 1.f),
                Graphics::VertexDescriptor(-0.5, 0.5, -0.5, -1, 0, 0, 0.f, 0.f),
                Graphics::VertexDescriptor(-0.5, 0.5, -0.5, -1, 0, 0, 0.f, 0.f),
                Graphics::VertexDescriptor(-0.5, -0.5, 0.5, -1, 0, 0, 1.f, 1.f),
                Graphics::VertexDescriptor(-0.5, 0.5, 0.5, -1, 0, 0, 1.f, 0.f)
        };

        mesh._elementIndex = {
                0, 1, 2,
                2, 1, 3,
                4, 5, 6,
                6, 5, 7,
                8, 9, 10,
                10, 9, 11,
                12, 13, 14,
                14, 13, 15,
                16, 17, 18,
                19, 17, 20,
                21, 22, 23,
                24, 25, 26
        };

        mesh._triangleCount = 12;
        mesh._vertexCount = mesh._triangleCount * 3;

        std::cout << "vertexCount before : " << mesh._vertices.size() << std::endl;

        mesh.computeBoundingBox();

        return mesh;
    }

    Mesh Mesh::genPlane(float width, float height, float textureLoop) {
        Mesh mesh;

        mesh._vertices = {
                Graphics::VertexDescriptor(-width/2, 0, height/2, 0, 1, 0, 0.f, 0.f),
                Graphics::VertexDescriptor(width/2, 0, height/2, 0, 1, 0, 0.f, textureLoop),
                Graphics::VertexDescriptor(-width/2, 0, -height/2, 0, 1, 0, textureLoop, 0.f),
                Graphics::VertexDescriptor(width/2, 0, -height/2, 0, 1, 0, textureLoop, textureLoop)
        };

        mesh._elementIndex = {
                0, 1, 2,
                2, 1, 3
        };

        mesh._triangleCount = 2;
        mesh._vertexCount = mesh._triangleCount * 3;

        mesh.computeBoundingBox();

        return mesh;
    }

    const Geometry::BoundingBox &Mesh::getBoundingBox() {
        return _boundaries;
    }
}