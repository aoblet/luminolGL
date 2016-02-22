#include "graphics/FrameBufferObject.hpp"
#include <stdexcept>

using namespace Graphics;

FrameBufferObject::FrameBufferObject(const glm::ivec2& resolutionTextures): _resolutionTextures(resolutionTextures){
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

void FrameBufferObject::bind(){
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
}

void FrameBufferObject::unbind(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObject::attachTexture(GLuint attachment, GLuint textureId) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment , GL_TEXTURE_2D, textureId, 0);
}

void FrameBufferObject::attachRenderBuffer(GLuint attachment, GLuint renderBufferId) {
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, attachment , GL_RENDERBUFFER, renderBufferId);
}

glm::ivec2 &FrameBufferObject::resolution() {
    return _resolutionTextures;
}

void FrameBufferObject::clear(GLenum what) {
    glClear(what);
}

void FrameBufferObject::clearColor() {
    clear(GL_COLOR_BUFFER_BIT);
}

void FrameBufferObject::clearDepth() {
    clear(GL_DEPTH_BUFFER_BIT);
}

void FrameBufferObject::clear() {
    clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
