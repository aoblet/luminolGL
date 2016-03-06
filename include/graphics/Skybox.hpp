#pragma once

#include "graphics/CubeMapTexture.hpp"
#include "graphics/ShaderProgram.hpp"
#include "graphics/VertexArrayObject.h"
#include "graphics/VertexBufferObject.h"
#include "graphics/Mesh.h"
#include "ModelMeshInstanced.hpp"

namespace Graphics{
    /**
     * Skybox is rendered as post treatment.
     * Instead of cube gl draw call we use a full screen quad.
     * 1: .vert  Trick the skybox cubeMap (quad) vertices to get the camera vector
     * 2: .frag: Compare if depth != empty => beauty else Skybox
     */
    class Skybox{
        const static std::string VERTEX_SHADER_PATH;
        const static std::string FRAGMENT_SHADER_PATH;

        CubeMapTexture _texture;
        ShaderProgram _shaderProgram;
    public:
        Skybox(const CubeMapTexture& tex);
        Skybox(CubeMapTexture&& tex);
        void updateUniforms(const glm::mat4 &screenToWorldNoTranslate, int unitTexSky, int unitTexDepth, int unitTexBeauty);
        /**
         * Bind the CubeMapTexture
         */
        void bindTexture(GLenum unitTex = GL_TEXTURE0);
        void useProgramShader();
        ShaderProgram& shaderProgram();
    };
}