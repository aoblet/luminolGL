//
// Created by mehdi on 18/02/16.
//

#ifndef LUMINOLGL_SCENE_H
#define LUMINOLGL_SCENE_H

#include <list>
#include <vector>
#include <string>
#include <glm/detail/type_vec.hpp>

#include "graphics/ModelMeshInstanced.hpp"
#include "graphics/VertexBufferObject.h"


namespace Graphics
{
    class Scene {
    private:
        std::list<ModelMeshInstanced*> _meshInstances;              /** Each mesh instance is associated to a name */
        std::vector<glm::mat4> _visibleTransformations;             /** This vector is updated with visible Transformations of the current instance */
        VertexBufferObject _visibleTransformationsVBO;              /** A VBO containing the Transformations of visible current instances */

        /**
         * Must be called after all MeshInstances has been attached.
         *  _visibleTransformationsVBO is attached to each MeshInstance VAO
         *  MeshInstance::init() is called for every MeshInstance to generate VAOs & VBOs
         */
        void initGL();

    public:
        Scene(const std::list<ModelMeshInstanced*>& meshes);

        /**
         * Call the draw() function of each MeshInstance.
         *  Performs frustum culling to update _visibleTransformationsVBO :
         *  Only visible instances of VP Matrix are sent to the GPU
         */
        void draw(const glm::mat4 &VP);

        /**
         * Compute frustum culling on current instance.
         *  VP is the matrix that will be used
         *  to say if an instance Transform is visible or not.
         *  - _visibleTransformations vector
         */
        void computeVisibleTransformations(const glm::mat4 & VP, ModelMeshInstanced* mesh);
    };
}




#endif //LUMINOLGL_SCENE_H
