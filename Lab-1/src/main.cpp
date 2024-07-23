#include <iostream>
#include <cstdint>
#include <vector>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "shader.h"
#include "utils.h"
#include "rasterization.h"

#define STRINGIFY2(X) #X

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos);
void error_callback(int code, const char *description);
inline void process_input(GLFWwindow* window);

const uint32_t SCR_WIDTH = 800;
const uint32_t SCR_HEIGHT = 600;

const uint32_t SUP_DRAW_NUM = 10000; // superior boundary of drawn pixels

uint32_t pressing = 0;
uint32_t drawn_size = 0;
uint32_t drawing_size = 0;
Pixel start, end;

enum class DrawMode : int {
    line_dda = 1,
    line_bresenham = 2,
    ellipse = 3,
};
static DrawMode mode = DrawMode::line_dda;

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
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
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

    Shader shader(SHADER_DIR"/point.vert", SHADER_DIR"/point.frag");

    uint32_t VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, SUP_DRAW_NUM * sizeof(Vector2f), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2f), 0);
    glEnableVertexAttribArray(0);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Choose Mode");
        ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::RadioButton("line (dda)", reinterpret_cast<int *>(&mode), (int)DrawMode::line_dda);
        ImGui::RadioButton("line (bresenham)", reinterpret_cast<int *>(&mode), (int)DrawMode::line_bresenham);
        ImGui::RadioButton("ellipse", reinterpret_cast<int *>(&mode), (int)DrawMode::ellipse);
        ImGui::End();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use_program();
        glBindVertexArray(VAO);
        // std::cout << drawn_size << ", " << drawing_size << std::endl;
        glDrawArrays(GL_POINTS, 0, drawn_size + drawing_size);

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

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse || button != GLFW_MOUSE_BUTTON_LEFT) {
        return;
    }
    
    if (action == GLFW_PRESS) {
        pressing = 1;

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        start.x = end.x = static_cast<int>(xpos);
        start.y = end.y = static_cast<int>(ypos);
    } else if (action == GLFW_RELEASE) {
        pressing = 0;
        
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        std::vector<Pixel> pixels;
        std::vector<Vector2f> vertices;
        if (mode == DrawMode::line_dda) {
            draw_line_dda(start, end, pixels);
        } else if (mode == DrawMode::line_bresenham) {
            draw_line_bresenham(start, end, pixels);
        } else if (mode == DrawMode::ellipse) {
            draw_ellipse(start, end, pixels);
        }
        for (auto& pixel : pixels) {
            vertices.emplace_back(pixel.to_vertex(width, height));
        }

        if (drawn_size + drawing_size < SUP_DRAW_NUM) {
            glBufferSubData(GL_ARRAY_BUFFER, drawn_size * sizeof(Vector2f), vertices.size() * sizeof(Vector2f), vertices.data());
            drawn_size += vertices.size();
        } else {
            std::cerr << "[E] Too many points to display!" << std::endl;
        }
        drawing_size = 0;
    }
}

void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return;
    }

    if (pressing) {
        end.x = static_cast<int>(xpos);
        end.y = static_cast<int>(ypos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        std::vector<Pixel> pixels;
        std::vector<Vector2f> vertices;
        if (mode == DrawMode::line_dda) {
            draw_line_dda(start, end, pixels);
        } else if (mode == DrawMode::line_bresenham) {
            draw_line_bresenham(start, end, pixels);
        } else if (mode == DrawMode::ellipse) {
            draw_ellipse(start, end, pixels);
        }
        for (auto& pixel : pixels) {
            vertices.emplace_back(pixel.to_vertex(width, height));
        }
        drawing_size = vertices.size();

        if (drawn_size + drawing_size < SUP_DRAW_NUM) {
            glBufferSubData(GL_ARRAY_BUFFER, drawn_size * sizeof(Vector2f), (SUP_DRAW_NUM - drawn_size) * sizeof(Vector2f), nullptr);
            glBufferSubData(GL_ARRAY_BUFFER, drawn_size * sizeof(Vector2f), vertices.size() * sizeof(Vector2f), vertices.data());
        } else {
            std::cerr << "[E] Too many points to display!" << std::endl;
        }
    }
}

void error_callback(int code, const char *description) {
    std::cerr << "[E]" << code << " : " << description << std::endl;
}
