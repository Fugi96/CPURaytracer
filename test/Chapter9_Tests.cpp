#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "objects/Material.h"
#include "objects/Plane.h"
#include "objects/Sphere.h"
#include "util/Intersection.h"
#include "util/Matrix.h"
#include "util/Point.h"
#include "util/Vector.h"
#include <array>
#include <catch2/catch_all.hpp>
#include <cstdint>

TEST_CASE("The default transformation") {
    Sphere s = Sphere();
    Plane p = Plane();

    REQUIRE(s.transform == Matrix<4, 4>::identity());
    REQUIRE(p.transform == Matrix<4, 4>::identity());
}

TEST_CASE("Assigning a transformation") {
    Sphere s = Sphere();
    Plane p = Plane();

    Matrix<4, 4> transform = Matrix<4, 4>::translation(2, 3, 4);
    s.set_transform(transform);
    p.set_transform(transform);

    REQUIRE(s.transform == transform);
    REQUIRE(p.transform == transform);
}

TEST_CASE("The default material") {
    Sphere s = Sphere();
    Plane p = Plane();

    REQUIRE(s.material == Material());
    REQUIRE(p.material == Material());
}

TEST_CASE("Assigning a material") {
    Sphere s = Sphere();
    Plane p = Plane();
    Material m = Material();
    m.m_ambient = 1;
    
    s.material = m;
    p.material = m;

    REQUIRE(s.material.m_ambient == Catch::Approx(1.0f));
    REQUIRE(p.material.m_ambient == Catch::Approx(1.0f));
}

TEST_CASE("The normal of a plane is a constant everywhere") {
    Plane p = Plane();
    Vector n1 = p.local_normal_at(Point(0, 0, 0));
    Vector n2 = p.local_normal_at(Point(10, 0, -10));
    Vector n3 = p.local_normal_at(Point(-5, 0, 150));

    REQUIRE(n1 == Vector(0, 1, 0));
    REQUIRE(n2 == Vector(0, 1, 0));
    REQUIRE(n3 == Vector(0, 1, 0));
}

TEST_CASE("Intersect with a ray parallel to the plane") {
    Plane p = Plane();
    Ray r(Point(0, 10, 0), Vector(0, 0, 1));
    std::array<Intersection, 8> xs;
    uint8_t xs_count = 0;
    p.local_intersect(r, xs, xs_count);

    REQUIRE(xs_count == 0);
}

TEST_CASE("Intersecting with a coplanar ray") {
    Plane p = Plane();
    Ray r(Point(0, 0, 0), Vector(0, 0, 1));
    std::array<Intersection, 8> xs;
    uint8_t xs_count = 0;
    p.local_intersect(r, xs, xs_count);

    REQUIRE(xs_count == 0);
}

TEST_CASE("A ray intersecting a plane from above") {
    Plane p = Plane();
    Ray r(Point(0, 1, 0), Vector(0, -1, 0));
    std::array<Intersection, 8> xs;
    uint8_t xs_count = 0;
    p.local_intersect(r, xs, xs_count);

    REQUIRE(xs_count == 1);
    REQUIRE(xs[0].t == Catch::Approx(1.0f));
    REQUIRE(xs[0].m_object == &p);
}

TEST_CASE("A ray intersecting a plane from below") {
    Plane p = Plane();
    Ray r(Point(0, -1, 0), Vector(0, 1, 0));
    std::array<Intersection, 8> xs;
    uint8_t xs_count = 0;
    p.local_intersect(r, xs, xs_count);

    REQUIRE(xs_count == 1);
    REQUIRE(xs[0].t == Catch::Approx(1.0f));
    REQUIRE(xs[0].m_object == &p);
}