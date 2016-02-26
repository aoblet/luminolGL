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
        unsigned int _vertexCount;              /** Vertices numbers sent to draw call openGL (linked with _elements indexes)*/
        unsigned int _triangleCount;            /** Number of triangles in the mesh */
        std::vector<VertexDescriptor> _vertices;/** Vertices attributes (position, normal , uv) */
        std::vector<int> _elementIndex;         /** Vertices indexes (vertices drawing order) */
        Geometry::BoundingBox _boundaries       /** Mesh Bounding box */;
        std::map<GLenum, Texture*> _textures     /** Textures attached to the mesh with
                                                  *  a specific binding point (GL_TEXTURE0, GL_TEXTURE1, ...) */;
    public:
        Mesh();
        Mesh(Mesh&& mesh);

        void addVertices(const std::vector<VertexDescriptor>& vertices); /** Add vertices to _vertices by copy */


        void addVertices(std::vector<VertexDescriptor> &&vertices); /** Add vertices to _vertices by move*/


        void addElementIndexes(const std::vector<int>& vertices); /** Add indexes to _elementIndex by copy */


        void addElementIndexes(std::vector<int> &&vertices); /** Add indexes to _elementIndex by move */


        void setTriangleCount(unsigned int value);


        void attachTexture(Graphics::Texture* tex, GLenum textureNumber); /** Add to _textures with a specified binding point :
                                                                           *  GL_TEXTURE0, GL_TEXTURE1, ...
                                                                           */


        void bindTextures(); /** Bind all Textures stored in _textures on their binding point */


        const std::vector<VertexDescriptor>& getVertices() const;
        const std::vector<int>& getElementIndex() const;
        int getVertexCount() const;
        int getTriangleCount() const;


        void computeBoundingBox(); /** look over _vertices to create a
                                    *  square bounding box containing all vertices */


        const Geometry::BoundingBox& getBoundingBox();


        /** Static functions to generate simple primitives */
        static Mesh genCube();
        static Mesh genPlane(float width = 1.f, float height = 1.f, float textureLoop = 1, const glm::vec3 & offset = glm::vec3(0,0,0));
        static Mesh genSphere(int latitudeBands, int longitudeBands, float radius = 1.f, const glm::vec3 & offset = glm::vec3(0,0,0));
    };
}




#endif //LUMINOLGL_MESH_H
