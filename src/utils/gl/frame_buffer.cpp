#include "frame_buffer.h"

namespace Utils::GL {

FrameBuffer::FrameBuffer(FrameBufferType type) : type(type) {
    glGenFramebuffers(1, id.init_ptr());
}

FrameBuffer::~FrameBuffer() {
    clear();
}

FrameBuffer::FrameBuffer(FrameBuffer&& fb) noexcept : Obj(std::move(fb.id)), type(fb.type), attachments(std::move(fb.attachments)) {}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& fb) noexcept {
    clear();
    id = std::move(fb.id);
    type = fb.type;
    attachments = std::move(fb.attachments);
    return *this;
}

void FrameBuffer::clear() {
    if (id.is_valid()) {
        glDeleteFramebuffers(1, id.del_ptr());
        id.clear();
    }
    attachments.clear();
}

void FrameBuffer::bind() const {
    glBindFramebuffer(type, id);
}

void FrameBuffer::bind_reset(FrameBufferType type) {
    glBindFramebuffer(type, static_cast<GLuint>(0));
}

void FrameBuffer::attach(FrameBufferAttachment attachment, Texture2D* texture, GLuint level) {
    bind();
    glFramebufferTexture2D(type, attachment, texture->type, texture->get_id(), level);
    bind_reset(type);
    attachments[attachment] = texture;
}

Texture* FrameBuffer::get_texture(FrameBufferAttachment attachment) const {
    auto it = attachments.find(attachment);
    if (it == attachments.end()) {
        return nullptr;
    }
    return it->second;
}

bool FrameBuffer::is_complete() const {
    bind();
    bool status = glCheckFramebufferStatus(type);
    bind_reset();
    return status == GL_FRAMEBUFFER_COMPLETE;
}



}