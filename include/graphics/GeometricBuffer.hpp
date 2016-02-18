#pragma once

#include "graphics/FrameBufferObject.hpp"
#include "graphics/Texture.h"
#include <string>
#include <glm/vec2.hpp>

namespace Graphics{
    class GeometricBuffer : public FrameBufferObject{
        Texture _color;
        Texture _normal;
        Texture _depth;
    public:
        GeometricBuffer(int widthTexture, int heightTexture);
        virtual void build() override;
        Texture& color();
        Texture& normal();
        Texture& depth();
    };
}