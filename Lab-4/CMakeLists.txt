cmake_minimum_required(VERSION 3.16)
project(lighting LANGUAGES C CXX)

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC "${PROJECT_SOURCE_DIR}/src/")
set(DEPS "${PROJECT_SOURCE_DIR}/deps/")

set(GLFW_DIR "${DEPS}/glfw")
set(GLAD_DIR "${DEPS}/glad")
set(STB_DIR "${DEPS}/stb")

set(EIGEN "${DEPS}/eigen-3.4.0")
set(EIGEN_UNS "${EIGEN}/unsupported")

set(SHADER_DIR "${SRC}/shaders")
add_compile_definitions(SHADER_DIR="${SHADER_DIR}")
set(RES_DIR "${PROJECT_SOURCE_DIR}/resources")
add_compile_definitions(RESOURCES_DIR="${RES_DIR}")

set(GLFW_INC "${GLFW_DIR}/include")
if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    set(GLFW_LIB "${GLFW_DIR}/lib-vc2022/glfw3dll.lib")
else()
    set(GLFW_LIB "${GLFW_DIR}/lib-mingw-w64/glfw3.dll")
endif()

set(GLAD_INC "${GLAD_DIR}/include")
set(GLAD_SRC "${GLAD_DIR}/src/glad.c")

file(GLOB_RECURSE PRJ_SRC "${SRC}*.cpp")
file(GLOB_RECURSE STB_SRC "${STB_DIR}*.cpp")

add_executable(${PROJECT_NAME} 
    ${PRJ_SRC}
    ${STB_SRC}
    ${GLAD_SRC}
)
target_link_libraries(${PROJECT_NAME} 
    ${GLFW_LIB}
)
target_include_directories(${PROJECT_NAME}
    PUBLIC ${SRC}
    ${GLFW_INC} 
    ${GLAD_INC} 
    ${STB_DIR}
    ${EIGEN}
    ${EIGEN_UNS}
)

