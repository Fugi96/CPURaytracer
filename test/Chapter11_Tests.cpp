#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "objects/Material.h"
#include "objects/Pattern.h"
#include "objects/Sphere.h"
#include "objects/World.h"
#include "shading/Light.h"
#include "util/Intersection.h"
#include "util/Matrix.h"
#include "util/Point.h"
#include "util/Vector.h"
#include "util/util.h"
#include <array>
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <memory>
#include <future>

TEST_CASE("Reflectivity for the default material") {
    Material m = Material();
    REQUIRE(m.m_reflective == Catch::Approx(0.0f));
}

TEST_CASE("Precomputing the reflection vector") {
    Plane p = Plane();
    Ray r(Point(0, 1, -1), Vector(0, -std::sqrtf(2)/2, std::sqrtf(2)/2));
    Intersection i(sqrtf(2), p);

    PreCompute comps(i, r);
    REQUIRE(comps.reflectv == Vector(0, std::sqrtf(2)/2, std::sqrtf(2)/2));
}

TEST_CASE("The reflected color for a nonreflective material") {
    World w = World::default_world();
    Ray r(Point(0, 0, 0), Vector(0, 0, 1));
    auto shape = w.objects[1];
    shape->material.m_ambient = 1;
    Intersection i(1, *shape);

    PreCompute comps(i, r);
    Color color = w.reflected_color(comps);

    REQUIRE(color == Color(0, 0, 0));
}

TEST_CASE("The reflected color for a reflective material") {
    World w = World::default_world();
    auto shape = std::make_shared<Plane>();
    shape->material.m_reflective = 0.5f;
    shape->set_transform(Matrix<4,4>::translation(0, -1, 0));
    w.add_object(shape);

    Ray r(Point(0, 0, -3), Vector(0, -std::sqrtf(2)/2, std::sqrtf(2)/2));
    Intersection i(std::sqrtf(2), *shape);

    PreCompute comps(i, r);
    Color color = w.reflected_color(comps);

    REQUIRE(color == Color(0.190332f, 0.237915f, 0.1427491f));
}

TEST_CASE("shade_hit() with a reflective material") {
    World w = World::default_world();
    auto shape = std::make_shared<Plane>();
    shape->material.m_reflective = 0.5f;
    shape->set_transform(Matrix<4,4>::translation(0, -1, 0));
    w.add_object(shape);

    Ray r(Point(0, 0, -3), Vector(0, -std::sqrtf(2)/2, std::sqrtf(2)/2));
    Intersection i(std::sqrtf(2), *shape);

    PreCompute comps(i, r);
    Color color = w.shade_hit(comps);

    REQUIRE(color == Color(0.876757f, 0.92434f, 0.82917f));
}

TEST_CASE("color_at() with mutually reflective surfaces") {
    World w = World();
    w.light = Light(Point(0, 0, 0), Color(1, 1, 1));

    auto lower = std::make_shared<Plane>("lower");
    lower->material.m_reflective = 1;
    lower->set_transform(Matrix<4,4>::translation(0, -1, 0));
    w.add_object(lower);

    auto upper = std::make_shared<Plane>("upper");
    upper->material.m_reflective = 1;
    upper->set_transform(Matrix<4,4>::translation(0, 1, 0));
    w.add_object(upper);

    Ray r(Point(0, 0, 0), Vector(0, 1, 0));
    
    REQUIRE_NOTHROW(
        [&]() {
            auto future = std::async(std::launch::async, [&]() {
                return w.color_at(r);
            });

            if (future.wait_for(std::chrono::milliseconds(200)) != std::future_status::ready) {
                throw std::runtime_error("color_at() did not terminate");
            }

            future.get();
        }()
    );
}

TEST_CASE("The reflected color at the maximum recursive depth") {
    World w = World::default_world();
    auto shape = std::make_shared<Plane>();
    shape->material.m_reflective = 0.5f;
    shape->set_transform(Matrix<4,4>::translation(0, -1, 0));
    w.add_object(shape);
    Ray r(Point(0, 0, -3), Vector(0, -std::sqrtf(2)/2, std::sqrtf(2)/2));
    Intersection i(std::sqrtf(2), *shape);
    PreCompute comps(i, r);
    Color color = w.reflected_color(comps, 0);
    REQUIRE(color == Color(0, 0, 0));
}

TEST_CASE("Transparency and Refractive Index for the default material") {
    Material m = Material();
    REQUIRE(m.m_transparency == Catch::Approx(0.0f));
    REQUIRE(m.m_refractive_index == Catch::Approx(1.0f));
}

TEST_CASE("A helper for producing a sphere with a glassy material") {
    Sphere s = Sphere::Glass_Sphere();
    REQUIRE(s.transform == Matrix<4, 4>::identity());
    REQUIRE(s.material.m_transparency == Catch::Approx(1.0f));
    REQUIRE(s.material.m_refractive_index == Catch::Approx(1.5f));
}

TEST_CASE("Finding n1 and n2 at various intersections") {
    Sphere A = Sphere::Glass_Sphere("A");
    A.material.m_refractive_index = 1.5f;
    A.set_transform(Matrix<4, 4>::scaling(2, 2, 2));
    
    Sphere B = Sphere::Glass_Sphere("B");
    B.material.m_refractive_index = 2.0f;
    B.set_transform(Matrix<4, 4>::translation(0, 0, -0.25f));

    Sphere C = Sphere::Glass_Sphere("C");
    C.material.m_refractive_index = 2.5f;
    C.set_transform(Matrix<4, 4>::translation(0, 0, 0.25f));

    Ray r(Point(0, 0, -4), Vector(0, 0, 1));
    std::array<Intersection, 8> xs{
        Intersection(2.0f, A),
        Intersection(2.75f, B),
        Intersection(3.25f, C),
        Intersection(4.75f, B),
        Intersection(5.25f, C),
        Intersection(6.0f, A)};
    uint8_t xs_count = 6;
    std::vector<PreCompute> comps;
    comps.reserve(xs_count);
    
    for (size_t i = 0; i < xs_count; ++i) {
        comps.emplace_back(xs[i], r, xs.data(), xs_count);
    }

    REQUIRE(comps[0].n1 == Catch::Approx(1.0f));
    REQUIRE(comps[0].n2 == Catch::Approx(1.5f));

    REQUIRE(comps[1].n1 == Catch::Approx(1.5f));
    REQUIRE(comps[1].n2 == Catch::Approx(2.0f));

    REQUIRE(comps[2].n1 == Catch::Approx(2.0f));
    REQUIRE(comps[2].n2 == Catch::Approx(2.5f));

    REQUIRE(comps[3].n1 == Catch::Approx(2.5f));
    REQUIRE(comps[3].n2 == Catch::Approx(2.5f));

    REQUIRE(comps[4].n1 == Catch::Approx(2.5f));
    REQUIRE(comps[4].n2 == Catch::Approx(1.5f));

    REQUIRE(comps[5].n1 == Catch::Approx(1.5f));
    REQUIRE(comps[5].n2 == Catch::Approx(1.0f));
}

TEST_CASE("The under point is offset below the surface") {
    Ray r(Point(0, 0, -5), Vector(0, 0, 1));
    Sphere shape = Sphere::Glass_Sphere();
    shape.set_transform(Matrix<4, 4>::translation(0, 0, 1));
    Intersection i(5, shape);
    std::array<Intersection, 8> xs{i};
    PreCompute comps(i, r, xs.data(), 1);

    REQUIRE(comps.under_point.data.z > (EPSILON/2.0f));
    REQUIRE(comps.point.data.z < comps.under_point.data.z);
}

TEST_CASE("The refracted color with an opaque surface") {
    World w = World::default_world();
    auto shape = w.objects[0];
    Ray r(Point(0, 0, -5), Vector(0, 0, 1));
    std::array<Intersection, 8> xs{Intersection(4.0f, *shape), Intersection(6.0f, *shape)};
    uint8_t xs_count = 2;
    PreCompute comps(xs[0], r, xs.data(), xs_count);
    Color c = w.refracted_color(comps, 5);

    REQUIRE(c == Color(0, 0, 0));
}

TEST_CASE("The refracted color at the maximum recursive depth") {
    World w = World::default_world();
    auto shape = w.objects[0];
    shape->material.m_transparency = 1.0f;
    shape->material.m_refractive_index = 1.5f;
    Ray r(Point(0, 0, -5), Vector(0, 0, 1));
    std::array<Intersection, 8> xs{Intersection(4.0f, *shape), Intersection(6.0f, *shape)};
    uint8_t xs_count = 2;
    PreCompute comps(xs[0], r, xs.data(), xs_count);
    Color c = w.refracted_color(comps, 0);

    REQUIRE(c == Color(0, 0, 0));
}

TEST_CASE("The refracted color under total internal reflection") {
    World w = World::default_world();
    auto shape = w.objects[0];
    shape->material.m_transparency = 1.0f;
    shape->material.m_refractive_index = 1.5f;
    Ray r(Point(0, 0, std::sqrtf(2)/2), Vector(0, 1, 0));
    std::array<Intersection, 8> xs{Intersection(-std::sqrtf(2)/2, *shape), 
        Intersection(std::sqrtf(2)/2, *shape)};
    uint8_t xs_count = 2;
    PreCompute comps(xs[1], r, xs.data(), xs_count);
    Color c = w.refracted_color(comps, 5);

    REQUIRE(c == Color(0, 0, 0));
}

TEST_CASE("The refracted color with a refracted ray") {
    World w = World::default_world();
    auto A = w.objects[0];
    A->material.m_ambient = 1.0f;
    A->material.pattern = Patterns::Test();

    auto B = w.objects[1];
    B->material.m_transparency = 1.0f;
    B->material.m_refractive_index = 1.5f;

    Ray r(Point(0, 0, 0.1f), Vector(0, 1, 0));
    std::array<Intersection, 8> xs{Intersection(-0.9899f, *A), 
                                   Intersection(-0.4899f, *B),
                                   Intersection(0.4899f, *B),
                                   Intersection(0.9899f, *A)};

    uint8_t xs_count = 4;
    PreCompute comps(xs[2], r, xs.data(), xs_count);
    Color c = w.refracted_color(comps, 5);

    REQUIRE(c == Color(0, 0.99887f, 0.04721f));
}