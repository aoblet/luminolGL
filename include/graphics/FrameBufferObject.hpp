#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include "TextureHandler.h"

namespace Graphics{
    class FrameBufferObject{
    protected:
        GLuint _id;
        std::vector<GLuint> _drawBuffers;
        int _widthTextures;
        int _heightTextures;

    public:
        FrameBufferObject(int widthTextures, int heightTextures);
        virtual ~FrameBufferObject();

        void attachFrameBufferTexture(GLuint attachement, GLuint textureId);

        static void checkStatus();
        virtual void build() = 0;
        GLuint id() const;
        void bind() const;
        void unbind() const;
    };
}