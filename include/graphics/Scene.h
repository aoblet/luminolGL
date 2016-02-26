//
// Created by mehdi on 18/02/16.
//

#ifndef LUMINOLGL_SCENE_H
#define LUMINOLGL_SCENE_H

#include <vector>
#include <string>
#include <glm/detail/type_vec.hpp>

#include "graphics/MeshInstance.h"
#include "graphics/VertexBufferObject.h"

namespace Graphics
{
    class Scene {
    private:
        std::map<std::string, MeshInstance*> _meshInstances; /** Each mesh instance is associated to a name */

        std::string _currentInstance;                        /** The name of the current instance called by setCurrentInstance() */

        std::vector<glm::mat4> _visibleTransformations;      /** This vector is updated with visible Transformations of the current instance */

        VertexBufferObject _visibleTransformationsVBO;       /** A VBO containing the Transformations of visible current instances */

        int _currentInstanceNumber;                          /** Current instance that will impact :
                                                             *   computeVisibleTransformations(), getInstance(), or getInstanceNumber()
                                                             */
    public:


        Scene(); /** Just init The VBO containing Instance transformations */


        void init(); /** Must be called after all MeshInstances has been attached.
                      *  _visibleTransformationsVBO is attached to each MeshInstance VAO
                      *  MeshInstance::init() is called for every MeshInstance to generate VAOs & VBOs
                      */


        void draw(const glm::mat4 &VP); /** Call the draw() function of each MeshInstance.
                                         *  Performs frustum culling to update _visibleTransformationsVBO :
                                         *  Only visible instances of VP Matrix are sent to the GPU
                                         */


        void addMeshInstance(MeshInstance *instance, const std::string& name); /** Add a MeshInstance to _meshInstances
                                                                                *  a name must be specified
                                                                                */


        void setCurrentInstance(const std::string& name); /** Switch the MeshInstance that will be updated.
                                                           *  Modify the behavior of :
                                                           *  - computeVisibleTransformations()
                                                           *  - getInstanceNumber()
                                                           *  - getInstance()
                                                           */


        MeshInstance* getInstance();         /** Returns the currentInstance */


        const std::vector<glm::mat4>& computeVisibleTransformations(const glm::mat4 & VP); /** Compute frustum culling on current instance.
                                                                                            *  VP is the matrix that will be used
                                                                                            *  to say if an instance Transform is visible or not.
                                                                                            *  Updates :
                                                                                            *  - _visibleTransformations vector
                                                                                            *  - _visibleTransformationsVBO
                                                                                            *  - _currentInstanceNumber
                                                                                            */


        int getInstanceNumber(); /** Returns the number of visible instances in the current MeshInstance
                                  *  Must be called after computeVisibleTransformations()
                                  */
    };
}




#endif //LUMINOLGL_SCENE_H
