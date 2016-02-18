#include "graphics/GeometricFBO.hpp"

using namespace Graphics;

void GeometricFBO::build(){
    bind();
    _drawBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers((GLsizei) _drawBuffers.size(), &_drawBuffers[0]);
    attachTexture(_drawBuffers[0], _color.glId());
    attachTexture(_drawBuffers[1], _normal.glId());
    attachTexture(GL_DEPTH_ATTACHMENT, _depth.glId());
    FrameBufferObject::checkStatus();
    unbind();
}

GeometricFBO::GeometricFBO(const glm::ivec2& resolutionTexture):
        FrameBufferObject(resolutionTexture),
        _color(resolutionTexture.x,  resolutionTexture.y, TextureType::FRAMEBUFFER_RGBA),
        _normal(resolutionTexture.x, resolutionTexture.y, TextureType::FRAMEBUFFER_NORMAL_ENCODED),
        _depth(resolutionTexture.x,  resolutionTexture.y, TextureType::FRAMEBUFFER_DEPTH){
    build();
}

Texture& GeometricFBO::color(){
    return _color;
}

Texture& GeometricFBO::normal(){
    return _normal;
}

Texture& GeometricFBO::depth(){
    return _depth;
}
