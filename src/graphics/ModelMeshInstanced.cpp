#include "graphics/ModelMeshInstanced.hpp"

using namespace Graphics;

ModelMeshInstanced::ModelMeshInstanced(const std::string &modelPath, const std::vector<Geometry::Transformation>& transformations):
        _modelMeshGroup(modelPath),
        _VAO(),
        _verticesVBO(Graphics::VERTEX_DESCRIPTOR),
        _indexesVBO(Graphics::ELEMENT_ARRAY_BUFFER),
        _transformations(transformations),
        _modelPath(modelPath){}

ModelMeshInstanced::ModelMeshInstanced(ModelMeshInstanced &&other):
        _modelMeshGroup(std::move(other._modelMeshGroup)),
        _VAO(std::move(other._VAO)),
        _verticesVBO(std::move(other._verticesVBO)),
        _indexesVBO(std::move(other._indexesVBO)),
        _transformations(std::move(other._transformations)),
        _modelPath(std::move(other._modelPath)),
        _scenePositionsVBO(std::move(other._scenePositionsVBO)){

    resetVAO();
}

void ModelMeshInstanced::resetVAO(){

    // VertexArrayObject _VAO handles pointers so we need to properly update these values.
    if(_VAO.vbos().size()){
        assert(_scenePositionsVBO != nullptr);
        _VAO.clearVBOs();
        _VAO.addVBO(&_verticesVBO);
        _VAO.addVBO(&_indexesVBO);
        _VAO.addVBO(_scenePositionsVBO);
    }
}

ModelMeshInstanced &ModelMeshInstanced::operator=(ModelMeshInstanced &&other) {
    std::swap(_modelMeshGroup, other._modelMeshGroup);
    std::swap(_VAO, other._VAO);
    std::swap(_verticesVBO, other._verticesVBO);
    std::swap(_indexesVBO, other._indexesVBO);
    std::swap(_transformations,other._transformations);
    std::swap(_modelPath, other._modelPath);
    std::swap(_scenePositionsVBO, other._scenePositionsVBO);
    resetVAO();

    return *this;
}


Geometry::Transformation& ModelMeshInstanced::addInstance(const Geometry::Transformation &trans){
    _transformations.push_back(trans);
    return _transformations[_transformations.size()-1];
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

int ModelMeshInstanced::getInstanceNumber() const {
    return (int)_transformations.size();
}

const Geometry::Transformation &ModelMeshInstanced::getTransformation(int index) const {
    return _transformations.at(index);
}


glm::mat4 ModelMeshInstanced::getTransformationMatrix(int index) const {
    return _transformations.at(index).getTRSMatrix();
}

Geometry::BoundingBox ModelMeshInstanced::getBoundingBox(int index) const {
    return _transformations.at(index).getTRSMatrix() * _modelMeshGroup.getBoundingBox();
}

void ModelMeshInstanced::initGLBuffers(VertexBufferObject& scenePositionsVBO) {
    _scenePositionsVBO = &scenePositionsVBO;
    _VAO.addVBO(&_verticesVBO);
    _VAO.addVBO(&_indexesVBO);
    _VAO.addVBO(&scenePositionsVBO);
    _VAO.init();

    _verticesVBO.updateData(_modelMeshGroup.allVertices());
    _indexesVBO.updateData(_modelMeshGroup.allIndexes());
    _VAO.unbindAll();
}

void ModelMeshInstanced::draw(int nbInstanceToDraw) {
    _VAO.bind();
    _modelMeshGroup.draw(nbInstanceToDraw);
}

std::string ModelMeshInstanced::modelPath() const {
    return _modelPath;
}

const std::vector<Geometry::Transformation>& ModelMeshInstanced::getTransformations() const {
    return _transformations;
}

std::vector<Geometry::Transformation>& ModelMeshInstanced::getTransformations() {
    return _transformations;
}

void ModelMeshInstanced::setTransformations(std::vector<Geometry::Transformation>&& transformations){
    _transformations = std::move(transformations);
}

const ModelMeshGroup &ModelMeshInstanced::modelMeshGroup() const {
    return _modelMeshGroup;
}

ModelMeshGroup &ModelMeshInstanced::modelMeshGroup(){
    return _modelMeshGroup;
}

VertexArrayObject &ModelMeshInstanced::vao() {
    return _VAO;
}