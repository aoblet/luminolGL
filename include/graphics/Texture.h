//
// Created by mehdi on 12/02/16.
//

#ifndef LUMINOLGL_TEXTURE_H
#define LUMINOLGL_TEXTURE_H

#include <GL/glew.h>
#include <string>
#include <cstdlib>

#include "stb/stb_image.h"

namespace Graphics
{
    /**
     * Util struct when generating GL texture.
     */
    struct TexParams{

        //For glTexImage2D
        GLenum internalFormat;  /** GL_RGB, GL_RGBA8, GL_DEPTH_COMPONENT24, ... */
        GLenum format;          /** Specifies the format of the pixel data. GL_RED, GL_RG, GL_RGB, GL_BGR, ... */
        GLenum type;            /** Specifies the data type of the pixel data. GL_UNSIGNED_BYTE, GL_BYTE, GL_INT, GL_FLOAT, ... */

        //For glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S/T, mode)
        GLenum wrapMode;        /** GL_CLAMP_TO_EDGE, GL_REPEAT, ... */

        //For glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN/MAX_FILTER, mode)
        GLenum filterMode;      /** GL_LINEAR, GL_NEAREST, ... */
        bool mipmap;

        TexParams(GLenum internalForm=GL_RGB, GLenum form=GL_RGB, GLenum typ=GL_UNSIGNED_BYTE, GLenum wrap=GL_REPEAT, GLenum filter=0, bool mipm = true) :
                internalFormat(internalForm),
                format(form),
                type(typ),
                wrapMode(wrap),
                filterMode(filter),
                mipmap(mipm){}

        static TexParams depthFBO(){
            return TexParams(GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, false);
        }

        static TexParams depthShadowFBO(){
            TexParams t = depthFBO();
            t.wrapMode = GL_CLAMP_TO_BORDER;
            return t;
        }


        static TexParams rgbaFBO(){
            return TexParams(GL_RGBA8,  GL_RGBA, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_NEAREST, false);
        }

        static TexParams normalEncodedFBO(){
            return TexParams(GL_RGBA8,  GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST, false);
        }
    };

    enum TextureType{
        DEFAULT,
        FRAMEBUFFER_DEPTH,
        FRAMEBUFFER_RGBA,
        FRAMEBUFFER_NORMAL_ENCODED,
        FROM_FILE
    };

    class Texture {
    private:
        GLuint _texId           = 0;
        unsigned char * _data   = nullptr;
        int _width              = 0;
        int _height             = 0;
        int _bitDepth           = 0;
        TextureType _type;
        TexParams _texParams;
        std::string _path      = "";

        void setTextureParameters(TextureType type);    /** Update texParams according type and call genGlTex */
        void genGlTex();                                /** Generate GL texture based on _texParams. FBO are handled by _params: see TexParams::depth|rgba */
        void loadImage(const std::string &filePath);    /** Load a given image. Update _data, _bitDepth, _width, _height **/

    public:
        static const GLenum GL_INDEX_DIFFUSE    = GL_TEXTURE0;
        static const GLenum GL_INDEX_SPECULAR   = GL_TEXTURE1;
        static const GLenum GL_INDEX_NORMAL_MAP = GL_TEXTURE2;

        Texture(const Texture& texture);
        Texture(Texture&& texture);

        Texture(const std::string& path="", TextureType type=DEFAULT) ; /** Default */
        Texture(const std::string& path, TexParams texParams) ;         /** Texture File with custom texParams */
        Texture(int width, int height, TextureType fboType);            /** Fbo constructor */
        Texture(int width, int height, TexParams texParams);            /** Custom fbo constructor*/
        ~Texture();

        void bind();                            /** Just call glBindTexture() */
        void bind(GLenum textureBindingIndex);  /** set glActiveTexture to textureBindingIndex before calling glBindTexture() */
        void unbind();


        void sendGL(void* data);

        GLuint& glId();
        GLuint glId() const;
        int width() const;
        int height() const;

        unsigned char * data() const;
    };
}




#endif //LUMINOLGL_TEXTURE_H
