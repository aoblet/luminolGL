//
// Created by mehdi on 18/02/16.
//
#pragma once

#include <vector>
#include <string>
#include <glm/detail/type_vec.hpp>

#include "graphics/ModelMeshInstanced.hpp"
#include "graphics/VertexBufferObject.h"
#include "data/SceneIO.hpp"

namespace Graphics {

    /**
     * Scene managing openGL draw calls and loading/saving.
     */

    class Scene {
    private:
        std::vector<ModelMeshInstanced> _meshInstances;             /** Scene meshes instanced*/
        std::vector<glm::mat4> _visibleTransformations;             /** This vector is updated with visible Transformations of the current instance */
        VertexBufferObject _visibleTransformationsVBO;              /** A VBO containing the Transformations of visible current instances */
        Data::SceneIO* _ioHandler;
        ModelMeshInstanced _water;

        glm::vec3 _cameraPosition;
        float _far;

        /**
         * Must be called after all MeshInstances has been attached
         * _visibleTransformationsVBO is attached to each MeshInstance VAO
         * MeshInstance::init() is called for every MeshInstance to generate VAOs & VBOs
         */
        void initGL();

    public:

        /**
         * Build a scene from optional parameters
         * @param scene scene path
         * @param meshes must be set with move semantic if given
         */
        Scene(Data::SceneIO* ioHandler, const std::string& scenePath="", std::vector<ModelMeshInstanced>&& meshes={});

        /**
         * Call the draw() function of each MeshInstance.
         * Performs frustum culling to update _visibleTransformationsVBO :
         * Only visible instances of VP Matrix are sent to the GPU
         */
        void draw(const glm::mat4 &VP, bool castShadowless = true);
        void drawWater(const glm::mat4 &VP);

        void setFar(float far);
        void updateCameraPosition(const glm::vec3& camPos);

        /**
         * Compute frustum culling on current instance.
         * @param VP matrix used to perform culling
         */
        void computeVisibleTransformations(const glm::mat4 & VP, const ModelMeshInstanced& mesh);

        /**
         * Modify the IO behavior called when loading and saving scene
         */
        void setIOHandler(Data::SceneIO * io);

        /**
         * Save scene at given path
         */
        void save(const std::string& path);

        /**
         * Load scene from given path
         */
        void load(const std::string& path);
        std::vector<ModelMeshInstanced>& meshInstances();
        const std::vector<ModelMeshInstanced>& meshInstances() const;
        ModelMeshInstanced& water();
        const ModelMeshInstanced& water() const;

        void addModelMeshInstanced(const std::string& modelPath, const Geometry::Transformation& transformation={});
        void deleteMeshByPtr(ModelMeshInstanced * meshInstanced, Geometry::Transformation * transformation);
        Geometry::Transformation& duplicateMesh(ModelMeshInstanced& meshInstanced, Geometry::Transformation& transformation);

        void addWater(const Geometry::Transformation& trans);
    };
}