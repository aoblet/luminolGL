//
// Created by mehdi on 18/02/16.
//

#ifndef LUMINOLGL_MESHINSTANCE_H
#define LUMINOLGL_MESHINSTANCE_H

#include "graphics/Mesh.h"
#include "geometry/Transformation.h"
#include "graphics/VertexBufferObject.h"
#include "graphics/VertexArrayObject.h"

namespace Graphics
{
    class MeshInstance {
    private:
        Mesh* _referenceMesh; /** Pointer to the mesh */
        std::vector<Geometry::Transformation> _transformations; /** Stores Transform property of every instances */
        VertexArrayObject _VAO; /** VAO attached. This VAO is unique for each MeshInstance */
        VertexBufferObject _verticesVBO; /** Stores every vertices attributes of the Mesh (Position, Normal, UV) */
        VertexBufferObject _indexesVBO; /** Stores the indexes of the mesh (i.e in which order the vertices will be drawn) */
    public:
        MeshInstance(Mesh* referenceMesh);

        /** Attach a VBO describing instance transformations
         *  This method must be called before init !
         */
        void attachInstanceTransformVBO(VertexBufferObject *vbo);

        /** Init VAO & VBOs attached to it :
         * _verticesVBO
         * _indexesVBO
         * _InstanceTransformVBO (must be attached before)
         */
        virtual void init();

        /** Draws the mesh with instanced rendering
         * instanceNumber specifies how many mesh will be drawn
         * This number must match the size of InstanceTransformVBO, which is not updated in this class
         */
        virtual void draw(int instanceNumber);

        /** Call _VAO.bind() */
        void bindVAO();

        /** Bind all the textures attached to _referenceMesh */
        void bindTextures();

        /** Several ways to add an instance to _transformations vector */
        void addInstance(const Geometry::Transformation &trans);
        void addInstance(const std::vector<Geometry::Transformation>& transformations);
        void addInstance(const std::vector<glm::vec3> &positions, const std::vector<glm::vec4> &rotations);
        void addInstance(const std::vector<glm::vec3> &positions);
        void addInstance(const glm::vec3& position, const glm::vec4& rotation = glm::vec4(1,1,1,0), const glm::vec3& scale = glm::vec3(1,1,1));
        void addInstance(float xpos, float ypos, float zpos, float angle = 0, float xrot = 0, float yrot = 0, float zrot = 0);

        const glm::vec3& getPosition(int index);
        const glm::vec4& getRotation(int index);

        /** Returns the Transformation property (position, rotation, scale) of a specified index */
        const Geometry::Transformation& getTransformation(int index) const;

        /** Returns the Transformation matrix of a specified index */
        glm::mat4 getTransformationMatrix(int index) const;

        /** Returns the bounding box a specified index
         * This bounding box is computed using the Transformation matrix at the specified index,
         * And the original BoundingBox of _referenceMesh
         */
        virtual Geometry::BoundingBox getBoundingBox(int index) const;

        /** Returns the TOTAL number of instance
         *  This method is different from Scene::getInstanceNumber(), which performs frustum culling
         */
        int getInstanceNumber();
    };
}



#endif //LUMINOLGL_MESHINSTANCE_H
