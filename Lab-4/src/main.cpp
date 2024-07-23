#include <iostream>
#include <cstdint>
#include <vector>
#include <memory>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Eigen/Dense"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "utils/shader.h"
#include "utils/camera.h"
#include "utils/model.h"
#include "utils/tools.h"
#include "utils/transform.h"
#include "utils/gl/core.h"
#include "utils/gl/texture.h"
#include "utils/gl/frame_buffer.h"

using Utils::Camera;
using Utils::Shader;
using Utils::Model;
using Utils::GL::Texture2D;
using Utils::GL::FrameBuffer;
using Utils::Transform::generate_model_matrix;
using Utils::Transform::look_at;
using Utils::Transform::perspective;
using Utils::Transform::rotate_with;

// declare callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void error_callback(int code, const char *description);
void process_input(GLFWwindow* window);
void process_release(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);

// load texture
Texture2D load_texture(const char *path);
Texture2D load_shadow_map(size_t shadow_texture_size);

// screen settings
static uint32_t SCR_WIDTH = 800;
static uint32_t SCR_HEIGHT = 600;

// camera
Camera camera(vecf3(0.0f, 0.0f, 3.0f));
float last_x = SCR_WIDTH / 2.0f;
float last_y = SCR_HEIGHT / 2.0f;
bool init_mouse = true;

// time
float delta_time = 0.0f;	// time between current frame and last frame
float last_frame = 0.0f;

// runtime status
bool is_space_pressing = false;
bool show_shadow = false;

// shadow map settings
constexpr size_t SHADOW_TEXTURE_SIZE = 1024;

int main(int argc, char **argv) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // set window and context
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Rendering", nullptr, nullptr);
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

    // shader setting
    Shader light_shader(SHADER_DIR"/p3t2n3.vert", SHADER_DIR"/light_shadow.frag");
    Shader shadow_shader(SHADER_DIR"/p3.vert", SHADER_DIR"/empty.frag");

    float ambient = 0.2f;
    float specular = 0.8f;
    auto light_pos = vecf3(0.0f, 10.0f, 0.0f);
    light_shader.set_tex("color_texture", 0);
    light_shader.set_tex("shadow_map", 1);
    light_shader.set_vecf3("point_light_pos", light_pos);
    light_shader.set_vecf3("point_light_radiance", {200, 200, 200});
    light_shader.set_float("ambient", ambient);
    light_shader.set_float("specular", specular);
    
    // load cow model
    auto cow_model = std::unique_ptr<Model>(Model::load(RESOURCES_DIR"/spot_triangulated_good.obj"));
    auto cow_texture = load_texture(RESOURCES_DIR"/spot_albedo.png");
    std::vector<vecf3> cow_translates = {
        vecf3(0.0f,  -1.0f,  0.0f),
        vecf3(2.0f,  5.0f, -15.0f),
        vecf3(-1.5f, -1.2f, -2.5f),
        vecf3(-3.8f, 3.0f, -12.3f),
        vecf3(2.4f, 0.4f, -3.5f),
        vecf3(-1.7f,  3.0f, -7.5f),
        vecf3(1.3f, 5.0f, -2.5f),
        vecf3(1.5f,  2.0f, -2.5f),
        vecf3(1.5f,  0.2f, -1.5f),
        vecf3(-1.3f,  1.0f, -1.5f),
    };

    // load plane model
    auto plane_model = std::unique_ptr<Model>(Model::load(RESOURCES_DIR"/plane.obj"));
    auto plane_texture = load_texture(RESOURCES_DIR"/checkerboard.png");
    vecf3 plane_pos(0.0f, -3.0f, -8.0f);
    vecf3 plane_scale(20.0f, 1.0f, 20.0f);
    matf4 plane_transform = generate_model_matrix(plane_pos, plane_scale, matf4::Identity());

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // init shadow map
    auto shadow_map = load_shadow_map(SHADOW_TEXTURE_SIZE);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, vecf4(1.0f, 1.0f, 1.0f, 1.0f).data());
    FrameBuffer shadow_fbo;
    shadow_fbo.attach(GL_DEPTH_ATTACHMENT, &shadow_map);

    while (!glfwWindowShouldClose(window)) {
        // record time
        auto current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        process_input(window);
        process_release(window);

        /////////////////////////////////////////////////
        // render shadow map
        shadow_fbo.bind();
        glViewport(0, 0, SHADOW_TEXTURE_SIZE, SHADOW_TEXTURE_SIZE);
        glClear(GL_DEPTH_BUFFER_BIT);

        // TODO 4.2 : Uncomment the following segment, then modify the light_projection and light_view implementations yourself.
        auto light_projection = matf4::Identity();
        auto light_view = matf4::Identity();
        auto light_space_matrix = light_projection * light_view;
//        shadow_shader.set_matf4("projection", light_projection);
//        shadow_shader.set_matf4("view", light_view);
//
//        for (auto i = 0; i < cow_translates.size(); ++i) {
//            float angle = 20.0f * i + 10.0f * static_cast<float>(glfwGetTime());
//            auto model_mat = generate_model_matrix(cow_translates[i], vecf3(1.0f, 1.0f, 1.0f),
//                                                rotate_with(to_radian(angle), vecf3(0.26726124, 0.53452248, 0.80178373)));
//            shadow_shader.set_matf4("model", model_mat);
//            cow_model->va->draw(shadow_shader);
//        }
//
//        shadow_shader.set_matf4("model", plane_transform);
//        plane_model->va->draw(shadow_shader);
        FrameBuffer::bind_reset();

        /////////////////////////////////////////////////
        // render light
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(ambient, ambient, ambient, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        light_shader.set_vecf3("camera_pos", camera.position);
        light_shader.active_texture(0, &cow_texture);
        light_shader.active_texture(1, &shadow_map);

        auto projection = perspective(to_radian(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        light_shader.set_matf4("projection", projection);
        light_shader.set_matf4("view", camera.get_view_matrix());
        light_shader.set_bool("have_shadow", show_shadow);
        light_shader.set_matf4("light_space_matrix", light_space_matrix);

        for (auto i = 0; i < cow_translates.size(); ++i) {
            float angle = 20.0f * i + 10.0f * static_cast<float>(glfwGetTime());
            auto model_mat = generate_model_matrix(cow_translates[i], vecf3(1.0f, 1.0f, 1.0f),
                                                rotate_with(to_radian(angle), vecf3(0.26726124, 0.53452248, 0.80178373)));
            light_shader.set_matf4("model", model_mat);
            cow_model->va->draw(light_shader);
        }

        light_shader.active_texture(0, &plane_texture);
        light_shader.set_matf4("model", plane_transform);
        plane_model->va->draw(light_shader);

        // show
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    light_shader.delete_program();
    shadow_shader.delete_program();

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

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        is_space_pressing = true;
    } 
}

void process_release(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && is_space_pressing) {
        is_space_pressing = false;
        show_shadow = !show_shadow;
    }
}

void mouse_callback(GLFWwindow *window, double x_pos, double y_pos) {
    if (init_mouse) {
        last_x = static_cast<float>(x_pos);
        last_y = static_cast<float>(y_pos);
        init_mouse = false;
    }

    float x_offset = static_cast<float>(x_pos) - last_x;
    float y_offset = last_y - static_cast<float>(y_pos);

    last_x = static_cast<float>(x_pos);
    last_y = static_cast<float>(y_pos);

    camera.process_mouse_movement(x_offset, y_offset);
}

void scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
    camera.process_mouse_scroll(static_cast<float>(y_offset));
}

Texture2D load_texture(const char *path) {
    Texture2D tex;
    tex.set_wrap_filter(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR);
    int width, height, nr_channels;
    stbi_set_flip_vertically_on_load(true);
    uint8_t *data = stbi_load(path, &width, &height, &nr_channels, 0);
    Utils::GL::PixelDataFormat c2f[4] = {
        GL_RED,
        GL_RG,
        GL_RGB,
        GL_RGBA
    };
    Utils::GL::PixelDataInternalFormat c2if[4] = {
        GL_RED,
        GL_RG,
        GL_RGB,
        GL_RGBA
    };
    if (data) {
        tex.set_image(0, c2if[nr_channels - 1], width, height, c2f[nr_channels - 1], GL_UNSIGNED_BYTE, data);
        tex.gen_mipmap();
    } else {
        std::cerr << "[E] Failed to load texture." << std::endl;
    }
    stbi_image_free(data);

    return tex;
}

Texture2D load_shadow_map(size_t shadow_texture_size) {
    Texture2D shadow_map;
    shadow_map.set_image(0, GL_DEPTH_COMPONENT, shadow_texture_size, shadow_texture_size, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    shadow_map.set_wrap_filter(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, GL_NEAREST, GL_NEAREST);

    return shadow_map;
}
