//
// Created by mehdi on 16/02/16.
//

#include <glm/gtc/constants.hpp>
#include "graphics/Mesh.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <stdexcept>
#include <glm/ext.hpp>

namespace Graphics
{

    Mesh::Mesh() : _vertexCount(0), _triangleCount(0) { }

    Mesh::Mesh(Mesh &&mesh) :
            _vertexCount(mesh._vertexCount),
            _triangleCount(mesh._triangleCount),
            _vertices(std::move(mesh._vertices)),
            _elementIndex(std::move(mesh._elementIndex)),
            _textures(std::move(mesh._textures))
    {
    }

    void Mesh::attachTexture(Graphics::Texture* tex, GLenum textureNumber) {
        _textures.insert({textureNumber, tex});
    }

    void Mesh::bindTextures() {
        for(auto& tex : _textures){
            tex.second->bind(tex.first);
        }
    }

    const std::vector<VertexDescriptor> &Mesh::getVertices() const {
        return _vertices;
    }


    const std::vector<int> &Mesh::getElementIndex() const {
        return _elementIndex;
    }


    int Mesh::getVertexCount() const {
        return _vertexCount;
    }

    int Mesh::getTriangleCount() const {
        return _triangleCount;
    }

    void Mesh::computeBoundingBox() {
        _boundaries.compute(_vertices);
    }

    Mesh Mesh::genCube() {
        Mesh mesh;

        mesh._vertices = {
                Graphics::VertexDescriptor(-0.5, -0.5, 0.5, 0, 0, 1, 0.f, 0.f),
                Graphics::VertexDescriptor(0.5, -0.5, 0.5, 0, 0, 1, 0.f, 1.f),
                Graphics::VertexDescriptor(-0.5, 0.5, 0.5, 0, 0, 1, 1.f, 0.f),
                Graphics::VertexDescriptor(0.5, 0.5, 0.5, 0, 0, 1, 1.f, 1.f),
                Graphics::VertexDescriptor(-0.5, 0.5, 0.5, 0, 1, 0, 0.f, 0.f),
                Graphics::VertexDescriptor(0.5, 0.5, 0.5, 0, 1, 0, 0.f, 1.f),
                Graphics::VertexDescriptor(-0.5, 0.5, -0.5, 0, 1, 0, 1.f, 0.f),
                Graphics::VertexDescriptor(0.5, 0.5, -0.5, 0, 1, 0, 1.f, 1.f),
                Graphics::VertexDescriptor(-0.5, 0.5, -0.5, 0, 0, -1, 0.f, 0.f),
                Graphics::VertexDescriptor(0.5, 0.5, -0.5, 0, 0, -1, 0.f, 1.f),
                Graphics::VertexDescriptor(-0.5, -0.5, -0.5, 0, 0, -1, 1.f, 0.f),
                Graphics::VertexDescriptor(0.5, -0.5, -0.5, 0, 0, -1, 1.f, 1.f),
                Graphics::VertexDescriptor(-0.5, -0.5, -0.5, 0, -1, 0, 0.f, 0.f),
                Graphics::VertexDescriptor(0.5, -0.5, -0.5, 0, -1, 0, 0.f, 1.f),
                Graphics::VertexDescriptor(-0.5, -0.5, 0.5, 0, -1, 0, 1.f, 0.f),
                Graphics::VertexDescriptor(0.5, -0.5, 0.5, 0, -1, 0, 1.f, 1.f),
                Graphics::VertexDescriptor(0.5, -0.5, 0.5, 1, 0, 0, 0.f, 0.f),
                Graphics::VertexDescriptor(0.5, -0.5, -0.5, 1, 0, 0, 0.f, 1.f),
                Graphics::VertexDescriptor(0.5, 0.5, 0.5, 1, 0, 0, 1.f, 0.f),
                Graphics::VertexDescriptor(0.5, 0.5, 0.5, 1, 0, 0, 1.f, 0.f),
                Graphics::VertexDescriptor(0.5, 0.5, -0.5, 1, 0, 0, 1.f, 1.f),
                Graphics::VertexDescriptor(-0.5, -0.5, -0.5, -1, 0, 0, 0.f, 1.f),
                Graphics::VertexDescriptor(-0.5, -0.5, 0.5, -1, 0, 0, 1.f, 1.f),
                Graphics::VertexDescriptor(-0.5, 0.5, -0.5, -1, 0, 0, 0.f, 0.f),
                Graphics::VertexDescriptor(-0.5, 0.5, -0.5, -1, 0, 0, 0.f, 0.f),
                Graphics::VertexDescriptor(-0.5, -0.5, 0.5, -1, 0, 0, 1.f, 1.f),
                Graphics::VertexDescriptor(-0.5, 0.5, 0.5, -1, 0, 0, 1.f, 0.f)
        };

        mesh._elementIndex = {
                0, 1, 2,
                2, 1, 3,
                4, 5, 6,
                6, 5, 7,
                8, 9, 10,
                10, 9, 11,
                12, 13, 14,
                14, 13, 15,
                16, 17, 18,
                19, 17, 20,
                21, 22, 23,
                24, 25, 26
        };

        mesh._triangleCount = 12;
        mesh._vertexCount = mesh._triangleCount * 3;

        mesh.computeBoundingBox();

        return mesh;
    }

    Mesh Mesh::genPlane(float width, float height, float textureLoop, const glm::vec3 & offset) {
        Mesh mesh;

        mesh._vertices = {
                Graphics::VertexDescriptor(-width/2 + offset.x, offset.y, height/2 + offset.z, 0, 1, 0, 0.f, 0.f),
                Graphics::VertexDescriptor(width/2 + offset.x, offset.y, height/2 + offset.z, 0, 1, 0, 0.f, textureLoop),
                Graphics::VertexDescriptor(-width/2 + offset.x, offset.y, -height/2 + offset.z, 0, 1, 0, textureLoop, 0.f),
                Graphics::VertexDescriptor(width/2 + offset.x, offset.y, -height/2 + offset.z, 0, 1, 0, textureLoop, textureLoop)
        };

        mesh._elementIndex = {
                0, 1, 2,
                2, 1, 3
        };

        mesh._triangleCount = 2;
        mesh._vertexCount = mesh._triangleCount * 3;

        mesh.computeBoundingBox();

        return mesh;
    }


    Mesh Mesh::genSphere(int latitudeBands, int longitudeBands, float radius, const glm::vec3 &offset) {
        Mesh mesh;

        std::vector<Graphics::VertexDescriptor> sphereVertices;
        for (int latNumber = 0; latNumber <= latitudeBands; latNumber++) {
            float theta = glm::pi<float>()*(float(latNumber) / float(latitudeBands));
            float sinTheta = glm::sin(theta);
            float cosTheta = glm::cos(theta);

            for (int longNumber = 0; longNumber <= longitudeBands; longNumber++) {
                float phi = glm::two_pi<float>() * (float(longNumber)  / float(longitudeBands));
                float sinPhi = glm::sin(phi);
                float cosPhi = glm::cos(phi);

                float x = sinPhi * sinTheta;
                float y = cosTheta;
                float z = cosPhi * sinTheta;

                float u = 1.f - (float(longNumber) / float(longitudeBands));
                float v = 1.f - (float(latNumber) / float(latitudeBands));

                sphereVertices.push_back(Graphics::VertexDescriptor(x*radius + offset.x, y * radius + offset.y, z * radius + offset.z, x, y, z, u, v));
            }
        }

        std::vector<int> sphereIds;
        for (int latNumber = 0; latNumber < latitudeBands; latNumber++) {
            for (int longNumber = 0; longNumber < longitudeBands; longNumber++) {
                int first = (latNumber * (longitudeBands + 1)) + longNumber;
                int second = first + longitudeBands + 1;
                sphereIds.push_back(first);
                sphereIds.push_back(second);
                sphereIds.push_back(first + 1);

                sphereIds.push_back(second);
                sphereIds.push_back(second + 1);
                sphereIds.push_back(first + 1);
            }
        }

        mesh.addVertices(sphereVertices);
        mesh.addElementIndexes(sphereIds);

        mesh._vertexCount = latitudeBands * longitudeBands * 6;
        mesh._triangleCount = mesh._vertexCount * 2;

        mesh.computeBoundingBox();

        return mesh;
    }

    const Geometry::BoundingBox &Mesh::getBoundingBox() {
        return _boundaries;
    }

    void Mesh::addVertices(const std::vector<VertexDescriptor> &vertices) {
        _vertices.insert(_vertices.begin(), vertices.begin(), vertices.end());
    }

    void Mesh::addVertices(std::vector<VertexDescriptor> &&vertices) {
        _vertices.insert(_vertices.begin(), std::make_move_iterator(vertices.begin()), std::make_move_iterator(vertices.end()));
    }

    void Mesh::addElementIndexes(const std::vector<int> &index) {
        _elementIndex.insert(_elementIndex.end(), index.begin(), index.end());
    }

    void Mesh::addElementIndexes(std::vector<int> &&index) {
        _elementIndex.insert(_elementIndex.end(), std::make_move_iterator(index.begin()), std::make_move_iterator(index.end()));
    }


    void Mesh::setTriangleCount(unsigned int value) {
        _triangleCount = value;
        _vertexCount = value * 3;
    }

    Mesh Mesh::loadMesh(const std::string &modelPath) {
        Assimp::Importer aImporter;
        const aiScene* scene = aImporter.ReadFile(modelPath, aiProcessPreset_TargetRealtime_MaxQuality);

        if(!scene)
            throw std::runtime_error("Mesh::loadMesh with assimp - fail while importing " + modelPath + "\n" + aImporter.GetErrorString());

        Mesh mesh;

        const aiMatrix4x4& t = scene->mRootNode->mTransformation;
        glm::mat4 modelMatrix = glm::mat4(t.a1, t.a2, t.a3, t.a4,
                                          t.b1, t.b2, t.b3, t.b4,
                                          t.c1, t.c2, t.c3, t.c4,
                                          t.d1, t.d2, t.d3, t.d4);

        for(unsigned int k=0; k<scene->mNumMeshes; ++k){
            const aiMesh* aiMesh = scene->mMeshes[k];

            // update element indexes
            for(unsigned int j=0; j<aiMesh->mNumFaces; ++j){
                const aiFace& face = aiMesh->mFaces[j];
                mesh._elementIndex.push_back(face.mIndices[0]);
                mesh._elementIndex.push_back(face.mIndices[1]);
                mesh._elementIndex.push_back(face.mIndices[2]);
            }
            mesh._triangleCount += aiMesh->mNumFaces;

            // positions, normals and uv
            for(unsigned int i=0; i<aiMesh->mNumVertices; ++i){
                VertexDescriptor vd;
                const aiVector3D& vPos = aiMesh->mVertices[i];
                const aiVector3D& vNormal = aiMesh->mNormals[i];
                const aiVector3D* uv = aiMesh->mTextureCoords[0];
                vd.position = glm::vec3((modelMatrix * glm::vec4(vPos.x, vPos.y, vPos.z, 1)));
                vd.normal   = glm::vec3(vNormal.x, vNormal.y, vNormal.z);
                vd.texcoord = aiMesh->HasTextureCoords(0) ? glm::vec2(uv[i].x, 1. - uv[i].y) : glm::vec2(0,0);
                mesh._vertices.push_back(std::move(vd));
            }
        }
        mesh._vertexCount = mesh._triangleCount*3;
        mesh.computeBoundingBox();
        return mesh;
    }
}