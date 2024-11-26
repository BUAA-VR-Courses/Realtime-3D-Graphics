#include "utils/image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

namespace Utils {

Image::Image() = default;

Image::Image(const char* _path) {
    path = const_cast<char*>(_path);
    auto suc = load();
    assert(suc);
}

Image::Image(int _width, int _height) : width(_width), height(_height) {
    bytes_per_scanline = width * bytes_per_pixel;
    bdata = new uint8_t[width * height * bytes_per_pixel];
}

Image::~Image() {
    delete[] bdata;
    STBI_FREE(fdata);
}

bool Image::load() {
    auto n = bytes_per_pixel; // Dummy out parameter: original components per pixel
    fdata = stbi_loadf(path, &width, &height, &n, bytes_per_pixel);
    if (fdata == nullptr) return false;

    bytes_per_scanline = width * bytes_per_pixel;
    convert_to_bytes();
    return true;
}

bool Image::save() const {
    assert(path);
    return stbi_write_png(path, width, height, bytes_per_pixel, bdata, bytes_per_scanline);
}

}