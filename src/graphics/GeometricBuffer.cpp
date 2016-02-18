#include "graphics/GeometricBuffer.hpp"

using namespace Graphics;

void GeometricBuffer::build(){
    bind();
    _drawBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers((GLsizei) _drawBuffers.size(), &_drawBuffers[0]);
    attachFrameBufferTexture(_drawBuffers[0], _color.glId());
    attachFrameBufferTexture(_drawBuffers[1], _normal.glId());
    attachFrameBufferTexture(GL_DEPTH_ATTACHMENT, _depth.glId());
    FrameBufferObject::checkStatus();
    unbind();
}

GeometricBuffer::GeometricBuffer(int widthTexture, int heightTexture):
        FrameBufferObject(widthTexture, heightTexture),
        _color(_widthTextures,  _heightTextures, TextureType::FRAMEBUFFER_RGBA),
        _normal(_widthTextures, _heightTextures, TextureType::FRAMEBUFFER_NORMAL_ENCODED),
        _depth(_widthTextures,  _heightTextures, TextureType::FRAMEBUFFER_DEPTH){
    build();
}

Texture& GeometricBuffer::color(){
    return _color;
}

Texture& GeometricBuffer::normal(){
    return _normal;
}

Texture& GeometricBuffer::depth(){
    return _depth;
}
