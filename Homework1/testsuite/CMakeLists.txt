project(testsuite LANGUAGES C CXX)

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC "${PROJECT_SOURCE_DIR}/src/")
set(RAS_SRC "${PROJECT_SOURCE_DIR}/../rasterization/src/")
set(TESTCASES "${PROJECT_SOURCE_DIR}/testcases")
add_compile_definitions(TESTCASES_DIR="${TESTCASES}/")

file(GLOB_RECURSE PRJ_SRC "${SRC}*.cpp")

add_executable(${PROJECT_NAME} 
    ${PRJ_SRC}
    ${RAS_SRC}/rasterization.cpp
    ${RAS_SRC}/utils.cpp
)
target_link_libraries(${PROJECT_NAME}
)
target_include_directories(${PROJECT_NAME}
    PUBLIC ${SRC}
    ${RAS_SRC}
)
