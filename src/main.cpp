#include <iostream>
#include <cstdint>
#include <vector>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Eigen/Dense"

#include "utils/shader.h"
#include "utils/camera.h"
#include "utils/model.h"
#include "utils/tools.h"
#include "mesh_simplification.h"

using Utils::Camera;
using Utils::Shader;
using Utils::Model;

// declare callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void error_callback(int code, const char *description);
void process_input(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// screen settings
static uint32_t SCR_WIDTH = 800;
static uint32_t SCR_HEIGHT = 600;

// camera
Camera camera(vecf3(0.0f, 0.0f, 3.0f));
float last_x = SCR_WIDTH / 2.0f;
float last_y = SCR_HEIGHT / 2.0f;
bool init_mouse = true;
float base_angle_y = 0.0f;
float angle_x = 0.0f;

// time
float delta_time = 0.0f;	// time between current frame and last frame
float last_frame = 0.0f;

// runtime status
bool is_left_pressing = false;
bool is_right_pressing = false;
bool is_changing = false;
bool is_b_pressing = false;
bool shows_border = false;

// model simplification
std::vector<std::unique_ptr<Model>> meshes;
int current_index = 0;
float simplification_ratio = 0.8f;

int main(int argc, char **argv) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // set window and context
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MeshSimplification", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "[E] Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetErrorCallback(error_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // load opengl function pointers
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

    // shader setting
    Shader shader(SHADER_DIR"/p3n3.vert", SHADER_DIR"/light.frag");
    Shader border_shader(SHADER_DIR"/p3n3.vert", SHADER_DIR"/border.frag");

    vecf3 ambient(0.2f, 0.2f, 0.2f);
    vecf3 point_light_pos(0.0f, 10.0f, 0.0f);
    vecf3 point_light_radiance(255.0f, 255.0f, 255.0f);

    shader.set_vecf3("point_light_pos", point_light_pos);
    shader.set_vecf3("point_light_radiance", point_light_radiance);
    shader.set_vecf3("local_color", {1.0f, 1.0f, 1.0f});
    shader.set_vecf3("ambient_irradiance", ambient);
    shader.set_float("roughness", 0.6f);
    shader.set_float("metalness", 0.1f);

    vecf3 border_color(1.0f, 1.0f, 0.0f);

    border_shader.set_vecf3("border_color", border_color);
    
    // load meshes
    meshes.emplace_back(Model::load(RESOURCES_DIR"/squirrel.obj"));
    
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        // record time
        auto current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // while calculating the meshes and vertices, I recommend avoid processing input
        if (!is_changing) {
            process_input(window);
        } else {
             if (current_index < meshes.size() - 1) {
                 current_index += 1;
             } else {
                 is_changing = false;
             }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(200, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);
        ImGui::Begin("Attributes");
        ImGui::Text("vertices: %d", meshes[current_index]->positions.size());
        ImGui::Text("faces: %d", meshes[current_index]->indices.size());
        ImGui::Text("borders: %s", shows_border ? "On" : "Off");
        ImGui::End();

        glClearColor(ambient[0], ambient[1], ambient[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);

        shader.set_vecf3("camera_pos", camera.position);
        shader.set_matf4("projection", camera.get_projection_matrix(SCR_WIDTH, SCR_HEIGHT, 0.1f, 100.0f));
        shader.set_matf4("view", camera.get_view_matrix());

        // render the model
        vecf3 model_pos(0.0f, 0.0f, 0.0f);
        float angle_y = base_angle_y + static_cast<float>(glfwGetTime());
        // rot matrix
        matf4 model_transform;
        model_transform <<  cos(angle_y), 0.0f, sin(angle_y), model_pos[0],
                                    0.0f, 1.0f,         0.0f, model_pos[1],
                           -sin(angle_y), 0.0f, cos(angle_y), model_pos[2],
                                    0.0f, 0.0f,         0.0f,         1.0f;
        shader.set_matf4("model", model_transform);
        meshes[current_index]->va->draw(shader);

        //render borders
        if (shows_border) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            border_shader.set_vecf3("camera_pos", camera.position);
            border_shader.set_matf4("projection", camera.get_projection_matrix(SCR_WIDTH, SCR_HEIGHT, 0.1f, 100.0f));
            border_shader.set_matf4("view", camera.get_view_matrix());
            border_shader.set_matf4("model", model_transform);
            meshes[current_index]->va->draw(border_shader);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shader.delete_program();

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void error_callback(int code, const char *description) {
    std::cerr << "[E]" << code << " : " << description << std::endl;
}

void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.process_keyboard(Camera::Movement::FORWARD, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.process_keyboard(Camera::Movement::BACKWARD, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.process_keyboard(Camera::Movement::LEFT, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.process_keyboard(Camera::Movement::RIGHT, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.process_keyboard(Camera::Movement::UP, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { 
        camera.process_keyboard(Camera::Movement::DOWN, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        is_b_pressing = true;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        is_left_pressing = true;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        is_right_pressing = true;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        is_changing = true;
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE && is_b_pressing) {
        is_b_pressing = false;
        shows_border = !shows_border;
    }
   

    if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE) && is_left_pressing) {
        if (current_index > 0) {
            current_index -= 1;
        }
        is_left_pressing = false;
    }
    if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE) && is_right_pressing) {
         if (current_index < meshes.size() - 1) {
             current_index += 1;
         } else {
             if ((meshes[current_index]->indices).size() > 200) {
                 meshes.emplace_back(simplify_mesh(meshes[current_index]->positions, meshes[current_index]->indices, simplification_ratio));
                 current_index += 1;
             }
         }
         is_right_pressing = false;
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (init_mouse) {
        last_x = static_cast<float>(xpos);
        last_y = static_cast<float>(ypos);
        init_mouse = false;
    }

    float xoffset = static_cast<float>(xpos) - last_x;
    float yoffset = last_y - static_cast<float>(ypos);

    last_x = static_cast<float>(xpos);
    last_y = static_cast<float>(ypos);

    // base_angle_y += static_cast<float>(xoffset);
    // angle_x += static_cast<float>(yoffset);
    camera.process_mouse_movement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.process_mouse_scroll(static_cast<float>(yoffset));
}
