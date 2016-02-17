#pragma once

#include "graphics/FrameBufferObject.hpp"
#include "graphics/Texture.h"
#include <string>
#include <glm/vec2.hpp>

namespace Graphics{
    class GeometricBuffer : public FrameBufferObject{
        const static std::string TEXTURE_COLOR;
        const static std::string TEXTURE_NORMAL;
        const static std::string TEXTURE_DEPTH;

        const static std::string DRAW_BUFFER_COLOR;
        const static std::string DRAW_BUFFER_NORMAL;
    public:
        GeometricBuffer(int widthTexture, int heightTexture);
        virtual void build() override;
        Texture& color();
        Texture& normal();
        Texture& depth();
    };
}