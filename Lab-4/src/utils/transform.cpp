#include "transform.h"

namespace Utils::Transform {

matf4 perspective(float fov_y, float aspect, float z_near, float z_far) noexcept {
    assert(fov_y > 0 && aspect > 0 && z_near >= 0 && z_far > z_near);

    auto tan_half_fov_y = tan(fov_y / 2.0f);
    auto cot_half_fov_y = 1 / tan_half_fov_y;

    auto m00 = cot_half_fov_y / aspect;
    auto m11 = cot_half_fov_y;
    auto m22 = -(z_far + z_near) / (z_far - z_near);
    auto m23 = -(2 * z_far * z_near) / (z_far - z_near);

    matf4 m;
    m <<  m00,      0,      0,      0,
            0,    m11,      0,      0,
            0,      0,    m22,    m23,
            0,      0,     -1,      0;
    
    return m;
}

matf4 orthographic(float width, float height, float z_near, float z_far) noexcept {
    assert(width > 0 && height > 0 && z_near >= 0 && z_far > z_near);

    auto m00 = 2 / width;
    auto m11 = 2 / height;
    auto m22 = -2 / (z_far - z_near);
    auto m23 = -(z_far + z_near) / (z_far - z_near);

    matf4 m;
    m <<  m00,      0,      0,      0,
            0,    m11,      0,      0,
            0,      0,    m22,    m23,
            0,      0,      0,      1;

    return m;
}

matf4 look_at(const vecf3& pos, const vecf3& target, const vecf3& up) noexcept {
    assert(abs(up.dot(up) - 1.0f) < 1e-6);

    const vecf3 front = (target - pos).normalized();
    assert(front != up);
    vecf3 right = front.cross(up).normalized();
    const vecf3 cam_up = right.cross(front);

    matf4 m;
    m <<  right[0],  right[1],  right[2],  -right.dot(pos),
         cam_up[0], cam_up[1], cam_up[2], -cam_up.dot(pos),
         -front[0], -front[1], -front[2],   front.dot(pos),
                 0,         0,         0,                1;

    return m;
}

matf4 rotate_with(float theta, const vecf3& axis) noexcept {
    assert(abs(axis.dot(axis) - 1.0f) < 1e-6);

    auto cos_theta = cos(theta);
    auto sin_theta = sin(theta);
    auto one_minus_cos_theta = 1 - cos_theta;

    matf4 m;
    m << cos_theta + axis[0] * axis[0] * one_minus_cos_theta, axis[0] * axis[1] * one_minus_cos_theta - axis[2] * sin_theta, axis[0] * axis[2] * one_minus_cos_theta + axis[1] * sin_theta, 0,
         axis[1] * axis[0] * one_minus_cos_theta + axis[2] * sin_theta, cos_theta + axis[1] * axis[1] * one_minus_cos_theta, axis[1] * axis[2] * one_minus_cos_theta - axis[0] * sin_theta, 0,
         axis[2] * axis[0] * one_minus_cos_theta - axis[1] * sin_theta, axis[2] * axis[1] * one_minus_cos_theta + axis[0] * sin_theta, cos_theta + axis[2] * axis[2] * one_minus_cos_theta, 0,
         0, 0, 0, 1;

    return m;
}

matf4 generate_model_matrix(const vecf3& pos, const vecf3& scale, const matf4& rotate) noexcept {
    matf4 m;
    m <<  scale[0],         0,         0,  pos[0],
                 0,  scale[1],         0,  pos[1],
                 0,         0,  scale[2],  pos[2],
                 0,         0,         0,       1;
    return m * rotate;
}

//matf4 generate_model_matrix(const vecf3& pos, const vecf4& quat) noexcept {
//    assert(abs(quat.dot(quat) - 1.0f) < 1e-6);
//
//    auto x = quat[0];
//    auto y = quat[1];
//    auto z = quat[2];
//    auto w = quat[3];
//
//    auto xx = x * x;
//    auto xy = x * y;
//    auto xz = x * z;
//    auto xw = x * w;
//    auto yy = y * y;
//    auto yz = y * z;
//    auto yw = y * w;
//    auto zz = z * z;
//    auto zw = z * w;
//
//    matf4 m;
//    m << 1 - 2 * (yy + zz),     2 * (xy - zw),     2 * (xz + yw),  pos[0],
//             2 * (xy + zw), 1 - 2 * (xx + zz),     2 * (yz - xw),  pos[1],
//             2 * (xz - yw),     2 * (yz + xw), 1 - 2 * (xx + yy),  pos[2],
//                         0,                 0,                 0,       1;
//
//    return m;
//}

} // namespace Utils::Transform