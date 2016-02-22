#include "graphics/PostFxFBO.hpp"

Graphics::PostFxFBO::PostFxFBO(const glm::ivec2 resTextures, int nbTextures):FrameBufferObject(resTextures),
                                                                            _nbTextures(nbTextures) {
    if(nbTextures < 1)
        throw std::runtime_error("PostFxFBO one texture at least must be created");

    build();
}

void Graphics::PostFxFBO::build() {
    for(int i=0; i<_nbTextures; ++i)
        _textures.add(Texture(_resolutionTextures.x, _resolutionTextures.y, TexParams::rgbaFBO()), std::to_string(i));

    bind();

    _drawBuffers = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers((GLsizei) _drawBuffers.size(), &_drawBuffers[0]);

    // Attach first fx texture to frame buffer
    attachTexture(_drawBuffers[0], texture(0).glId());

    FrameBufferObject::checkStatus();
    unbind();
}

Graphics::Texture &Graphics::PostFxFBO::texture(int index) {
    return _textures[std::to_string(index)];
}

void Graphics::PostFxFBO::changeCurrentTexture(int index) {
    attachTexture(_drawBuffers[0], _textures[std::to_string(index)].glId());
}
