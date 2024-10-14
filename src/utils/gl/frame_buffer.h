#ifndef UTILS_GL_FRAME_BUFFER_H
#define UTILS_GL_FRAME_BUFFER_H

#include <vector>
#include <map>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "utils/gl/core.h"
#include "utils/gl/texture.h"

namespace Utils::GL {

// class Texture2D; // forward declaration

class FrameBuffer : public Obj {
public:
    FrameBuffer(FrameBufferType type=GL_FRAMEBUFFER);
    ~FrameBuffer();
    FrameBuffer(FrameBuffer&& fb) noexcept;
    FrameBuffer& operator=(FrameBuffer&& fb) noexcept;
    
    FrameBufferType type;

    void clear();
    void bind() const;
    static void bind_reset(FrameBufferType type=GL_FRAMEBUFFER);

    void attach(FrameBufferAttachment attachment, Texture2D* texture, GLuint level=0);

    Texture* get_texture(FrameBufferAttachment attachment) const;

    bool is_complete() const;

private:
    std::map<FrameBufferAttachment, Texture2D *> attachments;
};

}
#endif // UTILS_GL_FRAME_BUFFER_H