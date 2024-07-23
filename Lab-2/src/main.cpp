#include <iostream>
#include <cstdint>
#include <vector>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Eigen/Dense"

#include "shader.h"
#include "transformation.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void error_callback(int code, const char *description);
inline void process_input(GLFWwindow* window);

const uint32_t SCR_WIDTH = 800;
const uint32_t SCR_HEIGHT = 600;

static float scale_x = 1.0f, scale_y = 1.0f, scale_z = 1.0f;
static float trans_x = 0.0f, trans_y = 0.0f, trans_z = 0.0f;
static float camera_height = 0.0f;
static float camera_angle = 0.0f;
static float fov = 120.0f;

int main(int argc, char **argv) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // set window and context
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MiniDraw", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "[E] Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetErrorCallback(error_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[E] Failed to initialize GLAD." << std::endl;
        return -2;
    }

    // set for imgui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    static_cast<void>(io);
    ImGui::StyleColorsLight();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Shader shader(SHADER_DIR"/point.vert", SHADER_DIR"/point.frag");
    Shader shader(SHADER_DIR"/point.vert", SHADER_DIR"/point.frag");

    float vertices[] = {
      -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    };
    int indices[] = {
            0, 2, 3,
            3, 1, 0,
            0, 4, 6,
            6, 2, 0,
            0, 1, 5,
            5, 4, 0,
            4, 5, 7,
            7, 6, 4,
            1, 3, 7,
            7, 5, 1,
            2, 6, 7,
            7, 3, 2,
    };
    float colors[] = {
        0.0f,0.0f,0.0f,
        1.0f,0.0f,0.0f,
        0.0f,1.0f,0.0f,
        1.0f,1.0f,0.0f,
        0.0f,0.0f,1.0f,
        1.0f,0.0f,1.0f,
        0.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f
    };

    constexpr int vertex_count = std::size(vertices) / 3;
    constexpr int triangle_count = std::size(indices) / 3;

    uint32_t VBO, VAO, EBO, ECO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    
    glGenBuffers(1, &ECO);
    glBindBuffer(GL_ARRAY_BUFFER, ECO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Attributes");
        ImGui::SliderFloat3("Scale", &scale_x, 0.4f, 1.6f);
        ImGui::SliderFloat3("Translation", &trans_x, -0.5f, 0.5f);
        ImGui::SliderFloat("Camera Height", &camera_height, -2.0f, 2.0f);
        ImGui::SliderFloat("Camera Angle", &camera_angle, -180.0f, 180.0f);
        ImGui::SliderFloat("fov", &fov, 60.0f, 160.0f);
        ImGui::End();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Eigen::Matrix4f sca_mat = get_scaling_matrix(Eigen::Vector3f(scale_x, scale_y, scale_z));
        Eigen::Matrix4f rot_mat = get_rotation_matrix(glfwGetTime() * 15.0);
        Eigen::Matrix4f tra_mat = get_translation_matrix(Eigen::Vector3f(trans_x, trans_y, trans_z));

        Eigen::Matrix4f modl_mat = tra_mat * rot_mat * sca_mat;
        Eigen::Matrix4f view_mat = get_view_matrix(Eigen::Vector3f(1.5f * sin(camera_angle / 180 * PI), camera_height, 1.5f * cos(camera_angle / 180 * PI)));
        Eigen::Matrix4f proj_mat = get_projection_matrix(fov, (float)SCR_WIDTH / SCR_HEIGHT, .1f, 10.0f);

        Eigen::Matrix4f mvp_mat = proj_mat * view_mat * modl_mat;

        shader.use_program();
        uint32_t view_loc = glGetUniformLocation(shader.get_id(), "MVP");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, mvp_mat.data());

        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);

        glDrawElements(GL_TRIANGLES, triangle_count * 3, GL_UNSIGNED_INT, 0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    shader.delete_program();

    glfwTerminate();
    return 0;
}

void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void error_callback(int code, const char *description) {
    std::cerr << "[E]" << code << " : " << description << std::endl;
}
