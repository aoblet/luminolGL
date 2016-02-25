//
// Created by mehdi on 12/02/16.
//

#include "graphics/Texture.h"

#include <iostream>

namespace Graphics
{
    Texture::Texture(const std::string& path, TextureType type): _path(path) {
        // Default => empty texture
        if(path.empty()){
            setTextureParameters(type);
            return;
        }
        loadImage(path);
        setTextureParameters(TextureType::FROM_FILE);
    }

    Texture::Texture(const std::string& path, TexParams texParams): _texParams(texParams), _path(path) {
        loadImage(path);
        genGlTex();
    }

    Texture::Texture(int width, int height, TextureType fboType):_width(width), _height(height), _type(fboType) {
        setTextureParameters(fboType);
    }

    Texture::Texture(int width, int height, TexParams texParams):
            _texId(0), _data(nullptr), _width(width), _height(height), _bitDepth(0), _type(), _texParams(texParams) {
        genGlTex();
    }

    Texture::Texture(Texture &&texture) {
        std::swap(_texId, texture._texId);
        std::swap(_data, texture._data);
        std::swap(_width, texture._width);
        std::swap(_height, texture._height);
        std::swap(_bitDepth, texture._bitDepth);
        std::swap(_type, texture._type);
    }

    Texture::Texture(const Texture& texture):
            _texId(0), _data(nullptr), _width(texture._width), _height(texture._height), _bitDepth(texture._bitDepth),
            _texParams(texture._texParams), _path(texture._path){

        if(!_path.empty())
            loadImage(_path);
        genGlTex();
    }

    Texture::~Texture() {
        glDeleteTextures(1, &_texId);
        free(_data);
    }

    void Texture::loadImage(const std::string &filePath){
        _data = stbi_load(filePath.c_str(), &_width, &_height, &_bitDepth, 3);
    }

    void Texture::setTextureParameters(TextureType type){
        _type = type;

        if(type == TextureType::FRAMEBUFFER_DEPTH)
            _texParams = TexParams::depthFBO();
        else if(type == TextureType::FRAMEBUFFER_RGBA)
            _texParams = TexParams::rgbaFBO();
        else if(type == TextureType::FRAMEBUFFER_NORMAL_ENCODED)
            _texParams = TexParams::normalEncodedFBO();
        else
            _texParams = TexParams();

        genGlTex();
    }

    void Texture::genGlTex(){
        glDeleteTextures(1, &_texId);
        glGenTextures(1, &_texId);
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, _texParams.internalFormat, _width, _height, 0, _texParams.format, _texParams.type, _data);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _texParams.wrapMode);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _texParams.wrapMode);

        if(_texParams.mipmap){
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);
            unbind();
            return;
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _texParams.filterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _texParams.filterMode);
        unbind();
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

    GLuint& Texture::glId(){
        return _texId;
    }

    GLuint Texture::glId() const{
        return _texId;
    }

    int Texture::width(){
        return _width;
    }

    int Texture::height(){
        return _height;
    }
}
