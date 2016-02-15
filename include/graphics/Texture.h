//
// Created by mehdi on 12/02/16.
//

#ifndef LUMINOLGL_TEXTURE_H
#define LUMINOLGL_TEXTURE_H

#include <GL/glew.h>
#include <string>

#include "stb/stb_image.h"

namespace Graphics
{
    struct TexParams{

        //For glTexImage2D
        GLenum internalFormat;  // GL_RGB, GL_RGBA8, GL_DEPTH_COMPONENT24, ...
        GLenum format;  // Specifies the format of the pixel data. GL_RED, GL_RG, GL_RGB, GL_BGR, ...
        GLenum type; //Specifies the data type of the pixel data. GL_UNSIGNED_BYTE, GL_BYTE, GL_INT, GL_FLOAT, ...

        //For glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S/T, mode)
        GLenum wrapMode; // GL_CLAMP_TO_EDGE, GL_REPEAT, ...

        //For glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN/MAX_FILTER, mode)
        GLenum filterMode; // GL_LINEAR, GL_NEAREST, ...


        bool mipmap;

        TexParams(){}

        TexParams(GLenum internalForm, GLenum form, GLenum typ, GLenum wrap, GLenum filter, bool mipm = false) :
                internalFormat(internalForm),
                format(form),
                type(typ),
                wrapMode(wrap),
                filterMode(filter),
                mipmap(mipm)
        {}
    };

    enum TextureType{
        FRAMEBUFFER_DEPTH,
        FRAMEBUFFER_RGBA,
        DEFAULT
    };

    class Texture {
    private:
        GLuint _texId;
        unsigned char * _data;
        int _width;
        int _height;
        int _bitDepth;

        void setTextureParameters(TextureType type);

        void genGlTex(TexParams params);
        void genGlTexDefault();
        void genGlTexFramebufferDepth();
        void genGlTexFramebufferRGBA();

        void loadFromFile(const std::string & filePath);

    public:
        Texture();
        Texture(const std::string& path, TextureType type = DEFAULT);
        Texture(const std::string& path, TexParams params);
        Texture(int width, int height, TexParams params);
        Texture(int width, int height, TextureType type = DEFAULT);
        void bind(); //just call glBindTexture()
        void bind(GLenum textureBindingIndex); //set glActiveTexture to textureBindingIndex before calling glBindTexture()
        void unbind();
        GLuint glId();
        int width();
        int height();
    };
}




#endif //LUMINOLGL_TEXTURE_H
