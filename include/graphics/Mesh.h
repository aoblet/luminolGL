//
// Created by mehdi on 16/02/16.
//

#ifndef LUMINOLGL_MESH_H
#define LUMINOLGL_MESH_H

#include <vector>
#include <map>
#include <memory>

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
        Geometry::BoundingBox _boundaries;
        std::map<GLenum, Texture*> _textures;
    public:
        Mesh();
        Mesh(Mesh&& mesh);
        void addVertices(const std::vector<VertexDescriptor>& vertices);
        void addVertices(std::vector<VertexDescriptor> &&vertices);
        void addElementIndexes(const std::vector<int>& vertices);
        void addElementIndexes(std::vector<int> &&vertices);
        void setTriangleCount(unsigned int value);
        void attachTexture(Graphics::Texture* tex, GLenum textureNumber);
        void bindTextures();
        const std::vector<VertexDescriptor>& getVertices() const;
        const std::vector<int>& getElementIndex() const;
        int getVertexCount() const;
        int getTriangleCount() const;
        bool isVisible(const View::CameraFreefly& camera) const;
        void computeBoundingBox();
        const Geometry::BoundingBox& getBoundingBox();
        static Mesh genCube();
        static Mesh genPlane(float width = 1.f, float height = 1.f, float textureLoop = 1, const glm::vec3 & offset = glm::vec3(0,0,0));
        static Mesh genSphere(int latitudeBands, int longitudeBands, float radius = 1.f, const glm::vec3 & offset = glm::vec3(0,0,0));
        static Mesh loadMesh(const std::string& filePath);
    };
}




#endif //LUMINOLGL_MESH_H
