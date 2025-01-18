#ifndef UTILS_IMAGE_H
#define UTILS_IMAGE_H

#include <cassert>
#include <cstdint>

#include "utils/tools.h"

namespace Utils {

class Image {
public:
    const int bytes_per_pixel = 3;
    float* fdata = nullptr;
    uint8_t* bdata = nullptr;
    int width = 0;
    int height = 0;

    char* path = nullptr;

    int bytes_per_scanline = 0;

    Image();
    Image(const char* _path);
    Image(int _width, int _height);
    ~Image();

    bool load();

    bool save() const;

    vecf3 sample(double u, double v) const {
        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0);

        auto i = int(u * width);
        auto j = int(v * height);
        auto pixel = get_pixel(i, j);

		static const double inv255 = 1.0 / 255.0;
        return vecf3(pixel[0], pixel[1], pixel[2]) * inv255;
    }

    const uint8_t* get_pixel(int x, int y) const {
        // Return the address of the three RGB bytes of the pixel at x,y. If there is no image
        // data, returns magenta.
        static uint8_t magenta[] = { 255, 0, 255 };
        if (bdata == nullptr) return magenta;

        x = clamp(x, 0, width);
        y = clamp(y, 0, height);

        return bdata + y * bytes_per_scanline + x * bytes_per_pixel;
    }

    void set_pixel(int x, int y, const vecf3& color) {
        // Set the pixel at x,y to the given color.
        if (bdata == nullptr) return;

        x = clamp(x, 0, width);
        y = clamp(y, 0, height);

        auto* pixel = bdata + y * bytes_per_scanline + x * bytes_per_pixel;
        pixel[0] = float_to_byte(linear_to_gamma(color[0]));
        pixel[1] = float_to_byte(linear_to_gamma(color[1]));
        pixel[2] = float_to_byte(linear_to_gamma(color[2]));
    }

private:
    static int clamp(int x, int low, int high) {
        // Return the value clamped to the range [low, high).
        if (x < low) return low;
        if (x < high) return x;
        return high - 1;
    }

    static double clamp(double x, double low, double high) {
        // Return the value clamped to the range [low, high).
        if (x < low) return low;
        if (x < high) return x;
        return high - 1;
    }

    static uint8_t float_to_byte(float value) {
        if (value <= 0.0) {
            return 0;
        }
        if (1.0 <= value) {
            return 255;
        }
        return static_cast<uint8_t>(256.0 * value);
    }

    void convert_to_bytes() {
        // Convert the linear floating point pixel data to bytes, storing the resulting byte
        // data in the `bdata` member.
        int total_bytes = width * height * bytes_per_pixel;
        bdata = new uint8_t[total_bytes];

        // Iterate through all pixel components, converting from [0.0, 1.0] float values to
        // unsigned [0, 255] byte values.
        auto* bptr = bdata;
        auto* fptr = fdata;
        for (auto i = 0; i < total_bytes; i++, fptr++, bptr++) {
            *bptr = float_to_byte(*fptr);
        }
    }
};

}


#endif // !UTILS_IMAGE_H
