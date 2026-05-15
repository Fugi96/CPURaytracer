#include "catch2/catch_test_macros.hpp"
#include "objects/Sphere.h"
#include "objects/World.h"
#include "shading/Light.h"
#include "shading/Shading.h"
#include "util/Color.h"
#include "util/Intersection.h"
#include "util/Matrix.h"
#include "util/Point.h"
#include "util/Vector.h"
#include "util/util.h"
#include <catch2/catch_all.hpp>
#include <memory>

TEST_CASE("Lighting with the surface in shadow") {
    Material m = Material();
    Point position(0, 0, 0);

    Vector eyev(0, 0, -1);
    Vector normalv(0, 0, -1);
    Light point_light(Point(0, 0, -10), Color(1, 1, 1));
    bool in_shadow = true;
    Color result = Shading::lighting(m, Sphere(), point_light, position, eyev, normalv, in_shadow);

    REQUIRE(result == Color(0.1f, 0.1f, 0.1f));
}

TEST_CASE("There is no shadow when nothing is collinear with point and light") {
    World w = World::default_world();
    Point p(0, 10, 0);
    
    REQUIRE(w.is_shadowed(p) == false);
}

TEST_CASE("The shadow when an object is between the point and the light") {
    World w = World::default_world();
    Point p(10, -10, 10);

    REQUIRE(w.is_shadowed(p) == true);
}

TEST_CASE("There is no shadow when an object is behind the point") {
    World w = World::default_world();
    Point p(-2, 2, -2);

    REQUIRE(w.is_shadowed(p) == false);
}

TEST_CASE("shade_hit() is given an intersection in shadow") {
    World w = World();
    w.light = Light(Point(0, 0, -10), Color(1, 1, 1));
    Sphere s1 = Sphere();
    w.add_object(std::make_shared<Sphere>(s1));
    Sphere s2 = Sphere();
    s2.set_transform(Matrix<4,4>::translation(0, 0, 10));
    w.add_object(std::make_shared<Sphere>(s2));
    Ray r(Point(0, 0, 5), Vector(0, 0, 1));
    Intersection i(4, s2);
    PreCompute comps = PreCompute(i, r);
    Color c = w.shade_hit(comps);

    REQUIRE(c == Color(0.1f, 0.1f, 0.1f));
}

TEST_CASE("The hit should offset the point") {
    Ray r(Point(0, 0, -5), Vector(0, 0, 1));
    Sphere shape = Sphere();
    shape.set_transform(Matrix<4, 4>::translation(0, 0, 1));
    Intersection i(5, shape);
    PreCompute comps = PreCompute(i, r);

    REQUIRE(comps.over_point.data.z < -EPSILON/2);
    REQUIRE(comps.point.data.z > comps.over_point.data.z);
}