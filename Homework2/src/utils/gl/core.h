#ifndef UTILS_GL_CORE_H
#define UTILS_GL_CORE_H

#include <cassert>
#include <utility>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Utils::GL {

using BasicPrimitiveType = GLuint;  // GL_POINTS     0x0000
                                    // GL_LINES      0x0001
                                    // GL_TRIANGLES  0x0004

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

using FrameBufferType = GLuint; // GL_FRAMEBUFFER       0x8D40
                                // GL_READ_FRAMEBUFFER  0x8CA8
                                // GL_DRAW_FRAMEBUFFER  0x8CA9

using FrameBufferAttachment = GLuint; // GL_COLOR_ATTACHMENT0 0x8CE0
                                      // GL_COLOR_ATTACHMENT1 0x8CE1
                                      // GL_COLOR_ATTACHMENT2 0x8CE2
                                      // GL_COLOR_ATTACHMENT3 0x8CE3
                                      // GL_COLOR_ATTACHMENT4 0x8CE4
                                      // GL_COLOR_ATTACHMENT5 0x8CE5
                                      // GL_COLOR_ATTACHMENT6 0x8CE6
                                      // GL_COLOR_ATTACHMENT7 0x8CE7
                                      // GL_COLOR_ATTACHMENT8 0x8CE8
                                      // GL_COLOR_ATTACHMENT9 0x8CE9
                                      // GL_COLOR_ATTACHMENT10 0x8CEA
                                      // GL_COLOR_ATTACHMENT11 0x8CEB
                                      // GL_COLOR_ATTACHMENT12 0x8CEC
                                      // GL_COLOR_ATTACHMENT13 0x8CED
                                      // GL_COLOR_ATTACHMENT14 0x8CEE
                                      // GL_COLOR_ATTACHMENT15 0x8CEF
                                      // GL_COLOR_ATTACHMENT16 0x8CF0
                                      // GL_COLOR_ATTACHMENT17 0x8CF1
                                      // GL_COLOR_ATTACHMENT18 0x8CF2
                                      // GL_COLOR_ATTACHMENT19 0x8CF3
                                      // GL_COLOR_ATTACHMENT20 0x8CF4
                                      // GL_COLOR_ATTACHMENT21 0x8CF5
                                      // GL_COLOR_ATTACHMENT22 0x8CF6
                                      // GL_COLOR_ATTACHMENT23 0x8CF7
                                      // GL_COLOR_ATTACHMENT24 0x8CF8
                                      // GL_COLOR_ATTACHMENT25 0x8CF9
                                      // GL_COLOR_ATTACHMENT26 0x8CFA
                                      // GL_COLOR_ATTACHMENT27 0x8CFB
                                      // GL_COLOR_ATTACHMENT28 0x8CFC
                                      // GL_COLOR_ATTACHMENT29 0x8CFD
                                      // GL_COLOR_ATTACHMENT30 0x8CFE
                                      // GL_COLOR_ATTACHMENT31 0x8CFF
                                      // GL_DEPTH_ATTACHMENT 0x8D00
                                      // GL_STENCIL_ATTACHMENT 0x8D20
                                      // GL_DEPTH_STENCIL_ATTACHMENT 0x821A

using WrapMode = GLuint;  // GL_CLAMP_TO_EDGE  0x812F
                          // GL_CLAMP_TO_BORDER  0x812D
                          // GL_MIRRORED_REPEAT  0x8370
                          // GL_REPEAT  0x2901

using FilterMode = GLuint;  // GL_NEAREST  0x2600
                            // GL_LINEAR   0x2601

using MinFilter = GLuint; // GL_NEAREST  0x2600
                          // GL_LINEAR   0x2601
                          // GL_NEAREST_MIPMAP_NEAREST  0x2700
                          // GL_LINEAR_MIPMAP_NEAREST  0x2701
                          // GL_NEAREST_MIPMAP_LINEAR  0x2702
                          // GL_LINEAR_MIPMAP_LINEAR  0x2703

using MagFilter = GLuint; // GL_NEAREST  0x2600
                          // GL_LINEAR   0x2601

using TextureType = GLuint; // GL_TEXTURE_1D  0x0DE0
                            // GL_TEXTURE_2D  0x0DE1
                            // GL_TEXTURE_3D  0x806F
                            // GL_TEXTURE_CUBE_MAP  0x8513
                            // GL_TEXTURE_1D_ARRAY  0x8C18
                            // GL_TEXTURE_2D_ARRAY  0x8C1A
                            // GL_TEXTURE_CUBE_MAP_ARRAY  0x9009
                            // GL_TEXTURE_RECTANGLE  0x84F5
                            // GL_TEXTURE_BUFFER  0x8C2A
                            // GL_TEXTURE_2D_MULTISAMPLE  0x9100
                            // GL_TEXTURE_2D_MULTISAMPLE_ARRAY  0x9102

using TextureTarget = GLuint; // GL_TEXTURE_BINDING_1D  0x8068
                              // GL_TEXTURE_BINDING_2D  0x8069
                              // GL_TEXTURE_BINDING_3D  0x806A
                              // GL_TEXTURE_BINDING_CUBE_MAP  0x8514
                              // GL_TEXTURE_BINDING_1D_ARRAY  0x8C1C
                              // GL_TEXTURE_BINDING_2D_ARRAY  0x8C1D
                              // GL_TEXTURE_BINDING_CUBE_MAP_ARRAY  0x900A
                              // GL_TEXTURE_BINDING_RECTANGLE  0x84F6
                              // GL_TEXTURE_BINDING_BUFFER  0x8C2C
                              // GL_TEXTURE_BINDING_2D_MULTISAMPLE  0x9104
                              // GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY  0x9105

using PixelDataInternalFormat = GLuint; // GL_R8  0x8229
                                        // GL_R8_SNORM  0x8F94
                                        // GL_R16  0x822A
                                        // GL_R16_SNORM  0x8F98
                                        // GL_RG8  0x822B
                                        // GL_RG8_SNORM  0x8F95
                                        // GL_RG16  0x822C
                                        // GL_RG16_SNORM  0x8F99
                                        // GL_R3_G3_B2  0x2A10
                                        // GL_RGB4  0x804F
                                        // GL_RGB5  0x8050
                                        // GL_RGB8  0x8051
                                        // GL_RGB8_SNORM  0x8F96
                                        // GL_RGB10  0x8052
                                        // GL_RGB12  0x8053
                                        // GL_RGB16_SNORM  0x8F9A
                                        // GL_RGBA2  0x8055
                                        // GL_RGBA4  0x8056
                                        // GL_RGB5_A1  0x8057
                                        // GL_RGBA8  0x8058
                                        // GL_RGBA8_SNORM  0x8F97
                                        // GL_RGB10_A2  0x8059
                                        // GL_RGB10_A2UI  0x906F
                                        // GL_RGBA12  0x805A
                                        // GL_RGBA16  0x805B
                                        // GL_SRGB8  0x8C41
                                        // GL_SRGB8_ALPHA8  0x8C43
                                        // GL_R16F  0x822D
                                        // GL_RG16F  0x822F
                                        // GL_RGB16F  0x881B
                                        // GL_RGBA16F  0x881A
                                        // GL_R32F  0x822E
                                        // GL_RG32F  0x8230
                                        // GL_RGB32F  0x8815
                                        // GL_RGBA32F  0x8814
                                        // GL_R11F_G11F_B10F  0x8C3A
                                        // GL_RGB9_E5  0x8C
                                        // GL_DEPTH_COMPONENT 0x1902
                                        // GL_DEPTH_STENCIL 0x84F9

using PixelDataFormat = GLuint; // GL_RED  0x1903
                                // GL_RG  0x8227
                                // GL_RGB  0x1907
                                // GL_BGR  0x80E0
                                // GL_RGBA  0x1908
                                // GL_BGRA  0x80E1
                                // GL_RED_INTEGER  0x8D94
                                // GL_RG_INTEGER  0x8228
                                // GL_RGB_INTEGER  0x8D98
                                // GL_BGR_INTEGER  0x8D9A
                                // GL_RGBA_INTEGER  0x8D99
                                // GL_BGRA_INTEGER  0x8D9B
                                // GL_STENCIL_INDEX  0x1901
                                // GL_DEPTH_COMPONENT  0x1902
                                // GL_DEPTH_STENCIL  0x84F9

using PixelDataType = GLuint; // GL_UNSIGNED_BYTE  0x1401
                              // GL_BYTE  0x1400
                              // GL_UNSIGNED_SHORT  0x1403
                              // GL_SHORT  0x1402
                              // GL_UNSIGNED_INT  0x1405
                              // GL_INT  0x1404
                              // GL_FLOAT  0x1406
                              // GL_HALF_FLOAT  0x140B
                              // GL_UNSIGNED_BYTE_3_3_2  0x8032
                              // GL_UNSIGNED_BYTE_2_3_3_REV  0x8362
                              // GL_UNSIGNED_SHORT_5_6_5  0x8363
                              // GL_UNSIGNED_SHORT_5_6_5_REV  0x8364
                              // GL_UNSIGNED_SHORT_4_4_4_4  0x8033
                              // GL_UNSIGNED_SHORT_4_4_4_4_REV  0x8365
                              // GL_UNSIGNED_SHORT_5_5_5_1  0x8034
                              // GL_UNSIGNED_SHORT_1_5_5_5_REV  0x8366
                              // GL_UNSIGNED_INT_8_8_8_8  0x8035
                              // GL_UNSIGNED_INT_8_8_8_8_REV  0x8367
                              // GL_UNSIGNED_INT_10_10_10_2  0x8036
                              // ...


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
