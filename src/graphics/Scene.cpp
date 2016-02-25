//
// Created by mehdi on 18/02/16.
//

#include "graphics/Scene.h"

namespace Graphics
{

    void Scene::addMeshInstance(MeshInstance *instance, const std::string& name) {
        _meshInstances.insert({name, instance});
    }

    MeshInstance *Scene::getInstance() {
        return _meshInstances.at(_currentInstance);
    }


    void Scene::setCurrentInstance(const std::string &name) {
        if(_meshInstances.count(name) < 1)
            throw std::runtime_error("Scene::setCurrentInstance : trying to access instance that doesn't exist");

        _currentInstance = name;
    }

    const std::vector<glm::vec3>& Scene::computeVisiblePositions(const glm::mat4 & VP){
        _visiblePositions.clear();

        for(int i = 0; i < _meshInstances[_currentInstance]->getInstanceNumber(); ++i){
            if(_meshInstances[_currentInstance]->getBoundingBox(i).isVisible(VP)){
                _visiblePositions.push_back(_meshInstances[_currentInstance]->getPosition(i));
            }
        }
        return _visiblePositions;
    }

    const std::vector<glm::vec4>& Scene::computeVisibleRotations(const glm::mat4 & VP){
        _visibleRotations.clear();

        for(int i = 0; i < _meshInstances[_currentInstance]->getInstanceNumber(); ++i){
            if(_meshInstances[_currentInstance]->getBoundingBox(i).isVisible(VP)){
                _visibleRotations.push_back(_meshInstances[_currentInstance]->getRotation(i));
            }
        }
        return _visibleRotations;
    }


    const std::vector<glm::vec3> &Scene::getVisiblePositions() {
        return _visiblePositions;
    }

    const std::vector<glm::vec4> &Scene::getVisibleRotations() {
        return _visibleRotations;
    }


    int Scene::getInstanceNumber() {
        return _visiblePositions.size();
    }
}