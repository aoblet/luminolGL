#include "graphics/ModelMeshInstanced.hpp"

using namespace Graphics;

ModelMeshInstanced::ModelMeshInstanced(const std::string &modelPath):
        _modelMeshGroup(modelPath),
        _VAO(),
        _verticesVBO(Graphics::VERTEX_DESCRIPTOR),
        _indexesVBO(Graphics::ELEMENT_ARRAY_BUFFER) {}

void ModelMeshInstanced::addInstance(const Geometry::Transformation &trans){
    _transformations.push_back(trans);
}

void ModelMeshInstanced::addInstance(const std::vector<Geometry::Transformation> &transformations) {
    _transformations.insert(_transformations.begin(), transformations.begin(), transformations.end());
}

void ModelMeshInstanced::addInstance(const std::vector<glm::vec3> &positions, const std::vector<glm::vec4> &rotations) {
    if(positions.size() != rotations.size())
        throw std::runtime_error("ModelMeshInstanced::addInstance : Trying to load position and rotation vector with different size");

    for(size_t i = 0; i < positions.size(); ++i)
        _transformations.push_back(Geometry::Transformation(positions[i], rotations[i]));
}

void ModelMeshInstanced::addInstance(const std::vector<glm::vec3> &positions) {
    for(auto& position : positions)
        _transformations.push_back(Geometry::Transformation(position));
}

void ModelMeshInstanced::addInstance(const glm::vec3 &position, const glm::vec4 &rotation, const glm::vec3& scale) {
    addInstance(Geometry::Transformation(position, rotation, scale));
}

void ModelMeshInstanced::addInstance(float xpos, float ypos, float zpos, float angle, float xrot, float yrot, float zrot) {
    addInstance(Geometry::Transformation(xpos, ypos, zpos, angle, xrot, yrot, zrot));
}


const glm::vec3& ModelMeshInstanced::getPosition(int index) {
    return _transformations.at(index).position;
}

const glm::vec4& ModelMeshInstanced::getRotation(int index) {
    return _transformations.at(index).rotation;
}

int ModelMeshInstanced::getInstanceNumber() {
    return _transformations.size();
}

const Geometry::Transformation &ModelMeshInstanced::getTransformation(int index) const {
    return _transformations.at(index);
}


glm::mat4 ModelMeshInstanced::getTransformationMatrix(int index) const {
    return _transformations.at(index).getTransformationMatrix();
}

Geometry::BoundingBox ModelMeshInstanced::getBoundingBox(int index) const {
    return _transformations.at(index).getTransformationMatrix() * _modelMeshGroup.getBoundingBox();
}

void ModelMeshInstanced::initGLBuffers(VertexBufferObject &scenePositionsVBO) {
    _VAO.addVBO(&_verticesVBO);
    _VAO.addVBO(&_indexesVBO);
    _VAO.addVBO(&scenePositionsVBO);
    _VAO.init();

    _verticesVBO.updateData(_modelMeshGroup.allVertices());
    _indexesVBO.updateData(_modelMeshGroup.allIndexes());
}

void ModelMeshInstanced::draw(int nbInstanceToDraw) {
    _VAO.bind();
    _modelMeshGroup.draw(nbInstanceToDraw);
}