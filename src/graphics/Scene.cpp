//
// Created by mehdi on 18/02/16.
//

#include <glm/ext.hpp>
#include <glog/logging.h>
#include "graphics/Scene.h"

namespace Graphics
{


    Scene::Scene() : _visibleTransformationsVBO(Graphics::INSTANCE_TRANSFORMATION_BUFFER, 3) { }


    void Scene::init() {
        for(auto& instance : _meshInstances){
            instance.second->initGLBuffers(_visibleTransformationsVBO);
        }
    }

    void Scene::draw(const glm::mat4 &VP) {
        for(auto& instance : _meshInstances){
            setCurrentInstance(instance.first);
            _visibleTransformationsVBO.updateData(computeVisibleTransformations(VP));
            instance.second->draw(getInstanceNumber());
        }
    }

    void Scene::addMeshInstance(ModelMeshInstanced *instance, const std::string& name) {
        _meshInstances.insert({name, instance});
    }

    ModelMeshInstanced *Scene::getInstance() {
        return _meshInstances.at(_currentInstance);
    }


    void Scene::setCurrentInstance(const std::string &name) {
        if(_meshInstances.count(name) < 1)
            throw std::runtime_error("Scene::setCurrentInstance : trying to access instance that doesn't exist");
        _currentInstance = name;
    }

    const std::vector<glm::mat4> &Scene::computeVisibleTransformations(const glm::mat4 &VP) {
        _visibleTransformations.clear();
        for(int i = 0; i < _meshInstances[_currentInstance]->getInstanceNumber(); ++i){
            if(_meshInstances[_currentInstance]->getBoundingBox(i).isVisible(VP)){
                _visibleTransformations.push_back(_meshInstances[_currentInstance]->getTransformationMatrix(i));
            }
        }
        _currentInstanceNumber = _visibleTransformations.size();

        return _visibleTransformations;
    }

    int Scene::getInstanceNumber() {
        return _currentInstanceNumber;
    }
}