//
// Created by mehdi on 16/02/16.
//

#include <glm/gtc/constants.hpp>
#include <fstream>
#include "graphics/Mesh.h"

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
        _vertices.insert(_vertices.begin(), std::make_move_iterator(vertices.begin()), std::make_move_iterator(vertices.begin()));
    }

    void Mesh::addElementIndexes(const std::vector<int> &index) {
        _elementIndex.insert(_elementIndex.begin(), index.begin(), index.end());
    }

    void Mesh::addElementIndexes(std::vector<int> &&index) {
        _elementIndex.insert(_elementIndex.begin(), std::make_move_iterator(index.begin()), std::make_move_iterator(index.begin()));
    }

    Mesh Mesh::loadMesh(const std::string &filePath) {
        Mesh mesh;
        //TODO: load model with assimp
        return mesh;
    }

    void Mesh::setTriangleCount(unsigned int value) {
        _triangleCount = value;
        _vertexCount = value * 3;
    }


    void Mesh::saveOBJ(const std::string &filePath, const std::string& filename) {

        std::ofstream file (filePath + filename + ".obj");

        if (!file.is_open())
            throw std::runtime_error("Unable to save mesh at \"" + filePath + "\"");

        file << "# " + filename + ".obj" << std::endl;

        // write positions
        for(int i = 0; i < _vertices.size(); ++i){
            file << "v ";
            file << _vertices[i].position.x << " ";
            file << _vertices[i].position.y << " ";
            file << _vertices[i].position.z << std::endl;
        }

        // write texcoords
        for(int i = 0; i < _vertices.size(); ++i){
            file << "vt ";
            file << _vertices[i].texcoord.x << " ";
            file << _vertices[i].texcoord.y << std::endl;
        }

        // write normals
        for(int i = 0; i < _vertices.size(); ++i){
            file << "vn ";
            file << _vertices[i].normal.x << " ";
            file << _vertices[i].normal.y << " ";
            file << _vertices[i].normal.z << std::endl;
        }

        //write indexes
        for(int i = 0; i < _elementIndex.size(); i+=3){
            file << "f ";
            file << _elementIndex[i] + 1 << "/" << _elementIndex[i] + 1 << "/" << _elementIndex[i] + 1 << " ";
            file << _elementIndex[i+1] + 1 << "/" << _elementIndex[i+1] + 1 << "/" << _elementIndex[i+1] + 1 << " ";
            file << _elementIndex[i+2] + 1 << "/" << _elementIndex[i+2] + 1 << "/" << _elementIndex[i+2] + 1 << std::endl;
        }

    }
}