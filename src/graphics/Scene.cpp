//
// Created by mehdi on 18/02/16.
//

#include <glm/ext.hpp>
#include <glog/logging.h>
#include "graphics/Scene.h"
#include <fstream>
#include <iomanip>

namespace Graphics
{


    Scene::Scene(const std::list<ModelMeshInstanced*>& meshes) :
            _meshInstances(meshes), _visibleTransformationsVBO(Graphics::INSTANCE_TRANSFORMATION_BUFFER, 3){
        initGL();
    }

    void Scene::initGL() {
        for(auto& instance : _meshInstances)
            instance->initGLBuffers(_visibleTransformationsVBO);
    }

    void Scene::draw(const glm::mat4 &VP) {
        for(auto& instance : _meshInstances){
            computeVisibleTransformations(VP, instance);
            _visibleTransformationsVBO.updateData(_visibleTransformations);

            // Draw for current instance nb transformations visible i.e total amount of instanced elements
            instance->draw((int)_visibleTransformations.size());
        }
    }

    void Scene::computeVisibleTransformations(const glm::mat4 &VP, ModelMeshInstanced* mesh) {
        _visibleTransformations.clear();
        for(int i = 0; i < mesh->getInstanceNumber(); ++i){
            if(mesh->getBoundingBox(i).isVisible(VP)){
                _visibleTransformations.push_back(mesh->getTransformationMatrix(i));
            }
        }
    }
}