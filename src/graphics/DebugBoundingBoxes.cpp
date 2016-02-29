#include "utils/utils.h"
#include "graphics/DebugBoundingBoxes.hpp"
#include "graphics/UBO_keys.hpp"

Graphics::DebugBoundingBoxes::DebugBoundingBoxes(const std::vector<ModelMeshInstanced>& meshes):
        _VAO(), _verticesVBO(Graphics::VEC3), _indexesVBO(Graphics::ELEMENT_ARRAY_BUFFER),
        _transformVBO(Graphics::INSTANCE_TRANSFORMATION_BUFFER, 1),
        _debugShader("../shaders/debug.vert", "", "../shaders/debug.frag"),
        _idsVertices({0,3,2,1,0,4,7,6,5,4,7,3,2,6,5,1}), _meshes(meshes), _isDrawing(false){
    _VAO.addVBO(&_verticesVBO);
    _VAO.addVBO(&_indexesVBO);
    _VAO.addVBO(&_transformVBO);
    _VAO.init();
    _indexesVBO.updateData(_idsVertices);

    Graphics::VertexArrayObject::unbindAll();
    Graphics::VertexBufferObject::unbindAll();
    checkErrorGL("VAO/VBO");
}

void Graphics::DebugBoundingBoxes::draw(const glm::mat4& mvp){
    if(!_isDrawing)
        return;

    _debugShader.useProgram();
    _debugShader.updateUniform(UBO_keys::MVP, mvp);
    _VAO.bind();

    for(auto& meshInstanced: _meshes){
        const std::vector<glm::vec3>& bbVertices = meshInstanced.modelMeshGroup().getBoundingBox().getVector();
        _verticesVBO.updateData(bbVertices);
        std::vector<glm::mat4> trToMatrix;

        for(size_t i = 0; i<meshInstanced.getTransformations().size(); ++i){
            trToMatrix.push_back(meshInstanced.getTransformation(i).getTransformationMatrix());
        }
        _transformVBO.updateData(trToMatrix);
        glDrawElementsInstanced(GL_LINE_STRIP, (int)_idsVertices.size(), GL_UNSIGNED_INT, (void*)0, (int)trToMatrix.size());
    }
}

void Graphics::DebugBoundingBoxes::toggle() {
    _isDrawing = !_isDrawing;
}
