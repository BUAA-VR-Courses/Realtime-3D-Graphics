#ifndef RASTERIZATION_H
#define RASTERIZATION_H

#pragma once
#include <vector>
#include <cstdlib>
#include <cstdint>
#include "utils.h"

void draw_line_dda(Pixel start, Pixel end, std::vector<Pixel>& pixels);
void draw_line_bresenham(Pixel start, Pixel end, std::vector<Pixel>& pixels);
void draw_ellipse(Pixel start, Pixel end, std::vector<Pixel>& pixels);

#endif // RASTERIZATION_H
