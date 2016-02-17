#include "graphics/FrameBufferObject.hpp"
#include <stdexcept>

using namespace Graphics;

FrameBufferObject::FrameBufferObject(int widthTextures, int heightTextures): _widthTextures(widthTextures), _heightTextures(heightTextures){
    glGenFramebuffers(1, &_id);
}

GLuint FrameBufferObject::id() const {
    return _id;
}

void FrameBufferObject::checkStatus(){
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Error on building framebuffer");
}

FrameBufferObject::~FrameBufferObject() {
    glDeleteFramebuffers(1, &_id);
}

void FrameBufferObject::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
}

void FrameBufferObject::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObject::attachFrameBufferTexture(GLuint attachement, GLuint textureId) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachement , GL_TEXTURE_2D, textureId, 0);
}
