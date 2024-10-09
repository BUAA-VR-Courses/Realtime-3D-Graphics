#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string_view>
#include <utility>
#include <vector>

#include "rasterization.h"
#include "utils.h"

std::string testcases_dir = TESTCASES_DIR;
std::string line_test_names[] = {
    "line0",
    "line1",
    "line2",
    "line3",
    "line4",
    "line5",
    "line6",
    "line7",
    "line8",
    "line9",
};
std::string ellipse_test_names[] = {
    "ellipse0",
    "ellipse1",
    "ellipse2",
    "ellipse3",
};

struct Testcase {
    Pixel start;
    Pixel end;
    std::vector<Pixel> expected;
    std::vector<Pixel> got;

    Testcase(const std::string& name) noexcept {
        std::ifstream file(testcases_dir + name, std::ios::in | std::ios::ate);
        if (!file.is_open()) {
            return;
        }
        auto size = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);
        char *buffer = new char[size + 1];
        buffer[size] = 0;
        file.read(buffer, static_cast<int64_t>(size));
        file.close();

        std::vector<std::string> lines;
        for (size_t i = 0; i <= size; ) {
            auto j = 0;
            while ((buffer[i + j] != '\n') && (buffer[i + j] != '\0')) {
                j++;
            }
            lines.emplace_back(buffer + i, j);
            i += j + 1;
        }
        delete[] buffer;
        buffer = nullptr;

        std::stringstream ss(lines[0]);
        ss >> start.x >> start.y;
        ss = std::stringstream(lines[1]);
        ss >> end.x >> end.y;
        for (size_t i = 2; i < lines.size(); ++i) {
            std::stringstream ss(lines[i]);
            Pixel p;
            ss >> p.x >> p.y;
            expected.push_back(p);
        }
    }

    virtual bool test() noexcept = 0;
};

struct LineTestcase : public Testcase {
    LineTestcase(const std::string& name) noexcept : Testcase(name) {}

    bool test() noexcept {
        int err_cnt = 0;
        draw_line_bresenham(start, end, got);
        if (abs(start.x - end.x) > abs(start.y - end.y)) {
            for (auto& e : expected) {
                auto g_it = std::find_if(got.begin(), got.end(), [&e](const Pixel& p) {
                    return p.x == e.x;
                }); 
                if (g_it == got.end()) {
                    err_cnt++;
                    continue;
                }
                auto& g = *g_it;
                if (g.y != e.y) {
                    err_cnt++;
                }
            }
        } else {
            for (auto& e : expected) {
                auto g_it = std::find_if(got.begin(), got.end(), [&e](const Pixel& p) {
                    return p.y == e.y;
                });
                if (g_it == got.end()) {
                    err_cnt++;
                    continue;
                } 
                auto& g = *g_it;
                if (g.x != e.x) {
                    err_cnt++;
                }
            }
        }
        return err_cnt <= 2;
    }
};

struct EllipseTestcase : public Testcase {
    EllipseTestcase(const std::string& name) noexcept : Testcase(name) {}

    bool test() noexcept {
        int err_cnt = 0;
        draw_ellipse(start, end, got);
        float center_x = (start.x + end.x) / 2.0f;
        float center_y = (start.y + end.y) / 2.0f;
        float a = std::fabs(double(start.x - end.x)) / 2.0f;
        float b = std::fabs(double(start.y - end.y)) / 2.0f;
        for (auto& p : got) {
            float x = p.x - center_x;
            float y = p.y - center_y;
            float t = std::atan2(a * y, b * x);
            float dx = x - a * std::cos(t);
            float dy = y - b * std::sin(t);
            if (dx * dx + dy * dy > 2) {
                err_cnt++;
            }
        }
        return err_cnt <= 2;
    }
};

int main() {
    // lines
    for (auto& name : line_test_names) {
        LineTestcase testcase(name);
        if (testcase.test()) {
            std::cout << "Test " << name << " passed." << std::endl;
        } else {
            std::cout << "Test " << name << " failed: " << std::endl;
            std::cout << "\tExpected: " << std::endl;
            for (auto& pixel : testcase.expected) {
                std::cout << "\t\t(" << pixel.x << ", " << pixel.y << ")" << std::endl;
            }
            std::cout << "Got: " << std::endl;
            for (auto& pixel : testcase.got) {
                std::cout << "\t\t(" << pixel.x << ", " << pixel.y << ")" << std::endl;
            }
        }
    }

    // ellipses
    for (auto& name : ellipse_test_names) {
        EllipseTestcase testcase(name);
        if (testcase.test()) {
            std::cout << "Test " << name << " passed." << std::endl;
        } else {
            std::cout << "Test " << name << " failed: " << std::endl;
            std::cout << "\tStart: (" << testcase.start.x << ", " << testcase.start.y << ")" << std::endl;
            std::cout << "\tEnd: (" << testcase.end.x << ", " << testcase.end.y << ")" << std::endl;
        }
    }

    return 0;
}