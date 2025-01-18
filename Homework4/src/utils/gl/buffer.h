#ifndef UTILS_GL_BUFFER_H
#define UTILS_GL_BUFFER_H

#include <cassert>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "utils/gl/core.h"

namespace Utils::GL {

class Buffer : public Obj {
public:
    Buffer(BufferType target, GLsizeiptr size, const void *data , BufferUsage usage = GL_STATIC_DRAW);
    Buffer(Buffer&& buf) noexcept;
    ~Buffer();

    Buffer& operator=(Buffer&& buf) noexcept;

    void clear();
    void bind() const;
    static void bind_reset(BufferType target);

    void sub_data(GLintptr offset, GLsizeiptr size, const void *data);

protected:
    BufferType target;
    BufferUsage usage;
};

}

#endif // UTILS_GL_BUFFER_H
