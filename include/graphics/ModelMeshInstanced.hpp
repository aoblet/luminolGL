#pragma once
#include "graphics/ModelMeshGroup.hpp"
#include "geometry/Transformation.h"
#include <string>
#include <vector>

namespace Graphics {

    /**
     * Interface between openGL and our MeshGroup for instanced renderer.
     */
    class ModelMeshInstanced {
        ModelMeshGroup _modelMeshGroup;                         /** Model mesh group build inside instantiator*/
        VertexArrayObject  _VAO;                                /** VAO attached. This VAO is unique for ModelMeshGroup */
        VertexBufferObject _verticesVBO;                        /** Stores every vertices attributes of the ModelMeshGroup (Position, Normal, UV) */
        VertexBufferObject _indexesVBO;                         /** Stores the indexes of the mesh (i.e in which order the vertices will be drawn) */
        std::vector<Geometry::Transformation> _transformations; /** Stores Transform property of every instances */
        std::string _modelPath;
        VertexBufferObject * _scenePositionsVBO = nullptr;

    public:
        ModelMeshInstanced(const std::string &modelPath, const std::vector<Geometry::Transformation>& transformations = {});
        ModelMeshInstanced(ModelMeshInstanced&& other);
        ModelMeshInstanced(const ModelMeshInstanced& other) = delete;

        void initGLBuffers(VertexBufferObject& scenePositionsVBO);
        void draw(int nbInstanceToDraw);

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
        const std::vector<Geometry::Transformation>& getTransformations() const;
        void setTransformations(std::vector<Geometry::Transformation>&& transformations);

        /** Returns the Transformation matrix of a specified index */
        glm::mat4 getTransformationMatrix(int index) const;

        /**
         * Returns the bounding box a specified index
         * This bounding box is computed using the Transformation matrix at the specified index.
         * @return Model Space
         */
        Geometry::BoundingBox getBoundingBox(int index) const;

        const ModelMeshGroup& modelMeshGroup() const;

        /**
         * Returns the TOTAL number of instance
         * This method is different from Scene::getInstanceNumber(), which performs frustum culling
         */
        int getInstanceNumber() const;
        std::string modelPath() const;
    };
}
