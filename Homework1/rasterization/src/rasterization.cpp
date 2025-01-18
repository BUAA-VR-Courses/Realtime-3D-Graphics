#include "rasterization.h"

void draw_line_dda(Pixel start, Pixel end, std::vector<Pixel>& pixels) {
    int dx = end.x - start.x;
    int dy = end.y - start.y;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    float x_inc = dx / static_cast<float>(steps);
    float y_inc = dy / static_cast<float>(steps);
    float x = start.x;
    float y = start.y;

    for (int i = 0; i <= steps; i++) {
        pixels.emplace_back(x, y);
        x += x_inc;
        y += y_inc;
    }
}

void draw_line_bresenham(Pixel start, Pixel end, std::vector<Pixel>& pixels) {
    // TODO: 1.1 complete bresenham algorithm

}

void draw_ellipse(Pixel start, Pixel end, std::vector<Pixel>& pixels) {
    // TODO: 1.2 complete rasterization for ellipse

}
