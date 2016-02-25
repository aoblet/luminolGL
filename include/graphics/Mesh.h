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
        unsigned int _vertexCount; /** Number of vertices in the mesh */
        unsigned int _triangleCount; /** Number of triangles in the mesh */
        std::vector<VertexDescriptor> _vertices; /** Vertices attributes (position, normal , uv) */
        std::vector<int> _elementIndex; /** Vertices indexes (vertices drawing order) */
        Geometry::BoundingBox _boundaries /** Bounding box of the mesh */;
        std::map<GLenum, Texture*> _textures /** Textures attached to the mesh with a specific binding point (GL_TEXTURE0, GL_TEXTURE1, ...) */;
    public:
        Mesh();
        Mesh(Mesh&& mesh);

        /** Add vertices to _vertices by copy */
        void addVertices(const std::vector<VertexDescriptor>& vertices);
        /** Add vertices to _vertices by move*/
        void addVertices(std::vector<VertexDescriptor> &&vertices);

        /** Add indexes to _elementIndex by copy */
        void addElementIndexes(const std::vector<int>& vertices);
        /** Add indexes to _elementIndex by move */
        void addElementIndexes(std::vector<int> &&vertices);

        void setTriangleCount(unsigned int value);

        /** Add to _textures with a specified binding point (GL_TEXTURE0, GL_TEXTURE1, ...) */
        void attachTexture(Graphics::Texture* tex, GLenum textureNumber);

        /** Bind all Textures stored in _textures on their binding point */
        void bindTextures();

        const std::vector<VertexDescriptor>& getVertices() const;
        const std::vector<int>& getElementIndex() const;

        int getVertexCount() const;
        int getTriangleCount() const;

        /** look over _vertices to create a bounding box containing all vertices */
        void computeBoundingBox();

        const Geometry::BoundingBox& getBoundingBox();

        /** Static functions to generate simple primitives */
        static Mesh genCube();
        static Mesh genPlane(float width = 1.f, float height = 1.f, float textureLoop = 1, const glm::vec3 & offset = glm::vec3(0,0,0));
        static Mesh genSphere(int latitudeBands, int longitudeBands, float radius = 1.f, const glm::vec3 & offset = glm::vec3(0,0,0));

        /** Static function to load a mesh using assimp */
        static Mesh loadMesh(const std::string& filePath);
    };
}




#endif //LUMINOLGL_MESH_H
