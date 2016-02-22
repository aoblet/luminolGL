#pragma once

#include "graphics/FrameBufferObject.hpp"
#include "graphics/Texture.h"

namespace Graphics{
    class BeautyFBO : public FrameBufferObject{
        Texture _beauty;
    public:
        BeautyFBO(const glm::ivec2 resTextures);
        virtual void build() override;
        Texture& beauty();
    };
}