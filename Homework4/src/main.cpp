#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Eigen/Dense"

#include "raytracing/camera.h"
#include "raytracing/hit.h"
#include "raytracing/material.h"
#include "raytracing/texture.h"
#include "utils/image.h"
#include "utils/shader.h"
#include "utils/tools.h"
#include "utils/gl/core.h"
#include "utils/gl/texture.h"

using namespace RayTracing;
using Utils::GL::Texture2D;
using Utils::Image;
using Utils::Shader;

// declare callbacks
void error_callback(int code, const char *description);
void process_input(GLFWwindow* window);

// camera
Camera camera(
    800, 800, 10000, 40, 40.0,
    vecf3(478, 278, -600), vecf3(278, 278, 0), vecf3(0.0, 1.0, 0.0),
    vecf3(0.0, 0.0, 0.0), 0.0, 10.0);
Image image(camera.width, camera.height);
int SCR_WIDTH = camera.width;
int SCR_HEIGHT = camera.height;

// update image texture, called in main thread
void update_image(Texture2D& image_texture);
size_t loop_times;

// prepare world scene
HittableList world;
void prepare_scene();

// render thread
void render_image(const Hittable& world);
std::mutex render_mutex;
void render_image_multithread(const Hittable& world);

int main(int argc, char **argv) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // set window and context
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Ray Tracing", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "[E] Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // set callbacks
    glfwSetErrorCallback(error_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // load opengl function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[E] Failed to initialize GLAD." << std::endl;
        return -2;
    }

    // prepare screen
    float screen_vertices[] = {
        -1.0f,  1.0f,  0.0f,  1.0f, // top left
        -1.0f, -1.0f,  0.0f,  0.0f, // bottom left
         1.0f, -1.0f,  1.0f,  0.0f, // bottom right
         1.0f,  1.0f,  1.0f,  1.0f  // top right
    };
    uint32_t screen_indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    uint32_t screen_vao, screen_vbo, screen_ebo;
    glGenVertexArrays(1, &screen_vao);
    glGenBuffers(1, &screen_vbo);
    glGenBuffers(1, &screen_ebo);

    glBindVertexArray(screen_vao);

    glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertices), screen_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screen_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(screen_indices), screen_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // prepare shader
    Shader screen_shader(SHADER_DIR"/display.vert", SHADER_DIR"/display.frag");
    screen_shader.set_tex("image_texture", 0);

    prepare_scene();

	std::thread render_thread(render_image_multithread, std::ref(world));
    render_thread.detach();
	// render_image(world);

    Texture2D image_texture;
    image_texture.set_wrap_filter(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR);

    while (!glfwWindowShouldClose(window)) {
        if (loop_times % 100 == 0) {
            update_image(image_texture);
        }

        process_input(window);

        // render
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(camera.background[0], camera.background[1], camera.background[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        screen_shader.active_texture(0, &image_texture);
        screen_shader.use_program();
        glBindVertexArray(screen_vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // show
        glfwSwapBuffers(window);
        glfwPollEvents();
 
        loop_times += 1;
    }

	// release resources
	glDeleteVertexArrays(1, &screen_vao);
	glDeleteBuffers(1, &screen_vbo);
	glDeleteBuffers(1, &screen_ebo);
    
    glfwTerminate();
    return 0;
}

void error_callback(int code, const char *description) {
    std::cerr << "[E]" << code << " : " << description << std::endl;
}

void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void prepare_scene() {
    HittableList boxes1;
    auto ground = std::make_shared<Lambertian>(vecf3(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes1.add(box(vecf3(x0,y0,z0), vecf3(x1,y1,z1), ground));
        }
    }

    world.add(std::make_shared<BVHNode>(boxes1));

    auto light = std::make_shared<DiffuseLight>(vecf3(7, 7, 7));
    world.add(std::make_shared<Quad>(vecf3(123, 554, 147), vecf3(300, 0, 0), vecf3(0, 0, 265), light));

    auto center1 = vecf3(400, 400, 200);
    auto center2 = center1 + vecf3(30, 0, 0);
    auto sphere_material = std::make_shared<Lambertian>(vecf3(0.7, 0.3, 0.1));
    world.add(std::make_shared<Sphere>(center1, center2, 50, sphere_material));

    world.add(std::make_shared<Sphere>(vecf3(260, 150, 45), 50, std::make_shared<Dielectric>(1.5)));
    world.add(std::make_shared<Sphere>(
        vecf3(0, 150, 145), 50, std::make_shared<Metal>(vecf3(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = std::make_shared<Sphere>(vecf3(360, 150, 145), 70, std::make_shared<Dielectric>(1.5));
    world.add(boundary);
    world.add(std::make_shared<ConstantMedium>(boundary, 0.2, vecf3(0.2, 0.4, 0.9)));
    boundary = std::make_shared<Sphere>(vecf3(0, 0, 0), 5000, std::make_shared<Dielectric>(1.5));
    world.add(std::make_shared<ConstantMedium>(boundary, .0001, vecf3(1, 1, 1)));

    auto emat = std::make_shared<Lambertian>(std::make_shared<ImageTexture>(RESOURCES_DIR"/earthmap.jpg"));
    world.add(std::make_shared<Sphere>(vecf3(400, 200, 400), 100, emat));
    auto pertext = std::make_shared<NoiseTexture>(0.2);
    world.add(std::make_shared<Sphere>(vecf3(220, 280, 300), 80, std::make_shared<Lambertian>(pertext)));

    HittableList boxes2;
    auto white = std::make_shared<Lambertian>(vecf3(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(std::make_shared<Sphere>(random_vecf3(0, 165), 10, white));
    }

    world.add(std::make_shared<Translate>(
        std::make_shared<RotateY>(
            std::make_shared<BVHNode>(boxes2), 15),
            vecf3(-100, 270, 395)
        )
    );
}

void update_image(Texture2D& image_texture) {
    image_texture.set_image(0, GL_RGB, image.width, image.height, GL_RGB, GL_UNSIGNED_BYTE, image.bdata);
    image_texture.gen_mipmap();
}

void render_image(const Hittable& world) {
    image.path = RESOURCES_DIR"/result.png";
    
    for (int j = 0; j < camera.height; j++) {
        std::clog << "\rScanlines remaining: " << (camera.height - j) << ' ' << std::flush;
        for (int i = 0; i < camera.width; i++) {
            vecf3 pixel_color(0, 0, 0);
            for (int sample = 0; sample < camera.samples_per_pixel; sample++) {
                Ray r = camera.get_ray(i, j);
                pixel_color += camera.ray_color(r, camera.max_depth, world);
            }

			image.set_pixel(i, j, pixel_color * camera.pixel_samples_scale);
        }
    }

	image.save();
    std::clog << "\rDone.                       \n";
}

void render_image_multithread(const Hittable& world) {
    image.path = RESOURCES_DIR"/result.png";

    std::queue<size_t> lines;
    for (size_t i = 0; i < camera.height; i++) {
        lines.push(i);
    }

    auto render_row = [&]() {
        while (true) {
            size_t row;

            {
                std::lock_guard<std::mutex> lock(render_mutex);
                if (lines.empty()) {
                    return;
                }
                row = lines.front();
                lines.pop();
            }

            for (size_t i = 0; i < camera.width; i++) {
                vecf3 pixel_color(0, 0, 0);
                for (int sample = 0; sample < camera.samples_per_pixel; sample++) {
                    Ray r = camera.get_ray(i, row);
                    pixel_color += camera.ray_color(r, camera.max_depth, world);
                }
                image.set_pixel(i, row, pixel_color * camera.pixel_samples_scale);
            }
        }
    };

	size_t num_threads = std::thread::hardware_concurrency() / 2;
    std::vector<std::thread> threads;

    for (size_t t = 0; t < num_threads; t++) {
        threads.push_back(std::thread(render_row));
    }

    for (auto& t : threads) {
        t.join();
    }

    image.save();
}
