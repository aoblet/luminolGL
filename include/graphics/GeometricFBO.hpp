#pragma once

#include "graphics/FrameBufferObject.hpp"
#include "graphics/Texture.h"
#include <string>
#include <glm/vec2.hpp>

namespace Graphics{
    class GeometricFBO : public FrameBufferObject{
        Texture _color;
        Texture _normal;
        Texture _depth;
        Texture _position;
    public:
        GeometricFBO(const glm::ivec2& resolutionTexture);
        virtual void build() override;
        Texture& color();
        Texture& normal();
        Texture& depth();
        Texture& position();

        void setColorWritingOnly();
        void setDefaultWriting();
    };
}