#include "graphics/GeometricFBO.hpp"

using namespace Graphics;

void GeometricFBO::build(){
    bind();
    _drawBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers((GLsizei) _drawBuffers.size(), &_drawBuffers[0]);
    setDefaultWriting();
    FrameBufferObject::checkStatus();
    unbind();
}

GeometricFBO::GeometricFBO(const glm::ivec2& resolutionTexture):
        FrameBufferObject(resolutionTexture),
        _color(resolutionTexture.x,  resolutionTexture.y, TextureType::FRAMEBUFFER_RGBA),
        _normal(resolutionTexture.x, resolutionTexture.y, TextureType::FRAMEBUFFER_NORMAL_ENCODED),
        _depth(resolutionTexture.x,  resolutionTexture.y, TextureType::FRAMEBUFFER_DEPTH),
        _position(resolutionTexture.x,  resolutionTexture.y, TexParams(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST, false))
{
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

Texture &GeometricFBO::position() {
    return _position;
}


void GeometricFBO::setColorWritingOnly() {
    attachTexture(_drawBuffers[1], 0);      // normal
    attachTexture(_drawBuffers[2], 0);      // positions
    attachTexture(GL_DEPTH_ATTACHMENT, 0);  // depth
}

void GeometricFBO::setDefaultWriting() {
    attachTexture(_drawBuffers[0], _color.glId());
    attachTexture(_drawBuffers[1], _normal.glId());    // normal
    attachTexture(_drawBuffers[2], _position.glId());   // positions
    attachTexture(GL_DEPTH_ATTACHMENT, _depth.glId());  // depth
}
