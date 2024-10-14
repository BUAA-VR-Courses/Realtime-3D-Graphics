#include "texture.h"

namespace Utils::GL {

Texture::Texture(TextureType type) : type(type) {
    glGenTextures(1, id.init_ptr());
}

Texture::~Texture() {
    clear();
}

Texture::Texture(Texture &&tex) noexcept: Obj(std::move(tex.id)), type(tex.type) {}

Texture &Texture::operator=(Texture &&tex) noexcept {
    clear();
    id = std::move(tex.id);
    type = tex.type;
    return *this;
}

void Texture::clear() noexcept {
    if (is_valid()) {
        glDeleteTextures(1, id.del_ptr());
        id.clear();
    }
}

void Texture::bind() const {
    glBindTexture(type, id);
}

void Texture::bind_reset() const {
    glBindTexture(type, 0);
}

void Texture::gen_mipmap() {
    glGenerateMipmap(type);
}

size_t Texture::max_units() {
    static GLint rst;
    static bool init = (glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &rst), true);
    return static_cast<size_t>(rst);
}

void Texture::set_image(TextureTarget target, GLint level, PixelDataInternalFormat internal_format, GLsizei width, GLsizei height, PixelDataFormat format, PixelDataType type, const void *pixels) {
    bind();
    glTexImage2D(target, level, internal_format, width, height, 0, format, type, pixels);
    bind_reset();
}

Texture2D::Texture2D() : Texture(GL_TEXTURE_2D) {}

const ID &Texture2D::get_id() const noexcept {
    return id;
}

void Texture2D::set_image(GLint level, PixelDataInternalFormat internal_format, GLsizei width, GLsizei height, PixelDataFormat format, PixelDataType type, const void *pixels) {
    Texture::set_image(GL_TEXTURE_2D, level, internal_format, width, height, format, type, pixels);
}

void Texture2D::set_wrap_filter(WrapMode s, WrapMode t, MinFilter min, MagFilter mag) {
    bind();
    glTexParameteri(type, GL_TEXTURE_WRAP_S, static_cast<GLint>(s));
    glTexParameteri(type, GL_TEXTURE_WRAP_T, static_cast<GLint>(t));
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(min));
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(mag));
    bind_reset();
}

//Texture2D Texture2D::load(char const *path) {
//    Texture2D tex;
//    tex.set_wrap_filter(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR);
//    int width, height, nr_channels;
//    stbi_set_flip_vertically_on_load(true);
//
//}

}