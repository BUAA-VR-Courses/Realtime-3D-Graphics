#ifndef UTILS_MODEL_H
#define UTILS_MODEL_H

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Eigen/Dense"

#include "utils/tools.h"
#include "utils/gl/vertex_array.h"

namespace Utils {

using GL::VertexArray;
using GL::VertexBuffer;
using GL::ElementBuffer;

struct Model {
    ~Model();

    std::unique_ptr<VertexArray> va = nullptr;
    std::unique_ptr<ElementBuffer> eb = nullptr;

    std::map<std::string, std::unique_ptr<VertexBuffer>> vbos;
    std::vector<vecf3> positions;
    std::vector<vecf2> texcoords;
    std::vector<vecf3> normals;
    std::vector<vecf3> tangents;
    std::vector<veci3> indices;
    
    static Model *load(const std::string& path);
    // from mesh
    static Model *load(std::vector<vecf3>&& positions, std::vector<veci3>&& indices);
    static Model *load(const std::vector<vecf3>& positions, const std::vector<veci3>& indices);
};

} 

#endif // UTILS_MODEL_H
