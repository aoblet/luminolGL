#include "graphics/ModelMeshGroup.hpp"
#include <glog/logging.h>

using namespace Graphics;

const std::string ModelMeshGroup::PATH_DEFAULT_TEX_DIFFUSE     = "../assets/textures/default.png";
const std::string ModelMeshGroup::PATH_DEFAULT_TEX_SPECULAR    = "../assets/textures/default.png";
const std::string ModelMeshGroup::PATH_DEFAULT_TEX_NORMAL      = "../assets/textures/default_normal.png";
const std::string ModelMeshGroup::PATH_PLANE                   = "../assets/models/primitives/plane.obj";


ModelMeshGroup::ModelMeshGroup(const std::string &modelPath) {

    DLOG(INFO) << "Loading model with assimp: " << modelPath;

    Assimp::Importer aImporter;
    const aiScene* scene = aImporter.ReadFile(modelPath, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenSmoothNormals | aiProcess_FlipUVs );

    if(!scene)
        throw std::runtime_error("ModelMeshGroup::loadMesh with assimp - fail while importing " + modelPath + "\n" + aImporter.GetErrorString());

    const aiMatrix4x4& t = scene->mRootNode->mTransformation;
    _modelMatrix = glm::mat4( t.a1, t.a2, t.a3, t.a4,
                              t.b1, t.b2, t.b3, t.b4,
                              t.c1, t.c2, t.c3, t.c4,
                              t.d1, t.d2, t.d3, t.d4);

    _directoryPath = modelPath.substr(0, modelPath.find_last_of('/'));
    _meshes.reserve(scene->mNumMeshes);
    loadMeshes(scene);
    computeBoundingBox();


    DLOG(INFO) << "Vertices count " << _allVertices.size();
    DLOG(INFO) << "Textures count " << _textures.size();
    DLOG(INFO) << "Loading model " << modelPath << " DONE" << std::endl << std::endl;
}

ModelMeshGroup::ModelMeshGroup(ModelMeshGroup &&other){
    *this = std::move(other);
}

ModelMeshGroup &ModelMeshGroup::operator=(ModelMeshGroup &&other) {
    std::swap(_textures, other._textures);
    std::swap(_meshes, other._meshes);
    std::swap(_directoryPath, other._directoryPath);
    std::swap(_modelMatrix, other._modelMatrix);
    std::swap(_boundingBox, other._boundingBox);
    std::swap(_allVertices, other._allVertices);
    std::swap(_allIds, other._allIds);
    return *this;
}

void ModelMeshGroup::loadMeshes(const aiScene* scene) {
    for(unsigned int i=0; i<scene->mNumMeshes; ++i){
        aiMeshToMesh(scene->mMeshes[i], scene);
    }
}

const std::vector<Mesh>&ModelMeshGroup::meshes() const {
    return _meshes;
}


std::map<std::string, Texture>&ModelMeshGroup::textures() {
    return _textures;
}

void ModelMeshGroup::aiMeshToMesh(aiMesh *aiMesh, const aiScene *scene) {

    Mesh mesh;
    // update element indexes
    std::vector<int> elementIndexes;
    for (unsigned int j = 0; j < aiMesh->mNumFaces; ++j) {
        const aiFace &face = aiMesh->mFaces[j];
        for(unsigned int k = 0; k < face.mNumIndices; ++k){
            assert(face.mNumIndices == 3);
            elementIndexes.push_back(face.mIndices[k]);
        }
    }
    _allIds.insert(_allIds.end(), elementIndexes.begin(), elementIndexes.end());
    mesh.addElementIndexes(elementIndexes);
    mesh.setTriangleCount(aiMesh->mNumFaces);

    // positions, normals and uv
    std::vector<VertexDescriptor> vds;
    for (unsigned int i = 0; i < aiMesh->mNumVertices; ++i) {
        VertexDescriptor vd;

        const aiVector3D &vPos = aiMesh->mVertices[i];
        const aiVector3D &vNormal = aiMesh->mNormals[i];
        const aiVector3D *uv = aiMesh->mTextureCoords[0];

        vd.position = glm::vec3((_modelMatrix * glm::vec4(vPos.x, vPos.y, vPos.z, 1)));
        vd.normal = aiMesh->HasNormals() ? glm::vec3(vNormal.x, vNormal.y, vNormal.z) : glm::vec3(1,1,1);
        vd.texcoord = aiMesh->HasTextureCoords(0) ? glm::vec2(uv[i].x, uv[i].y) : glm::vec2(0, 0);
        vds.push_back(vd);

        _allVertices.push_back(vd);
    }
    mesh.addVertices((vds));
    mesh.computeBoundingBox();

    if(aiMesh->mMaterialIndex >= 0){
        aiString texName;
        aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];

        // just handle one texture per mesh currently
        // TODO: handle multiple diffuse/specular textures in shaders

        // Diffuse
        if(material->GetTextureCount(aiTextureType_DIFFUSE)){
            material->GetTexture(aiTextureType_DIFFUSE, 0, &texName);
            mesh.attachTexture(saveTexture(_directoryPath + "/" + texName.C_Str()), Texture::GL_INDEX_DIFFUSE);
        }
        else{
            mesh.attachTexture(saveTexture(PATH_DEFAULT_TEX_DIFFUSE), Texture::GL_INDEX_DIFFUSE);
        }

        // Specular
        if(material->GetTextureCount(aiTextureType_SPECULAR)){
            material->GetTexture(aiTextureType_SPECULAR, 0, &texName);
            mesh.attachTexture(saveTexture(_directoryPath + "/" + texName.C_Str()), Texture::GL_INDEX_SPECULAR);
        }
        else{
            mesh.attachTexture(saveTexture(PATH_DEFAULT_TEX_SPECULAR), Texture::GL_INDEX_SPECULAR);
        }

        // Normal map
        if(material->GetTextureCount(aiTextureType_HEIGHT)){
            material->GetTexture(aiTextureType_HEIGHT, 0, &texName);
            mesh.attachTexture(saveTexture(_directoryPath + "/" + texName.C_Str()), Texture::GL_INDEX_NORMAL_MAP);
        }
        else{
            mesh.attachTexture(saveTexture(PATH_DEFAULT_TEX_NORMAL), Texture::GL_INDEX_NORMAL_MAP);
        }
    }
    _meshes.push_back(std::move(mesh));
}

Texture* ModelMeshGroup::saveTexture(const std::string &pathTexture, bool sRGB) {

    // DLOG(INFO) << "Saving texture (no doubloons)" << pathTexture;

    // If the texture does not exist we create it
    if(_textures.find(pathTexture) == _textures.end())
        _textures.insert(std::make_pair(pathTexture, Texture(pathTexture, sRGB ? TextureType::SRGB: TextureType::DEFAULT)));
    return &_textures[pathTexture];
}

void ModelMeshGroup::draw(int nbInstancesToDraw){
    /**
     * We need to offset VBOs since we have every vertices and ids in respectively one VBO
     * Example: VBO ids:
     * 0 1 2 3 4 5 6 | 0 1 2 3 4 5 6
     *    Mesh1      |      Mesh2
     *
     * VBO vertices:
     * v0 v1 v2 v3 v4 v5 v6 | v0 v1 v2 v3 v4 v5 v6
     *           Mesh1      |      Mesh2
     *
     * Here we need to offset by 6 when pull from Mesh2 ids to get Mesh2 vertices
     */

    int offsetIndexes(0), offsetsVertices(0);
    for(auto& mesh : _meshes){
        mesh.bindTextures();
        glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
                                          mesh.getVertexCount(),
                                          GL_UNSIGNED_INT,
                                          (void*)(offsetIndexes*sizeof(unsigned int)),
                                          nbInstancesToDraw,
                                          offsetsVertices);

        offsetIndexes   += mesh.getElementIndex().size();
        offsetsVertices += mesh.getVertices().size();
    }
}


const std::vector<VertexDescriptor>& ModelMeshGroup::allVertices() const{
    return _allVertices;
}

std::vector<int> &ModelMeshGroup::allIndexes() {
    return _allIds;
}

void ModelMeshGroup::computeBoundingBox() {
    _boundingBox.compute(_allVertices);
}

const Geometry::BoundingBox& ModelMeshGroup::getBoundingBox() const {
    return _boundingBox;
}

Geometry::BoundingBox ModelMeshGroup::getBoundingBox(){
    return _boundingBox;
}

Geometry::BoundingBox* ModelMeshGroup::getBoundingBoxPtr(){
    return &_boundingBox;
}

void ModelMeshGroup::removeTextures() {
    for(auto& m: _meshes)
        m.textures().clear();
    _textures.clear();
}
