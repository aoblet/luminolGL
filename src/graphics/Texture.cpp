//
// Created by mehdi on 12/02/16.
//

#include "graphics/Texture.h"

#include <iostream>

namespace Graphics
{

    Texture::Texture() { }

    Texture::Texture(const std::string& path, TextureType type) {
        loadFromFile(path);
        setTextureParameters(type);
    }

    Texture::Texture(const std::string& path, TexParams params){
        loadFromFile(path);
        genGlTex(params);
    }

    Texture::Texture(int width, int height, TexParams params){
        _data = nullptr;
        _width = width;
        _height = height;
        genGlTex(params);
    }

    Texture::Texture(int width, int height, TextureType type){
        _data = nullptr;
        _width = width;
        _height = height;
        setTextureParameters(type);
    }

    void Texture::loadFromFile(const std::string & filePath){
        _data = stbi_load(filePath.c_str(), &_width, &_height, &_bitDepth, 3);
    }

    void Texture::setTextureParameters(TextureType type){
        switch(type){
            case DEFAULT:
                genGlTexDefault();
                break;
            case FRAMEBUFFER_DEPTH:
                genGlTexFramebufferDepth();
                break;
            case FRAMEBUFFER_RGBA:
                genGlTexFramebufferRGBA();
                break;
            default:
                genGlTexDefault();
                break;
        }
    }

    void Texture::genGlTex(TexParams params){
        glGenTextures(1, &_texId);
        glBindTexture(GL_TEXTURE_2D, _texId);
        glTexImage2D(GL_TEXTURE_2D, 0, params.internalFormat, _width, _height, 0, params.format, params.type, _data != nullptr ? _data : 0);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrapMode);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrapMode);

        if(params.mipmap){
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);
            return;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.filterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.filterMode);
    }

    void Texture::genGlTexDefault() {
        TexParams params(GL_RGB,  GL_RGB, GL_UNSIGNED_BYTE, GL_REPEAT, 0, true);
        genGlTex(params);
    }

    void Texture::genGlTexFramebufferDepth(){
        TexParams params(GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, false);
        genGlTex(params);
    }

    void Texture::genGlTexFramebufferRGBA(){
        TexParams params(GL_RGBA8,  GL_RGBA, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_NEAREST, false);
        genGlTex(params);
    }

    void Texture::bind() {
        glBindTexture(GL_TEXTURE_2D, _texId);
    }

    void Texture::bind(GLenum textureBindingIndex) {
        glActiveTexture(textureBindingIndex);
        glBindTexture(GL_TEXTURE_2D, _texId);
    }

    void Texture::unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint Texture::glId(){
        return _texId;
    }

    int Texture::width(){
        return _width;
    }

    int Texture::height(){
        return _height;
    }

}
