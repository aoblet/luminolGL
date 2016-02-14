#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>

namespace Graphics{

    /**
     * Shader OpenGL wrapper.
     */
    class Shader{
        GLuint _idShader;
        GLenum _type;
        std::string _path;
    public:
        Shader(GLenum type, const std::string& path);
        ~Shader();

        std::string getPath() const;
        GLuint id() const;
        void compile();
        void checkCompile() const;
        void changeProperties(GLenum type, const std::string& path);
    };
}