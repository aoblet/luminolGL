//
// Created by mehdi on 18/02/16.
//

#include <glm/ext.hpp>
#include <glog/logging.h>
#include "graphics/Scene.h"
#include <fstream>
#include <iomanip>

namespace Graphics {
    Scene::Scene(Data::SceneIO* ioHandler, const std::string& scenePath, std::vector<ModelMeshInstanced>&& meshes, bool debugBoundingBoxes):
            _meshInstances(std::move(meshes)), _visibleTransformationsVBO(Graphics::INSTANCE_TRANSFORMATION_BUFFER, 3),
            _ioHandler(ioHandler){

        if(!scenePath.empty())
            load(scenePath);
        else
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

    void Scene::drawWater(){
        _visibleTransformations.clear();
        _visibleTransformations.push_back(_water->getTransformationMatrix(0));
        _visibleTransformationsVBO.updateData(_visibleTransformations);
        _water->vao().bind();
        glDrawElementsInstancedBaseVertex(GL_TRIANGLES, _water->modelMeshGroup().allIndexes().size()*3, GL_UNSIGNED_INT, 0, 1, 0);
    }

    void Scene::initWaterGL(ModelMeshInstanced *water){
        _water = water;
        _water->initGLBuffers(_visibleTransformationsVBO);
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
            DLOG(INFO) << "Scene::addModelMeshInstanced: Mesh not found " << modelPath;
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
}