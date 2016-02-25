#pragma once

#include <vector>
#include "graphics/Mesh.h"
#include "graphics/VertexBufferObject.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "graphics/VertexArrayObject.h"

namespace Graphics{
    /**
     * Model loaded with assimp.
     * Has potentially multiple meshes and multiple textures per mesh.
     */

    class AssimpModel {
        std::vector<Mesh> _meshes;                  /** Contains meshes contained in the scene */
        std::map<std::string, Texture> _textures;   /** Shared textures - Format: Path => textures  */
        std::string _directoryPath;                 /** Model directory path */
        glm::mat4 _modelMatrix;

        std::vector<VertexDescriptor> _allVertices;
        std::vector<int> _allIds;

        VertexBufferObject _verticesVBO;
        VertexBufferObject _idsVBO;

        /** Build aiMesh to Graphics::Mesh and add it to _meshes */
        void aiMeshToMesh(aiMesh* aiMesh, const aiScene* scene);
        /** Save the texture and return the storage address */
        Texture* saveTexture(const std::string& texName);
        void buildVBOS();

    public:
        AssimpModel(const std::string& modelPath);
        AssimpModel(const AssimpModel& other) = delete;
        AssimpModel(AssimpModel&& other) = delete;

        void draw(VertexArrayObject& vao, int instanceNumber);

        std::vector<Mesh>& meshes();
        std::map<std::string, Texture>& textures();
        std::vector<VertexDescriptor>& allVertices();
        std::vector<int>& allIds();

        VertexBufferObject& verticesVBO();
        VertexBufferObject& idsVBO();
    };
}