#ifndef UTILS_TRANSFORM_H
#define UTILS_TRANSFORM_H

#include "Eigen/Dense"

#include "utils/tools.h"

namespace Utils::Transform {
    
matf4 perspective(float fov_y, float aspect, float z_near, float z_far) noexcept;

matf4 orthographic(float width, float height, float z_near, float z_far) noexcept;

matf4 look_at(const vecf3& pos, const vecf3& target, const vecf3& up) noexcept;

matf4 rotate_with(float theta, const vecf3& axis) noexcept;

matf4 generate_model_matrix(const vecf3& pos, const vecf3& scale, const matf4& rotate) noexcept;

}
#endif // UTILS_TRANSFORM_H