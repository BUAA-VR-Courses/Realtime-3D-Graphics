#ifndef UTILS_GL_CORE_H
#define UTILS_GL_CORE_H

#pragma once

#include <cassert>
#include <utility>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Utils::GL {

using BasicPrimitiveType = GLuint;  // GL_POINTS     0x0000
                                    // GL_LINES      0x0001
                                    // GL_TRIANGLES  0x0004
static constexpr size_t point_num(BasicPrimitiveType primitive) noexcept {
    switch (primitive) {
    case GL_POINTS:
        return 1;
    case GL_LINES:
        return 2;
    case GL_TRIANGLES:
        return 3;
    default:
        assert(false && "Invalid primitive type");
        return 0;
    }
}

using BufferType = GLuint; // GL_ARRAY_BUFFER          0x8892
                           // GL_ELEMENT_ARRAY_BUFFER  0x8893
using BufferUsage = GLuint; // GL_STREAM_DRAW   0x88E0
                            // GL_STREAM_READ   0x88E1
                            // GL_STREAM_COPY   0x88E2
                            // GL_STATIC_DRAW   0x88E4
                            // GL_STATIC_READ   0x88E5
                            // GL_STATIC_COPY   0x88E6
                            // GL_DYNAMIC_DRAW  0x88E8
                            // GL_DYNAMIC_READ  0x88E9
                            // GL_DYNAMIC_COPY  0x88EA

using DataType = GLuint; // GL_BYTE            0x1400
                         // GL_UNSIGNED_BYTE   0x1401
                         // GL_SHORT           0x1402
                         // GL_UNSIGNED_SHORT  0x1403
                         // GL_INT             0x1404
                         // GL_UNSIGNED_INT    0x1405
                         // GL_FLOAT           0x1406
                         // GL_HALF_FLOAT      0x140B
                         // GL_DOUBLE          0x140A

class ID {
public:
    ID(GLuint data = static_cast<GLuint>(0)) noexcept : data(data) {}
    ID(ID&& id) noexcept : data(id.data) {
        id.clear();
    }
    ID& operator=(ID&& id) noexcept {
        data = id.data;
        id.clear();
        return *this;
    }

    void clear() noexcept {
        data = static_cast<GLuint>(0);
    }

    operator GLuint() const noexcept {
        assert(is_valid());
        return data;
    }

    bool is_valid() const noexcept {
        return data != static_cast<GLuint>(0);
    }

    GLuint* init_ptr() noexcept {
        assert(!is_valid());
        return &data;
    }

    const GLuint* del_ptr() const noexcept {
        assert(is_valid());
        return &data;
    }

private:
    GLuint data;
};

class Obj {
public:
    Obj() = default;
    virtual ~Obj() {};
    Obj(ID&& id) noexcept : id(std::move(id)) {}
    Obj& operator=(Obj&& other) noexcept {
        id = std::move(other.id);
        return *this;
    }
    Obj(GLuint data) noexcept : id(data) {}

    virtual bool is_valid() const noexcept {
        return id.is_valid();
    }

protected:
    ID id { static_cast<GLuint>(0) };
};

} 

#endif // UTILS_GL_CORE_H
