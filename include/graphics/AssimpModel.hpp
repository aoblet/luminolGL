#pragma once

#include <vector>
#include <string>
#include "graphics/Mesh.h"
#include "graphics/VertexBufferObject.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "graphics/VertexArrayObject.h"

namespace Graphics{
    /**
     * Model loaded with assimp. Represents a "mesh" containing multiple meshes.
     * Has potentially multiple meshes and multiple textures per mesh.
     */

    class AssimpModel {
        static const std::string PATH_DEFAULT_TEX_DIFFUSE;
        static const std::string PATH_DEFAULT_TEX_SPECULAR;
        static const std::string PATH_DEFAULT_TEX_NORMAL;

        std::map<std::string, Texture> _textures;           /** Shared textures - Format: Path => textures  */
        std::vector<Mesh> _meshes;                          /** Contains meshes contained in the scene */
        std::string _directoryPath;                         /** Model directory path. Used for textures paths */
        glm::mat4 _modelMatrix;

        std::vector<VertexDescriptor> _allVertices;         /** All vertices of all meshes into one vector - for reduce to 1 vbo */
        std::vector<int> _allIds;                           /** All ids of all meshes into one vector - for reduce to 1 vbo */

        VertexBufferObject _verticesVBO;
        VertexBufferObject _idsVBO;

        /** Build aiMesh to Graphics::Mesh and add it to _meshes */
        void aiMeshToMesh(aiMesh* aiMesh, const aiScene* scene);
        /** Save the texture and return the storage address */
        Texture* saveTexture(const std::string& pathTexture);

    public:
        AssimpModel(const std::string& modelPath);
        AssimpModel(const AssimpModel& other) = delete;
        AssimpModel(AssimpModel&& other) = delete;

        /** Draw each mesh with its textures */
        void draw(VertexArrayObject& vao, int nbInstancesToDraw);

        std::vector<Mesh>& meshes();
        std::map<std::string, Texture>& textures();
        std::vector<VertexDescriptor>& allVertices();
        std::vector<int>& allIds();

        VertexBufferObject& verticesVBO();
        VertexBufferObject& idsVBO();
    };
}