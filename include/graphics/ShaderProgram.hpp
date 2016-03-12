#pragma once

#include "graphics/Shader.hpp"
#include <GL/gl.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <memory>
#include <vector>

namespace Graphics{

    /**
     * Shader Program wrapper.
     * In order to prevent delete side effects of shared shader between ShaderProgram, each shader is a shared_ptr.
     */

    class ShaderProgram {
        GLuint _programId;
        std::shared_ptr<Shader> _vertexShader;
        std::shared_ptr<Shader> _geometryShader;
        std::shared_ptr<Shader> _fragShader;
        void compile();
        void checkLinkErrors() const;

    public:
        ShaderProgram(const std::string& vShader, const std::string& gShader, const std::string& fShader);
        ShaderProgram(const std::shared_ptr<Shader>& vShader, const std::shared_ptr<Shader>& fShader);
        ShaderProgram(const std::shared_ptr<Shader>& vShader, const std::string& fShader);
        ShaderProgram(const std::string& vShader            , const std::shared_ptr<Shader>& fShader);
        ~ShaderProgram();

        GLuint id() const;
        const std::shared_ptr<Shader> vShader() const;
        const std::shared_ptr<Shader> gShader() const;
        const std::shared_ptr<Shader> fShader() const;

        void updateUniform(const std::string& uniformName, float v);
        void updateUniform(const std::string& uniformName, int v);
        void updateUniform(const std::string& uniformName, const glm::ivec2 & v);
        void updateUniform(const std::string& uniformName, const glm::vec2 & v);
        void updateUniform(const std::string& uniformName, const glm::vec3 & v);
        void updateUniform(const std::string& uniformName, const glm::mat3 & v);
        void updateUniform(const std::string& uniformName, const glm::mat4 & v);
        void updateUniform(const std::string &uniformName, const std::vector<glm::vec3> &v);

        void updateBindingPointUBO(const std::string &uniformName, GLuint uboBindingPoint);
        void useProgram() const;

    };
}