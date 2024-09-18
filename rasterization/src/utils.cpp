#include "utils.h"

Vector2f::Vector2f(float x, float y) : x(x), y(y) {}

Pixel::Pixel() : x(0), y(0) {}

Pixel::Pixel(int x, int y) : x(x), y(y) {}

Vector2f Pixel::to_vertex(int width, int height) {
    return Vector2f((static_cast<float>(x) / static_cast<float>(width) * 2) - 1.0f, 
                    -(static_cast<float>(y) / static_cast<float>(height) * 2) + 1.0f);
}

Pixel Pixel::operator+(const Pixel& other) const {
    return Pixel(x + other.x, y + other.y);
}

Pixel Pixel::operator-(const Pixel& other) const {
    return Pixel(x - other.x, y - other.y);
}

bool Pixel::operator==(const Pixel& other) const {
    return x == other.x && y == other.y;
}

bool Pixel::operator!=(const Pixel& other) const {
    return x != other.x || y != other.y;
}
