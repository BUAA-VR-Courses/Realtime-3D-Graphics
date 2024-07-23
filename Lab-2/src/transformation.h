#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#pragma once
#include "Eigen/Dense"

constexpr double PI = 3.1415926535;

Eigen::Matrix4f get_scaling_matrix(Eigen::Vector3f scaling);

Eigen::Matrix4f get_rotation_matrix(double rotation_angle);

Eigen::Matrix4f get_translation_matrix(Eigen::Vector3f translation);

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f from_position);

Eigen::Matrix4f get_projection_matrix(float fov, float aspect, float near, float far);

#endif // TRANSFORMATION_H
