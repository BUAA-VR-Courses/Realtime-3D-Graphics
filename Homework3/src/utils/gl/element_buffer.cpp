#include "element_buffer.h"

namespace Utils::GL {

ElementBuffer::ElementBuffer(BasicPrimitiveType primitive, size_t num, const GLuint *data, BufferUsage usage)
    : Buffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(num * point_num(primitive) * sizeof(GLuint)), data, usage),
      primitive(primitive), num_points(static_cast<GLuint>(num * point_num(primitive))) {}

void ElementBuffer::bind_reset() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}
