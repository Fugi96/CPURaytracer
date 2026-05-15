#pragma once
#include "util/Matrix.h"
#include "util/Ray.h"
#include <iostream>
#include <math.h>
#include "Canvas.h"
#include "World.h"
#include <omp.h>
#include <chrono>

struct Camera {
    int hsize;
    int vsize;
    float field_of_view;
    Matrix<4, 4> transform{glm::mat4(1.0f)};
    Matrix<4, 4> transform_inv{ glm::mat4(1.0f) };
    float half_view;
    float half_width;
    float half_height;
    float pixel_size;

    Camera(int hsize, int vsize, float field_of_view) : hsize(hsize), vsize(vsize), field_of_view(field_of_view) {
        float aspect = static_cast<float>(hsize) / static_cast<float>(vsize);
        this->half_view = std::tanf(field_of_view / 2.0f);

        if (aspect >= 1.0f) {
            this->half_width = this->half_view;
            this->half_height = this->half_view / aspect;
        }
        else {
            this->half_width = half_view * aspect;
            this->half_height = half_view;
        }
        this->pixel_size = (this->half_width * 2.0f) / static_cast<float>(hsize);
    }

    void set_transform(const Matrix<4, 4>& transform) {
        this->transform = transform;
        this->transform_inv = Matrix<4, 4>::inverse(transform);
    }

     Ray ray_for_pixel(int x, int y) const {
        float xoffset = (0.5f + x) * this->pixel_size;
        float yoffset = (0.5f + y) * this->pixel_size;

        float world_x = this->half_width - xoffset;
        float world_y = this->half_height - yoffset;

        Point pixel = transform_inv * Point(glm::vec3(world_x, world_y, -1.0f));
        Point origin = transform_inv * Point(glm::vec3(0.0f, 0.0f, 0.0f));
        Vector direction = Vector::normalize(pixel - origin);

        return Ray(origin, direction);
    }

    void render(World& world, Canvas& image, bool& should_restart) const {
        #pragma omp parallel for schedule(static)
        for (int y = 0; y < this->vsize; ++y) {
            for (int x = 0; x < this->hsize; ++x) {
                if (should_restart) continue;
                Ray ray = this->ray_for_pixel(x, y);
                Color c = world.color_at(ray);
                image.write_pixel(x, y, c);
            }
        }
    }

    void render_batched(World& world, Canvas& image, bool& should_restart) const {
        constexpr int ray_batch_size = 8;
        #pragma omp parallel for schedule(static)
        for (int y = 0; y < this->vsize; ++y) {
            for (int x = 0; x < this->hsize; x+=ray_batch_size) {
                if (should_restart) continue;
                std::array<Ray, ray_batch_size> ray_batch;
                for (int offset = 0; offset < ray_batch_size; ++offset) {
                    ray_batch[offset] = ray_for_pixel(x + offset, y);
                }
                std::array<Color, ray_batch_size> colors = world.color_at_batch(ray_batch.data());
                for (int offset = 0; offset < ray_batch_size; ++offset) {
                    image.write_pixel(x+offset, y, colors[offset]);
                }
            }
        }
    }

    void benchmark(World& world, Canvas& image, bool& should_restart) const {
        using clock = std::chrono::high_resolution_clock;

        constexpr int iterations = 10;
        //render(world, image, should_restart);
        //image.reset_image();
        render_batched(world, image, should_restart);
        image.reset_image();

        auto old_start = clock::now();

        //for (size_t i = 0; i < iterations; ++i) {
        //    auto iteration_start = clock::now();
        //    render(world, image, should_restart);
        //    image.reset_image();
        //    auto iteration_end = clock::now();
        //    double iteration_ms =
        //        std::chrono::duration<double, std::milli>(
        //        iteration_end - iteration_start
        //    ).count();
        //    std::cout << "Single Primary Ray Method Iteration " << i << " " << iteration_ms << "ms" << "\n";
        //}

        auto old_end = clock::now();

        auto new_start = clock::now();

        for (size_t i = 0; i < iterations; ++i) {
            auto iteration_start = clock::now();
            render_batched(world, image, should_restart);
            image.reset_image();
            auto iteration_end = clock::now();
            double iteration_ms =
                std::chrono::duration<double, std::milli>(
                iteration_end - iteration_start
            ).count();
            std::cout << "8 Primary Rays Batch Method Iteration " << i << " " << iteration_ms << "ms" << "\n";
        }

        auto new_end = clock::now();

            double old_ms =
        std::chrono::duration<double, std::milli>(
            old_end - old_start
        ).count();

        double new_ms =
            std::chrono::duration<double, std::milli>(
                new_end - new_start
            ).count();

        std::cout << "\n========== BENCHMARK ==========\n";

        std::cout << "Iterations: " << iterations << "\n\n";

        std::cout << "color_at():        "
                  << old_ms
                  << " ms total ("
                  << old_ms / iterations
                  << " ms avg)\n";

        std::cout << "direct_color_at(): "
                  << new_ms
                  << " ms total ("
                  << new_ms / iterations
                  << " ms avg)\n";

        if (new_ms > 0.0) {
            std::cout << "\nSpeedup: "
                      << (old_ms / new_ms)
                      << "x\n";
        }

        std::cout << "================================\n";
        render_batched(world, image, should_restart);
    }
};  