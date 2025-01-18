#ifndef UTILS_TOOLS_H
#define UTILS_TOOLS_H

#pragma once

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

inline std::vector<vecf3> generate_normals(const std::vector<vecf3>& positions, const std::vector<veci3>& indices) {
    std::vector<vecf3> normals(positions.size(), vecf3::Zero());
    for (const auto& idx : indices) {
        const auto& p0 = positions[idx[0]];
        const auto& p1 = positions[idx[1]];
        const auto& p2 = positions[idx[2]];
        auto normal = (p1 - p0).cross(p2 - p0);
        normals[idx[0]] += normal;
        normals[idx[1]] += normal;
        normals[idx[2]] += normal;
    }
    for (auto& normal : normals) {
        normal = normal.normalized();
    }
    return normals;
}

}

#endif // UTILS_TOOLS_H
