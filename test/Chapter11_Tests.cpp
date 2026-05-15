#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "objects/Material.h"
#include "objects/World.h"
#include "shading/Light.h"
#include "util/Intersection.h"
#include "util/Vector.h"
#include <catch2/catch_all.hpp>
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

