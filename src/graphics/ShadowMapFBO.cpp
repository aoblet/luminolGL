#include "graphics/ShadowMapFBO.hpp"

using namespace Graphics;

ShadowMapFBO::ShadowMapFBO(const glm::ivec2 &resolutionTexture):
        FrameBufferObject(resolutionTexture),
        _shadowTexture(_resolutionTextures.x, _resolutionTextures.y, TexParams::depthShadowFBO()){
   build();

}

void ShadowMapFBO::build() {
    bind();

    // Create a render buffer since we don't need to read shadow color in a texture
    glGenRenderbuffers(1, &_shadowRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _shadowRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, _resolutionTextures.x, _resolutionTextures.y);
    attachRenderBuffer(GL_COLOR_ATTACHMENT0, _shadowRenderBuffer);

    // shadow texture 2x2 percentage closer
    _shadowTexture.bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC,  GL_LEQUAL);
    _shadowTexture.unbind();

    // Attach the shadow texture to the depth attachment
    attachTexture(GL_DEPTH_ATTACHMENT, _shadowTexture.glId());
    unbind();
}

Texture &ShadowMapFBO::shadowTexture() {
    return _shadowTexture;
}
