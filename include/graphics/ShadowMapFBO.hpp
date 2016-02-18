#pragma once

#include "graphics/FrameBufferObject.hpp"

namespace Graphics{

    class ShadowMapFBO: public FrameBufferObject{
        Texture _shadowTexture;
        GLuint _shadowRenderBuffer; /** Render buffer since we don't need to read shadow color in a texture*/
    public:
        ShadowMapFBO(const glm::ivec2& resolutionTexture);
        void build() override;
        Texture& shadowTexture();
    };
}