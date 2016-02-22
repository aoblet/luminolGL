#pragma once

#include "graphics/FrameBufferObject.hpp"
#include "graphics/Texture.h"

namespace Graphics{
    class PostFxFBO : public FrameBufferObject{
        TextureHandler _textures;
        int _nbTextures;
    public:
        PostFxFBO(const glm::ivec2 resTextures, int nbTextures);
        virtual void build() override;
        Texture& texture(int index);
        void changeCurrentTexture(int index);
    };
}