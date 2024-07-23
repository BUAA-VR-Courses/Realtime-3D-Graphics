#include <utils/model.h>

namespace Utils {

Model::~Model() = default;

Model *Model::load(const std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "[E] Failed to open file: " << path << std::endl;
        return nullptr;
    }

    auto size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);
    char *buffer = new char[size + 1];
    buffer[size] = 0;
    file.read(buffer, static_cast<int64_t>(size));
    file.close();

    std::vector<std::string> lines;
    for (size_t i = 0; i <= size; ) {
        auto j = 0;
        while ((buffer[i + j] != '\n') && (buffer[i + j] != '\0')) {
            j++;
        }
        lines.emplace_back(buffer + i, j);
        i += j + 1;
    }
    delete[] buffer;
    buffer = nullptr;
    
    std::vector<vecf3> positions;
    std::vector<vecf3> normals;
    std::vector<vecf2> texcoords;
    std::vector<veci3> indices;
    std::vector<vecf3> tangents;
    for (const auto& line : lines) {
        if (line.empty() || line == "\n" || line == "\r\n") {
            continue;
        }

        std::stringstream ss(line);
        std::string id;
        ss >> id;
        if (id == "v") {
            vecf3 pos;
            ss >> pos[0] >> pos[1] >> pos[2];
            positions.emplace_back(pos);
        } else if (id == "f") {
            veci3 idx;
            ss >> idx[0] >> idx[1] >> idx[2];
            indices.emplace_back(idx - veci3::Ones());
        } else if (id == "vt") {
            vecf2 tex;
            ss >> tex[0] >> tex[1];
            texcoords.emplace_back(tex);
        } else if (id == "vn") {
            vecf3 normal;
            ss >> normal[0] >> normal[1] >> normal[2];
            normals.emplace_back(normal);
        } else if (id == "t") {
            vecf3 tangent;
            ss >> tangent[0] >> tangent[1] >> tangent[2];
            tangents.emplace_back(tangent);
        }
    }
    
    auto model = new Model;

    // I don't implement the bounding box for this
    vecf3 center = vecf3::Zero();
    float scale = 0.0f;
    for (const auto& pos : positions) {
        center += pos;
    }
    center /= static_cast<float>(positions.size());
    for (const auto& pos : positions) {
        scale = std::max(scale, (pos - center).norm());
    }
    scale = 1.0f / scale;
    for (auto& pos : positions) {
        pos = (pos - center) * scale;
    }
    
    if (normals.empty()) {
        normals = generate_normals(positions, indices);
    }
    
    auto vb_pos = new VertexBuffer(static_cast<GLsizeiptr>(positions.size() * sizeof(vecf3)), positions.data());
    auto vb_uv = new VertexBuffer(static_cast<GLsizeiptr>(texcoords.size() * sizeof(vecf2)), texcoords.data());
    auto vb_norm = new VertexBuffer(static_cast<GLsizeiptr>(normals.size() * sizeof(vecf3)), normals.data());
    auto vb_t = new VertexBuffer(static_cast<GLsizeiptr>(tangents.size() * sizeof(vecf3)), tangents.data());
    auto eb = new ElementBuffer(GL_TRIANGLES, indices.size(), (GLuint *)indices.data());

    VertexArray::Format format;
    format.attr_ptrs.emplace_back(vb_pos->attr_ptr(3, GL_FLOAT, GL_FALSE, sizeof(vecf3)));
    format.attr_ptrs.emplace_back(vb_uv->attr_ptr(2, GL_FLOAT, GL_FALSE, sizeof(vecf2)));
    format.attr_ptrs.emplace_back(vb_norm->attr_ptr(3, GL_FLOAT, GL_FALSE, sizeof(vecf3)));
    format.attr_ptrs.emplace_back(vb_t->attr_ptr(3, GL_FLOAT, GL_FALSE, sizeof(vecf3)));
    format.eb = eb;

    model->vbos["position"] = std::unique_ptr<VertexBuffer>(vb_pos);
    model->vbos["normal"] = std::unique_ptr<VertexBuffer>(vb_norm);
    model->eb = std::unique_ptr<ElementBuffer>(eb);
    model->va = std::make_unique<VertexArray>(std::vector<GLuint>{0, 1, 2, 3}, format);

    model->positions = std::move(positions);
    model->texcoords = std::move(texcoords);
    model->normals = std::move(normals);
    model->tangents = std::move(tangents);
    model->indices = std::move(indices);

    return model;
}

Model *Model::load(std::vector<vecf3>&& positions, std::vector<veci3>&& indices) {
    auto model = new Model;
    auto normals = generate_normals(positions, indices);
    auto vb_pos = new VertexBuffer(static_cast<GLsizeiptr>(positions.size() * sizeof(vecf3)), positions.data());
    auto vb_norm = new VertexBuffer(static_cast<GLsizeiptr>(normals.size() * sizeof(vecf3)), normals.data());
    auto eb = new ElementBuffer(GL_TRIANGLES, indices.size(), (GLuint *)indices.data());

    VertexArray::Format format;
    format.attr_ptrs.emplace_back(vb_pos->attr_ptr(3, GL_FLOAT, GL_FALSE, sizeof(vecf3)));
    format.attr_ptrs.emplace_back(vb_norm->attr_ptr(3, GL_FLOAT, GL_FALSE, sizeof(vecf3)));
    format.eb = eb;

    model->vbos["position"] = std::unique_ptr<VertexBuffer>(vb_pos);
    model->vbos["normal"] = std::unique_ptr<VertexBuffer>(vb_norm);
    model->eb = std::unique_ptr<ElementBuffer>(eb);
    model->va = std::make_unique<VertexArray>(std::vector<GLuint>{0, 1}, format);

    model->positions = std::move(positions);
    model->normals = std::move(normals);
    model->indices = std::move(indices);

    return model;
}

Model *Model::load(const std::vector<vecf3>& positions, const std::vector<veci3>& indices) {
    auto model = new Model;
    auto normals = generate_normals(positions, indices);
    auto vb_pos = new VertexBuffer(static_cast<GLsizeiptr>(positions.size() * sizeof(vecf3)), positions.data());
    auto vb_norm = new VertexBuffer(static_cast<GLsizeiptr>(normals.size() * sizeof(vecf3)), normals.data());
    auto eb = new ElementBuffer(GL_TRIANGLES, indices.size(), (GLuint *)indices.data());

    VertexArray::Format format;
    format.attr_ptrs.emplace_back(vb_pos->attr_ptr(3, GL_FLOAT, GL_FALSE, sizeof(vecf3)));
    format.attr_ptrs.emplace_back(vb_norm->attr_ptr(3, GL_FLOAT, GL_FALSE, sizeof(vecf3)));
    format.eb = eb;

    model->vbos["position"] = std::unique_ptr<VertexBuffer>(vb_pos);
    model->vbos["normal"] = std::unique_ptr<VertexBuffer>(vb_norm);
    model->eb = std::unique_ptr<ElementBuffer>(eb);
    model->va = std::make_unique<VertexArray>(std::vector<GLuint>{0, 1}, format);

    model->positions = positions;
    model->normals = std::move(normals);
    model->indices = indices;

    return model;
}

}
