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
     * Mesh group loaded with assimp. Represents a "mesh" containing multiple meshes.
     * Has potentially multiple meshes and multiple textures per mesh.
     */

    class ModelMeshGroup {
        static const std::string PATH_DEFAULT_TEX_DIFFUSE;
        static const std::string PATH_DEFAULT_TEX_SPECULAR;
        static const std::string PATH_DEFAULT_TEX_NORMAL;

        std::map<std::string, Texture> _textures;           /** Shared textures - Format: Path => textures  */
        std::vector<Mesh> _meshes;                          /** Contains meshes contained in the scene */
        std::string _directoryPath;                         /** Model directory path. Used for textures paths */
        glm::mat4 _modelMatrix;
        Geometry::BoundingBox _boundingBox;

        std::vector<VertexDescriptor> _allVertices;         /** All vertices of all meshes into one vector - for reduce to 1 vbo */
        std::vector<int> _allIds;                           /** All ids of all meshes into one vector - for reduce to 1 vbo */

        /** Build aiMesh to Graphics::Mesh and add it to _meshes */
        void aiMeshToMesh(aiMesh* aiMesh, const aiScene* scene);
        void loadMeshes(const aiScene* scene);
        /** Save the texture and return the storage address */
        Texture* saveTexture(const std::string& pathTexture);

    public:
        ModelMeshGroup(const std::string& modelPath);
        ModelMeshGroup(const ModelMeshGroup & other) = delete;
        ModelMeshGroup(ModelMeshGroup && other) = delete;

        /** Draw each mesh with its textures.
         * Since we are in instanced renderer we call this method from ModelMeshInstanced::draw which binds the good VAO.
         */
        void draw(int nbInstancesToDraw);
        void computeBoundingBox();
        const Geometry::BoundingBox& getBoundingBox() const;

        std::vector<Mesh>& meshes();
        std::map<std::string, Texture>& textures();
        std::vector<VertexDescriptor>& allVertices();
        std::vector<int>&allIndexes();
    };
}