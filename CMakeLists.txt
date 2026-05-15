cmake_minimum_required(VERSION 3.16)
project(Raytracer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.15.0
)
FetchContent_MakeAvailable(catch2)

add_executable(raytracer
    src/TheRaytracerChallengeGLM.cpp
    src/glad.c
    src/objects/Plane.cpp
    src/objects/Sphere.cpp
    src/Scenes.cpp
)

set_source_files_properties(src/glad.c PROPERTIES LANGUAGE C)

target_include_directories(raytracer PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/test
    ${CMAKE_SOURCE_DIR}/src
)

find_package(OpenMP REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(raytracer PRIVATE
    OpenMP::OpenMP_CXX
    glfw
    OpenGL::GL
)

add_executable(tests 
    src/objects/Plane.cpp
    src/objects/Sphere.cpp
    test/Chapter1_Tests.cpp
    test/Chapter2_Tests.cpp
    test/Chapter3_Tests.cpp
    test/Chapter4_Tests.cpp
    test/Chapter5_Tests.cpp
    test/Chapter6_Tests.cpp
    test/Chapter7_Tests.cpp
    test/Chapter8_Tests.cpp
    test/Chapter9_Tests.cpp
    test/Chapter10_Tests.cpp
    test/Chapter11_Tests.cpp
)

target_include_directories(tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(tests PRIVATE
    Catch2::Catch2WithMain
)

target_compile_options(raytracer PRIVATE
    #-Wall
    #-Wextra
    #-Wpedantic
    #-Wshadow
    #-Wconversion
    #-Wsign-conversion
    -march=native 
)

target_compile_options(tests PRIVATE
    -march=native
)

target_compile_options(raytracer PRIVATE
    $<$<CONFIG:Release>:
        -O3 
        -DNDEBUG 
        -funroll-loops 
        -fno-math-errno
        -fno-trapping-math
        -ffp-contract=fast
    >
)

enable_testing()
include(Catch)
catch_discover_tests(tests)

# -g -DNDEBUG -ffast-math -funroll-loops -ftree-vectorize