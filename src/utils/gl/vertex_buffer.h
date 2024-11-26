#ifndef UTILS_GL_VERTEX_BUFFER_H
#define UTILS_GL_VERTEX_BUFFER_H

#include <cassert>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "utils/gl/core.h"
#include "utils/gl/buffer.h"

namespace Utils::GL {

class VertexBuffer : public Buffer {
public:
    struct AttributePointer {
        const VertexBuffer *vbo;
        GLuint size;
        DataType type;
        GLboolean normalized;
        GLsizei stride;
        const void *pointer;
    };

public:
    VertexBuffer(GLsizeiptr size, const void *data, BufferUsage usage = GL_STATIC_DRAW);

    AttributePointer attr_ptr(
        GLuint size,
        DataType type,
        GLboolean normalized,
        GLsizei stride,
        const void *pointer = (void*)(0)) const noexcept {
        return AttributePointer{this, size, type, normalized, stride, pointer};
    }

    static GLint max_vertex_attributes() noexcept;
    static void bind_reset();
};

}

#endif // UTILS_GL_VERTEX_BUFFER_H
