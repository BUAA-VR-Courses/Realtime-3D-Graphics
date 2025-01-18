#include "tools.h"

namespace Utils {

std::vector<vecf3> generate_normals(const std::vector<vecf3>& positions, const std::vector<veci3>& indices) {
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

} // namespace Utils