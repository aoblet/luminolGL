//
// Created by mehdi on 18/02/16.
//

#include <glm/ext.hpp>
#include <glog/logging.h>
#include "graphics/Scene.h"
#include <fstream>
#include <iomanip>

namespace Graphics {
    Scene::Scene(Data::SceneIO* ioHandler, const std::string& scenePath, std::vector<ModelMeshInstanced>&& meshes):
            _meshInstances(std::move(meshes)), _visibleTransformationsVBO(Graphics::INSTANCE_TRANSFORMATION_BUFFER, 3), _ioHandler(ioHandler){

        if(!scenePath.empty())
            load(scenePath);

        initGL();
    }

    void Scene::initGL() {
        for(auto& instance : _meshInstances)
            instance.initGLBuffers(_visibleTransformationsVBO);
    }

    void Scene::draw(const glm::mat4 &VP) {
        for(auto& instance : _meshInstances){
            computeVisibleTransformations(VP, instance);
            _visibleTransformationsVBO.updateData(_visibleTransformations);

            // Draw for current instance nb transformations visible i.e total amount of instanced elements
            instance.draw((int)_visibleTransformations.size());
        }
    }

    void Scene::computeVisibleTransformations(const glm::mat4 &VP, const ModelMeshInstanced& mesh) {
        _visibleTransformations.clear();
        for(int i = 0; i < mesh.getInstanceNumber(); ++i){
            if(mesh.getBoundingBox(i).isVisible(VP)){
                _visibleTransformations.push_back(mesh.getTransformationMatrix(i));
            }
        }
    }

    void Scene::setIOHandler(Data::SceneIO *io) {
        _ioHandler = io;
    }

    void Scene::save(const std::string &path) {
        _ioHandler->save(*this, path);
    }

    void Scene::load(const std::string &path) {
        _ioHandler->load(*this, path);
    }

    std::vector<ModelMeshInstanced>& Scene::meshInstances() {
        return _meshInstances;
    }

    const std::vector<ModelMeshInstanced>& Scene::meshInstances() const{
        return _meshInstances;
    }
}