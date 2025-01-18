#include "buffer.h"

namespace Utils::GL {

Buffer::Buffer(BufferType target, GLsizeiptr size, const void *data, BufferUsage usage)
    : target(target), usage(usage) {
    glGenBuffers(1, id.init_ptr());
    bind();
    glBufferData(target, size, data, usage);
}

Buffer::Buffer(Buffer&& buf) noexcept
    : Obj(std::move(buf.id)), target(buf.target), usage(buf.usage) {}

Buffer::~Buffer() { clear(); }

Buffer& Buffer::operator=(Buffer&& buf) noexcept {
    clear();
    id = std::move(buf.id);
    target = buf.target;
    usage = buf.usage;
    return *this;
}

void Buffer::clear() {
    if (is_valid()) {
        glDeleteBuffers(1, id.del_ptr());
        id.clear();
    }
}

void Buffer::bind() const {
    assert(is_valid());
    glBindBuffer(target, id);
}

void Buffer::bind_reset(BufferType target) {
    glBindBuffer(target, 0);
}

void Buffer::sub_data(GLintptr offset, GLsizeiptr size, const void *data) {
    assert(data != nullptr);
    glBufferSubData(target, offset, size, data);
}

}