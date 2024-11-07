#include "transform.h"

namespace Utils::Transform {

matf4 perspective(float fov_y, float aspect, float z_near, float z_far) noexcept {
    assert(fov_y > 0 && aspect > 0 && z_near >= 0 && z_far > z_near);

	// TODO 2.1.2 : Create the perspective projection matrix.
	matf4 m = matf4::Identity();
    
    return m;
}

matf4 orthographic(float width, float height, float z_near, float z_far) noexcept {
    assert(width > 0 && height > 0 && z_near >= 0 && z_far > z_near);

	// TODO 2.1.2 : Create the orthographic projection matrix.
	matf4 m = matf4::Identity();

    return m;
}

matf4 look_at(const vecf3& pos, const vecf3& target, const vecf3& up) noexcept {
    assert(abs(up.dot(up) - 1.0f) < 1e-6);

    // TODO 2.1.2 : Create the lookat view matrix for the camera.
	matf4 m = matf4::Identity();

    return m;
}

matf4 get_scale_matrix(const vecf3& scale) noexcept {
    // TODO 2.1.1 : Implement the function that returns a scale matrix.
    matf4 m = matf4::Identity();

    return m;
}

matf4 get_trans_matrix(const vecf3& trans) noexcept {
    // TODO 2.1.1 : Implement the function that returns a translation matrix.
    matf4 m = matf4::Identity();

    return m;
}

matf4 rotate_with(float theta, const vecf3& axis) noexcept {
    assert(abs(axis.dot(axis) - 1.0f) < 1e-6);

	// TODO 2.1.1 : Create the rotation matrix with the given theta and axis.
    // Note that the parameter 'theta' is expressed in the radian system.
	matf4 m = matf4::Identity();

    return m;
}

matf4 generate_model_matrix(const vecf3& pos, const vecf3& scale, float theta, const vecf3& axis) noexcept {
	// TODO 2.1.1 : Implement the function that returns a model matrix.
	matf4 m = matf4::Identity();

	return m;
}

matf4 generate_model_matrix(const vecf3& pos, const vecf3& scale, const matf4& rotate) noexcept {
    matf4 m = get_trans_matrix(pos) * rotate * get_scale_matrix(scale);

    return m;
}

} // namespace Utils::Transform