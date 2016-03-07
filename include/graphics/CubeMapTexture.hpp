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
        /**
         * @param files if given must follow this order right left top bottom back front. Otherwise the constructor try to find them with extension param.
         * @param unitTextureTarget which unit texture to bind the CubeMapTexture.
         */
        CubeMapTexture(const std::string& folderTextures, const std::vector<std::string>& files={}, const std::string& extension = ".png");
        CubeMapTexture(CubeMapTexture && other);
        CubeMapTexture(const CubeMapTexture & other);
        ~CubeMapTexture();

        void bind(GLenum unitTextureTarget = GL_TEXTURE0);
        void unbind();
    };
}