#include "vertex_array.h"

namespace Utils::GL {

VertexArray::VertexArray() {
    glGenVertexArrays(1, id.init_ptr());
}

VertexArray::~VertexArray() {
    clear();
}

void VertexArray::bind() const {
    glBindVertexArray(id);
}

void VertexArray::bind_reset() {
    glBindVertexArray(0);
}

VertexArray::VertexArray(const std::vector<GLuint>& indices, const Format& format) noexcept
    : VertexArray() {
    attach(indices, format);
}

VertexArray::VertexArray(std::vector<GLuint>&& indices, VertexArray::Format&& format) noexcept
    : VertexArray() {
    attach(indices, format);
}

VertexArray::VertexArray(VertexArray&& va) noexcept : Obj(std::move(va.id)), eb(va.eb) {}

VertexArray& VertexArray::operator=(VertexArray&& va) noexcept {
    clear();
    id = std::move(va.id);
    eb = va.eb;
    return *this;
}

void VertexArray::clear() {
    glDeleteVertexArrays(1, id.del_ptr());
    id.clear();
}

bool VertexArray::is_valid() const noexcept {
    return id.is_valid() && eb != nullptr;
}

void VertexArray::attach(GLuint idx, const VertexBuffer::AttributePointer& attr_ptr) const {
    bind();
    attr_ptr.vbo->bind();
    glVertexAttribPointer(idx, attr_ptr.size, attr_ptr.type, attr_ptr.normalized, attr_ptr.stride, attr_ptr.pointer);
    glEnableVertexAttribArray(idx);
    bind_reset();
    attr_ptr.vbo->bind_reset();
}

void VertexArray::attach(const std::vector<GLuint>& indices, const Format& format) {
    eb = format.eb;

    assert(indices.size() == format.attr_ptrs.size());
    assert(format.eb != nullptr);
    bind();
    for (auto i = 0; i < indices.size(); i++) {
        const auto& attr_ptr = format.attr_ptrs[i];
        attr_ptr.vbo->bind();
        glVertexAttribPointer(indices[i], attr_ptr.size, attr_ptr.type, attr_ptr.normalized, attr_ptr.stride, attr_ptr.pointer);
        glEnableVertexAttribArray(indices[i]);
    }
    format.eb->bind();
    bind_reset();
    VertexBuffer::bind_reset();
    ElementBuffer::bind_reset();
}

void VertexArray::attach(const ElementBuffer *eb) {
    this->eb = eb;

    bind();
    eb->bind();
    bind_reset();
    ElementBuffer::bind_reset();
}

void VertexArray::draw(const Shader& shader) const {
    assert(is_valid());
    shader.use_program();
    bind();
    glDrawElements(eb->primitive, eb->num_points, GL_UNSIGNED_INT, nullptr);
    bind_reset();
}

}
