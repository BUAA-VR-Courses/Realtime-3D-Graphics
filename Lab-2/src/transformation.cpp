#include "transformation.h"

Eigen::Matrix4f get_scaling_matrix(Eigen::Vector3f scaling) {
    Eigen::Matrix4f matrix = Eigen::Matrix4f::Identity();

    // TODO: 2.1 Create the scaling matrix for scaling the cube.
                  
    return matrix;
}

Eigen::Matrix4f get_rotation_matrix(double rotation_angle) {
    Eigen::Matrix4f matrix = Eigen::Matrix4f::Identity();

    // TODO: 2.1 Create the rotation matrix for rotating the cube around the Z axis.
    // Note that the parameter 'rotation_angle' is expressed in the angle system.
    
    return matrix;
}
// TODO: bonus Arbitrary rotation transformation
// And design a way to check whether the transformation is correct or not

Eigen::Matrix4f get_translation_matrix(Eigen::Vector3f translation) {
    Eigen::Matrix4f matrix = Eigen::Matrix4f::Identity();

    // TODO: 2.1 Create the translation matrix for translating the cube.

    return matrix;
}


Eigen::Matrix4f get_view_matrix(Eigen::Vector3f from_position) {
    Eigen::Matrix4f matrix = Eigen::Matrix4f::Identity();

    // TODO: 2.2 Create the lookat view matrix for the camera.
    // The camera is looking at (0,0,0) from given position, and up should always be up.
    
    // this is just a special sample and you should write your own code
    matrix << 1,  0, 0,   0,
              0, -1, 0,   0,
              0,  0, 1, 1.5,
              0,  0, 0,   1;

    return matrix;
}

Eigen::Matrix4f get_projection_matrix(float fov, float aspect, float near, float far) {
    Eigen::Matrix4f matrix = Eigen::Matrix4f::Identity();

    // TODO: 2.3 Create the perspective projection matrix.

    // this is just a special sample and you should write your own code
    matrix << 0.43301,       0,        0,       0,
                    0, 0.57735,        0,       0,
                    0,       0, -1.0202f, 0.20202,
                    0,       0,        1,       0;        

    return matrix;
}