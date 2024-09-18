#pragma once

struct Vector2f {
    float x, y;
    Vector2f(float x, float y);
};

struct Pixel {
    int x, y;
    Pixel();
    Pixel(int x, int y);
    Vector2f to_vertex(int width, int height);
    Pixel operator+(const Pixel& other) const;
    Pixel operator-(const Pixel& other) const;
    bool operator==(const Pixel& other) const;
    bool operator!=(const Pixel& other) const;
};
