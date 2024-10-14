#ifndef UTILS_TOOLS_H
#define UTILS_TOOLS_H


#include <iostream>
#include <vector>

#include "Eigen/Dense"

using veci2 = Eigen::Vector2i;
using veci3 = Eigen::Vector3i;
using veci4 = Eigen::Vector4i;
using vecf2 = Eigen::Vector2f;
using vecf3 = Eigen::Vector3f;
using vecf4 = Eigen::Vector4f;
using matf3 = Eigen::Matrix3f;
using matf4 = Eigen::Matrix4f;

#define to_radian(degree) (degree * 3.1415926535f / 180.0f)

namespace Utils {

std::vector<vecf3> generate_normals(const std::vector<vecf3>& positions, const std::vector<veci3>& indices);

}

#endif // UTILS_TOOLS_H
