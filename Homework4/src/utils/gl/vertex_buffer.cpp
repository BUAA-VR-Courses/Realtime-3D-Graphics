#include "vertex_buffer.h"

namespace Utils::GL {

VertexBuffer::VertexBuffer(GLsizeiptr size, const void* data, BufferUsage usage)
    : Buffer(GL_ARRAY_BUFFER, size, data, usage) {}

GLint VertexBuffer::max_vertex_attributes() noexcept {
    GLint res;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &res);
    return res;
}

void VertexBuffer::bind_reset() {
    Buffer::bind_reset(GL_ARRAY_BUFFER);
}

}