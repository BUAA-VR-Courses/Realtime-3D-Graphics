#ifndef UTILS_GL_VERTEX_ARRAY_H
#define UTILS_GL_VERTEX_ARRAY_H

#include <vector>
#include <cassert>
#include <utility>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "utils/shader.h"
#include "utils/gl/core.h"
#include "utils/gl/vertex_buffer.h"
#include "utils/gl/element_buffer.h"

namespace Utils::GL {

class VertexArray : public Obj {
public:
    struct Format {
        std::vector<VertexBuffer::AttributePointer> attr_ptrs;
        ElementBuffer *eb;
    };

public:
    VertexArray();
    ~VertexArray();

    VertexArray(std::vector<GLuint>&& indices, Format&& format) noexcept;
    VertexArray(const std::vector<GLuint>& indices, const Format& format) noexcept;
    VertexArray(VertexArray&& va) noexcept;
    VertexArray& operator=(VertexArray&& va) noexcept;

    void attach(GLuint idx, const VertexBuffer::AttributePointer& attr_ptr) const;
    void attach(const ElementBuffer *eb);
    void attach(const std::vector<GLuint>& indices, const Format& format);

    void draw(const Shader& shader) const;

    bool is_valid() const noexcept;

    void clear();

    void bind() const;
    static void bind_reset();

private:
    const ElementBuffer *eb = nullptr;
};

}

#endif // UTILS_GL_VERTEX_ARRAY_H
