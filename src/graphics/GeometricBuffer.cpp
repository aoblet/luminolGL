#include "graphics/GeometricBuffer.hpp"

using namespace Graphics;

const std::string GeometricBuffer::TEXTURE_COLOR        = "TextureColor";
const std::string GeometricBuffer::TEXTURE_NORMAL       = "TextureNormal";
const std::string GeometricBuffer::TEXTURE_DEPTH        = "TextureDepth";

void GeometricBuffer::build(){
    bind();
    _textures.add(Texture(_widthTextures, _heightTextures, TextureType::FRAMEBUFFER_RGBA), TEXTURE_COLOR);
    _textures.add(Texture(_widthTextures, _heightTextures, TextureType::FRAMEBUFFER_NORMAL_ENCODED), TEXTURE_NORMAL);
    _textures.add(Texture(_widthTextures, _heightTextures, TextureType::FRAMEBUFFER_DEPTH), TEXTURE_DEPTH);
    _drawBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(_drawBuffers.size(), &_drawBuffers[0]);
    attachFrameBufferTexture(_drawBuffers[0], _textures[TEXTURE_COLOR].glId());
    attachFrameBufferTexture(_drawBuffers[1], _textures[TEXTURE_NORMAL].glId());
    attachFrameBufferTexture(GL_DEPTH_ATTACHMENT, _textures[TEXTURE_DEPTH].glId());
    FrameBufferObject::checkStatus();
    unbind();
}

GeometricBuffer::GeometricBuffer(int widthTexture, int heightTexture):FrameBufferObject(widthTexture, heightTexture) {
    build();
}

Texture& GeometricBuffer::color(){
    return _textures[GeometricBuffer::TEXTURE_COLOR];
}

Texture& GeometricBuffer::normal(){
    return _textures[GeometricBuffer::TEXTURE_NORMAL];
}

Texture& GeometricBuffer::depth(){
    return _textures[GeometricBuffer::TEXTURE_DEPTH];
}
