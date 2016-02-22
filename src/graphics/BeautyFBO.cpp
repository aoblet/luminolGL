#include "graphics/BeautyFBO.hpp"

using namespace Graphics;

BeautyFBO::BeautyFBO(const glm::ivec2 resTextures):FrameBufferObject(resTextures),
                                                   _beauty(resTextures.x, resTextures.y, TexParams::rgbaFBO()){
    build();
}

void BeautyFBO::build() {
    bind();
    // Initialize DrawBuffers
    _drawBuffers = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers((GLsizei) _drawBuffers.size(), &_drawBuffers[0]);
    attachTexture(_drawBuffers[0], _beauty.glId());
    FrameBufferObject::checkStatus();
    unbind();
}

Texture& BeautyFBO::beauty() {
    return _beauty;
}