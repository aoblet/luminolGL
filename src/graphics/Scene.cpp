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
            _meshInstances(std::move(meshes)), _visibleTransformationsVBO(Graphics::DataType::INSTANCE_TRANSFORMATION_BUFFER, 3),
            _ioHandler(ioHandler), _water(ModelMeshGroup::PATH_PLANE){

        scenePath.empty() ? initGL() : load(scenePath);

        _water.initGLBuffers(_visibleTransformationsVBO);
        _water.modelMeshGroup().removeTextures();
    }

    void Scene::initGL() {
        for(auto& instance : _meshInstances)
            instance.initGLBuffers(_visibleTransformationsVBO);
    }

    void Scene::draw(const glm::mat4 &VP, bool castShadowless) {
        for(auto& instance : _meshInstances){
            if(!instance.castShadows() && !castShadowless)
                continue;

            computeVisibleTransformations(VP, instance);
            _visibleTransformationsVBO.updateData(_visibleTransformations);
            // Draw for current instance nb transformations visible i.e total amount of instanced elements
            instance.draw((int)_visibleTransformations.size());
        }
    }

    void Scene::drawWater(const glm::mat4 &VP){
        _visibleTransformations.clear();
        std::for_each(_water.getTransformations().begin(), _water.getTransformations().end(), [this](Geometry::Transformation& t){_visibleTransformations.push_back(t.getTRSMatrix());});
        _visibleTransformationsVBO.updateData(_visibleTransformations);
        _water.draw((int)_visibleTransformations.size());
    }


    void Scene::setFar(float far) {
        _far = far;
    }

    void Scene::updateCameraPosition(const glm::vec3 &camPos) {
        _cameraPosition = camPos;
    }

    void Scene::computeVisibleTransformations(const glm::mat4 &VP, const ModelMeshInstanced& mesh) {
        _visibleTransformations.clear();
        for(int i = 0; i < mesh.getInstanceNumber(); ++i){
            float distance = glm::distance(mesh.getPosition(i), _cameraPosition);
            if(mesh.getBoundingBox(i).isVisible(VP) && distance < _far){
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
        _meshInstances.clear();
        _ioHandler->load(*this, path);
        initGL();
    }

    std::vector<ModelMeshInstanced>& Scene::meshInstances() {
        return _meshInstances;
    }

    const std::vector<ModelMeshInstanced>& Scene::meshInstances() const{
        return _meshInstances;
    }

    void Scene::addModelMeshInstanced(const std::string &modelPath, const Geometry::Transformation &transformation) {
        DLOG(INFO) << "Scene::addModelMeshInstanced: " << modelPath;

        auto foundMesh = std::find_if(_meshInstances.begin(), _meshInstances.end(), [&modelPath](const ModelMeshInstanced& m) { return m.modelPath() == modelPath;});

        if(foundMesh == _meshInstances.end()){
            DLOG(INFO) << "Scene::addModelMeshInstanced: Mesh not found " << modelPath << ". Create it.";
            try{
                ModelMeshInstanced m(modelPath, {transformation});
                m.initGLBuffers(_visibleTransformationsVBO);
                _meshInstances.push_back(std::move(m));
            }
            catch (std::exception& e){
                DLOG(ERROR) << e.what();
            }
        }
        else{
            DLOG(INFO) << "Scene::addModelMeshInstanced: Mesh found " << modelPath;
            foundMesh->addInstance(transformation);
        }
    }

    void Scene::deleteMeshByPtr(ModelMeshInstanced* meshInstanced, Geometry::Transformation* transformation) {
        // Find index
        auto& tr = meshInstanced->getTransformations();
        size_t indexRemove = transformation - &(tr[0]);

        if(indexRemove > tr.size()){
            DLOG(ERROR) << "Scene::deleteMeshByPtr : You are trying to remove a transform which is not inside Scene";
            return;
        }
        tr.erase(tr.begin() + indexRemove);

        if(meshInstanced == &_water)
            return;

        // Remove meshInstanced if no more transformations are present
        if(!tr.empty())
            return;

        indexRemove = meshInstanced - &(_meshInstances[0]);
        if(indexRemove > _meshInstances.size() || indexRemove < 0){
            DLOG(ERROR) << "Scene::deleteMeshByPtr : You are trying to remove a mesh which is not inside Scene";
            return;
        }
        _meshInstances.erase(_meshInstances.begin() + indexRemove);
    }

    Geometry::Transformation& Scene::duplicateMesh(ModelMeshInstanced &meshInstanced, Geometry::Transformation &transformation) {
        return meshInstanced.addInstance(transformation);
    }

    ModelMeshInstanced &Scene::water() {
        return _water;
    }

    const ModelMeshInstanced& Scene::water() const {
        return _water;
    }

    void Scene::addWater(const Geometry::Transformation& trans) {
        _water.addInstance(trans);
    }
}