#include "camera.h"

namespace Utils {

Camera::Camera(const vecf3& position, const vecf3& up, float yaw, float pitch)
    : front(vecf3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM),
      position(position), world_up(up), yaw(yaw), pitch(pitch) {
    update_camera_vectors();
}

Camera::Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch) 
    : front(vecf3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM),
      position(vecf3(pos_x, pos_y, pos_z)), world_up(vecf3(up_x, up_y, up_z)), yaw(yaw), pitch(pitch) {
    update_camera_vectors();
}

matf4 Camera::get_view_matrix() const {
    auto look_at = [](const vecf3& pos, const vecf3& target, const vecf3& up) {
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
    };

    return look_at(position, position + front, world_up);
}

matf4 Camera::get_projection_matrix(float width, float height, float near, float far) const {
    auto perspective = [](float fov_y, float aspect, float z_near, float z_far) {
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
    };

    return perspective(to_radian(zoom), width / height, near, far);
}

void Camera::process_keyboard(Movement direction, float delta_time) {
    float velocity = movement_speed * delta_time;
    switch (direction) {
    case Movement::FORWARD:
        position += front * velocity;
        break;
    case Movement::BACKWARD:
        position -= front * velocity;
        break;
    case Movement::LEFT:
        position -= right * velocity;
        break;
    case Movement::RIGHT:
        position += right * velocity;
        break;
    case Movement::UP:
        position += up * velocity;
        break;
    case Movement::DOWN:
        position -= up * velocity;
        break;
    default:
        break;
    }
}

void Camera::process_mouse_movement(float xoffset, float yoffset, bool constrain_pitch) {
    xoffset *= mouse_sensitivity;
    yoffset *= mouse_sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrain_pitch) {
        if (pitch > 89.0f) {
            pitch = 89.0f;
        }
        if (pitch < -89.0f) {
            pitch = -89.0f;
        }
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    update_camera_vectors();
}

void Camera::process_mouse_scroll(float yoffset) {
    if (zoom >= 1.0f && zoom <= 45.0f) {
        zoom -= yoffset;
    }
    if (zoom <= 1.0f) {
        zoom = 1.0f;
    }
    if (zoom >= 45.0f) {
        zoom = 45.0f;
    }
}

void Camera::update_camera_vectors() {
    // Calculate the new Front vector
    vecf3 _front;
    _front[0] = cos(to_radian(yaw)) * cos(to_radian(pitch));
    _front[1] = sin(to_radian(pitch));
    _front[2] = sin(to_radian(yaw)) * cos(to_radian(pitch));
    front = _front.normalized();

    // Also re-calculate the Right and Up vector
    right = front.cross(world_up).normalized(); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up = right.cross(front).normalized();
}

}
