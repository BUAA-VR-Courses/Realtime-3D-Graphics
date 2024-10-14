#ifndef UTILS_CAMERA_H
#define UTILS_CAMERA_H

#include <cassert>

#include "Eigen/Dense"
#include "utils/tools.h"
#include "utils/transform.h"

namespace Utils {

class Camera {
public:
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum class Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN,
    };

    // Default camera values
    static constexpr float YAW = -90.0f;
    static constexpr float PITCH = 0.0f;
    static constexpr float SPEED = 2.5f;
    static constexpr float SENSITIVITY = 0.1f;
    static constexpr float ZOOM = 45.0f;
    
    // Camera Attributes
    vecf3 position;
    vecf3 front;
    vecf3 up;
    vecf3 right;
    vecf3 world_up;
    // Euler Angles
    float yaw;
    float pitch;
    // Camera options
    float movement_speed;
    float mouse_sensitivity;
    float zoom;

    // Constructor with vectors
    Camera(const vecf3& position = vecf3(0.0f, 0.0f, 0.0f), const vecf3& up = vecf3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    // Constructor with scalar values
    Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    matf4 get_view_matrix() const;
    // Returns the projection matrix
    matf4 get_projection_matrix(float width, float height, float near, float far) const;

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void process_keyboard(Movement direction, float delta_time);

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void process_mouse_movement(float xoffset, float yoffset, bool constrain_pitch = true);

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void process_mouse_scroll(float yoffset);

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void update_camera_vectors();
};
}

#endif // UTILS_CAMERA_H
