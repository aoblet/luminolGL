#pragma once

#include "graphics/CubeMapTexture.hpp"
#include "graphics/ShaderProgram.hpp"
#include "graphics/VertexArrayObject.h"
#include "graphics/VertexBufferObject.h"
#include "graphics/Mesh.h"
#include "ModelMeshInstanced.hpp"

namespace Graphics{
    class Skybox{
        const static std::string VERTEX_SHADER_PATH;
        const static std::string FRAGMENT_SHADER_PATH;
        const static std::string CUBE_MODEL_PATH;

        CubeMapTexture _texture;
        ShaderProgram _shaderProgram;
        ModelMeshInstanced _cube;
    public:
        Skybox(const CubeMapTexture& tex);
        Skybox(CubeMapTexture&& tex);
        void draw(const glm::mat4& MVP);
    };
}