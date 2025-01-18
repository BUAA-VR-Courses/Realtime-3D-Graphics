#pragma once

#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "glad/glad.h"

#define vert_type 0
#define frag_type 1
#define prog_type 2

class Shader {
public:
    Shader(const char *vert_shader_path, const char *frag_shader_path);
    ~Shader();
    void use_program();
    void delete_program();
private:
    uint32_t id;
    void check_compile_errors(uint32_t shader, int type);
};
