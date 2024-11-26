#ifndef UTILS_GL_ELEMENT_BUFFER_H
#define UTILS_GL_ELEMENT_BUFFER_H

#include <iostream>
#include <cstdint>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "utils/gl/core.h"
#include "utils/gl/buffer.h"

namespace Utils::GL {

class ElementBuffer : public Buffer {
public:
    ElementBuffer(BasicPrimitiveType primitive, size_t num, const GLuint *data, BufferUsage usage = GL_STATIC_DRAW);

    BasicPrimitiveType primitive;
    GLuint num_points;

    static void bind_reset();
};

}

#endif // UTILS_GL_ELEMENT_BUFFER_H
