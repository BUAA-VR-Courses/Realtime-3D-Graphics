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
    return Transform::look_at(position, position + front, world_up);
}

matf4 Camera::get_projection_matrix(float width, float height, float near, float far) const {
    return Transform::perspective(to_radian(zoom), width / height, near, far);
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
