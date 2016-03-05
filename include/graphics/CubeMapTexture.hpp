#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <string>

namespace Graphics{
    /**
     * Cube map texture.
     * It is not inherited from Texture because texture storage is slightly different.
     */

    class CubeMapTexture {
        GLuint _texId;
        std::string _folder;
        std::vector<std::string> _files;
    public:
        CubeMapTexture(const std::string& folderTextures, const std::vector<std::string>& files={},
                       const std::string& extension = ".png", GLenum unitTextureTarget = GL_TEXTURE0);
        CubeMapTexture(CubeMapTexture && other);
        CubeMapTexture(const CubeMapTexture & other);
        ~CubeMapTexture();

        void bind(GLenum unitTextureTarget = GL_TEXTURE0);
        void unbind();
    };
}