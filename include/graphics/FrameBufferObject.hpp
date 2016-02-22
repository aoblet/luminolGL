#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include "TextureHandler.h"
#include <glm/vec2.hpp>

namespace Graphics{
    /**
     * FBO wrapper.
     * Textures are not handled here: avoid one indirection when accessing - subclasses responsibilities.
     */
    class FrameBufferObject{
    protected:
        GLuint _id;
        std::vector<GLuint> _drawBuffers;
        glm::ivec2 _resolutionTextures;

    public:
        FrameBufferObject(const glm::ivec2& resolutionTextures);
        virtual ~FrameBufferObject();

        void attachTexture(GLuint attachment, GLuint textureId);
        void attachRenderBuffer(GLuint attachment, GLuint renderBufferId);

        static void checkStatus();
        virtual void build() = 0;
        GLuint id() const;
        void bind();
        void unbind();

        void clearColor();
        void clearDepth();
        void clear();
        void clear(GLenum what);

        glm::ivec2& resolution();
    };
}