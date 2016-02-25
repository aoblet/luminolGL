#include "graphics/AssimpModel.hpp"
#include <glog/logging.h>

using namespace Graphics;

AssimpModel::AssimpModel(const std::string &modelPath): _verticesVBO(DataType::VERTEX_DESCRIPTOR), _idsVBO(DataType::ELEMENT_ARRAY_BUFFER) {
    DLOG(INFO) << "Loading model with assimp: " << modelPath;

    Assimp::Importer aImporter;
    const aiScene* scene = aImporter.ReadFile(modelPath,  aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals );

    if(!scene)
        throw std::runtime_error("AssimpModel::loadMesh with assimp - fail while importing " + modelPath + "\n" + aImporter.GetErrorString());

    _directoryPath = modelPath.substr(0, modelPath.find_last_of('/'));
    _meshes.reserve(scene->mNumMeshes);

    const aiMatrix4x4& t = scene->mRootNode->mTransformation;
    _modelMatrix = glm::mat4( t.a1*0.1, t.a2, t.a3, t.a4,
                              t.b1, t.b2*0.1, t.b3, t.b4,
                              t.c1, t.c2, t.c3*0.1, t.c4,
                              t.d1, t.d2, t.d3, t.d4);

    for(unsigned int i=0; i<scene->mNumMeshes; ++i){
        aiMeshToMesh(scene->mMeshes[i], scene);
    }

    buildVBOS();
    DLOG(INFO) << "Loading model " << modelPath << " DONE";
}


std::vector<Mesh>& AssimpModel::meshes() {
    return _meshes;
}

std::map<std::string, Texture>& AssimpModel::textures() {
    return _textures;
}

void AssimpModel::aiMeshToMesh(aiMesh *aiMesh, const aiScene *scene) {

    Mesh mesh;
    // update element indexes
    std::vector<int> elementIndexes;
    for (unsigned int j = 0; j < aiMesh->mNumFaces; ++j) {
        const aiFace &face = aiMesh->mFaces[j];
        elementIndexes.push_back(face.mIndices[0]);
        elementIndexes.push_back(face.mIndices[1]);
        elementIndexes.push_back(face.mIndices[2]);
    }
    _allIds.insert(_allIds.end(), elementIndexes.begin(), elementIndexes.end());

    mesh.addElementIndexes(std::move(elementIndexes));
    mesh.setTriangleCount(aiMesh->mNumFaces);

    // positions, normals and uv
    std::vector<VertexDescriptor> vds;
    for (unsigned int i = 0; i < aiMesh->mNumVertices; ++i) {
        VertexDescriptor vd;
        const aiVector3D &vPos = aiMesh->mVertices[i];
        const aiVector3D &vNormal = aiMesh->mNormals[i];
        const aiVector3D *uv = aiMesh->mTextureCoords[0];
        vd.position = glm::vec3((_modelMatrix * glm::vec4(vPos.x, vPos.y, vPos.z, 1)));
        vd.normal = glm::vec3(vNormal.x, vNormal.y, vNormal.z);
        vd.texcoord = aiMesh->HasTextureCoords(0) ? glm::vec2(uv[i].x, uv[i].y) : glm::vec2(0, 0);
        vds.push_back(vd);

        _allVertices.push_back(vd);
    }
    mesh.addVertices(std::move(vds));
    mesh.computeBoundingBox();

    if(aiMesh->mMaterialIndex >= 0){
        aiString texName;
        aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];

        // just handle one texture per mesh currently
        // TODO: handle multiple diffuse/specular textures in shaders
        if(material->GetTextureCount(aiTextureType_DIFFUSE)){
            material->GetTexture(aiTextureType_DIFFUSE, 0, &texName);
            mesh.attachTexture(saveTexture(texName.C_Str()), Texture::GL_INDEX_DIFFUSE);
        }

        if(material->GetTextureCount(aiTextureType_SPECULAR)){
            material->GetTexture(aiTextureType_SPECULAR, 0, &texName);
            mesh.attachTexture(saveTexture(texName.C_Str()), Texture::GL_INDEX_SPECULAR);
        }

        if(material->GetTextureCount(aiTextureType_HEIGHT)){
            material->GetTexture(aiTextureType_HEIGHT, 0, &texName);
            mesh.attachTexture(saveTexture(texName.C_Str()), Texture::GL_INDEX_NORMAL_MAP);
        }
    }
    _meshes.push_back(std::move(mesh));
}

Texture* AssimpModel::saveTexture(const std::string &textureName) {
    std::string path = _directoryPath + "/" + textureName;
    DLOG(INFO) << "Save texture - " << path;

    if(_textures.find(path) == _textures.end())
        _textures.insert(std::make_pair(path, Texture(path)));
    return &_textures[path];
}

void AssimpModel::buildVBOS(){
//    _verticesVBO.updateData(_allVertices);
//    _idsVBO.updateData(_allIds);
}

void AssimpModel::draw(VertexArrayObject& vao, int instanceNumber){
    int offsetIds = 0;

    vao.bind();
    for(auto& mesh : _meshes){
        mesh.bindTextures();
        glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh.getVertexCount(), GL_UNSIGNED_INT, (void*)0, 100, offsetIds);
        offsetIds += mesh.getVertexCount();
    }
}

VertexBufferObject &AssimpModel::verticesVBO() {
    return _verticesVBO;
}

VertexBufferObject &AssimpModel::idsVBO() {
    return _idsVBO;
}

std::vector<VertexDescriptor> &AssimpModel::allVertices() {
    return _allVertices;
}

std::vector<int> &AssimpModel::allIds() {
    return _allIds;
}
