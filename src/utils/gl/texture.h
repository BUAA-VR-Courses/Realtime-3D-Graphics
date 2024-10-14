#ifndef UTILS_GL_TEXTURE_H
#define UTILS_GL_TEXTURE_H

#include "utils/gl/core.h"

namespace Utils::GL {

class Texture : public Obj {
public:
    Texture(TextureType type);
    ~Texture();
    Texture(Texture&& tex) noexcept;
    Texture& operator=(Texture&& tex) noexcept;

    void clear() noexcept;
    void bind() const;
    void bind_reset() const;

    void gen_mipmap();

    static size_t max_units();

    TextureType type;
protected:
    void set_image(TextureTarget target, GLint level, PixelDataInternalFormat internal_format, GLsizei width, GLsizei height, PixelDataFormat format, PixelDataType type, const void *pixels);
};

class FrameBuffer; // forward declaration

class Texture2D : public Texture {
public:
    Texture2D();

    void set_image(GLint level, PixelDataInternalFormat internal_format, GLsizei width, GLsizei height, PixelDataFormat format, PixelDataType type, const void *pixels);

    void set_wrap_filter(WrapMode s, WrapMode t, MinFilter min, MagFilter mag);

//    static Texture2D load(char const* path);

private:
    friend class FrameBuffer;
    const ID& get_id() const noexcept;
};
}

#endif // UTILS_GL_TEXTURE_H