#include "mesh_simplification.h"

Model *simplify_mesh(
        const std::vector<vecf3>& _vertices,    // positions of vertices in the mesh
        const std::vector<veci3>& _faces,       // indices of vertices in each face
        float ratio                             // the ratio of the number of vertices after simplification to the original number of vertices
        ) {

    // avoid modifying the original mesh
    std::vector<vecf3> vertices = _vertices;
    std::vector<veci3> faces = _faces;

    // record whether the vertex is deleted
    std::deque<bool> vertices_deleted(vertices.size(), false);

    // record the face index of each vertex,
    std::vector<std::vector<int>> faces_of_vertices(vertices.size());
    for (int i = 0; i < faces.size(); ++i) {
        for (int j = 0; j < 3; ++j) {
            faces_of_vertices[faces[i][j]].push_back(i);
        }
    }
    for (int i = 0; i < vertices.size(); ++i) {
        if (faces_of_vertices[i].empty()) {
            vertices_deleted[i] = true;
        }
    }

    // TODO 3.1:
    // compute the Q matrices for all the initial vertices

    
    // TODO 3.2:
    // select all valid pairs(edges) and compute the cost of each edge


    // TODO 3.3:
    // iteratively remove the pair of the least cost from the heap
    uint32_t face_cnt = faces.size();
    uint32_t target_face_cnt = face_cnt * ratio;
    while (face_cnt > target_face_cnt) {
        // remove the min edge from the heap


        // update the costs of all valid pairs

        
        // maintain the faces
        // set face invalid (with -1, -1, -1)
        

    }

    // create the new mesh
    int new_vert_cnt = 0;
    int new_face_cnt = 0;
    for (auto i = 0; i < vertices.size(); ++i) {
        if (!vertices_deleted[i]) {
            vertices[new_vert_cnt] = vertices[i];
            for (auto face : faces_of_vertices[i]) {
                assert(face != -1);
                for (int j = 0; j < 3; ++j) {
                    if (faces[face][j] == i) {
                        faces[face][j] = new_vert_cnt;
                    }
                }
            }
            new_vert_cnt += 1;
        }
    }
    for (int i = 0; i < faces.size(); ++i) {
        if (faces[i][0] != faces[i][1] && faces[i][1] != faces[i][2] && faces[i][2] != faces[i][0]) {
            faces[new_face_cnt] = faces[i];
            new_face_cnt += 1;
        }
    }
    vertices.resize(new_vert_cnt);
    faces.resize(new_face_cnt);

    return Model::load(vertices, faces);
}
