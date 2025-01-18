#ifndef UTILS_SHADER_H
#define UTILS_SHADER_H

#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Eigen/Dense"
#include "glad/glad.h"
#include "utils/tools.h"

#include "utils/gl/texture.h"

#define vert_type 0
#define frag_type 1
#define prog_type 2

namespace Utils {
class Shader {
public:
    Shader(const char *vert_shader_path, const char *frag_shader_path);
    ~Shader();
    
private:
    uint32_t id;
    static void check_compile_errors(uint32_t shader, int type);

public:
    void use_program() const;
    void active_texture(size_t idx, Utils::GL::Texture2D* tex);
    void delete_program() const;
    uint32_t get_id() const;
    
    void set_bool(const GLchar* name, GLboolean v) const;
    void set_int(const GLchar* name, GLint v) const;
    void set_uInt(const GLchar* name, GLint v) const;
    void set_float(const GLchar* name, GLfloat v) const;
    void set_vecf2(const GLchar* name, const vecf2& v) const;
    void set_vecf3(const GLchar* name, const vecf3& v) const;
    void set_vecf4(const GLchar* name, const vecf4& v) const;

    void set_ints(const GLchar* name, GLuint n, const GLint* data) const;
    void set_uInts(const GLchar* name, GLuint n, const GLuint* data) const;
    void set_floats(const GLchar* name, GLuint n, const GLfloat* data) const;
    void set_vecf2s(const GLchar* name, GLuint n, const GLfloat* data) const;
    void set_vecf3s(const GLchar* name, GLuint n, const GLfloat* data) const;
    void set_vecf4s(const GLchar* name, GLuint n, const GLfloat* data) const;

    void set_matf4(const GLchar* name, const matf4& mat) const;

    void set_tex(const GLchar *name, size_t v);
};
}

#endif // UTILS_SHADER_H
