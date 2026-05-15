#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "objects/Sphere.h"
#include "util/Intersection.h"
#include "util/Point.h"
#include "util/Ray.h"
#include "util/Vector.h"
#include <catch2/catch_all.hpp>
#include <cstdint>

TEST_CASE("Creating and querying a ray", "[ray]") {
    Point origin(1, 2, 3);
    Vector direction(4, 5, 6);

    Ray r(origin, direction);

    REQUIRE(r.m_origin == origin);
    REQUIRE(r.m_direction == direction);
}

TEST_CASE("Computing a point from a distance", "[ray]") {
    Ray r(Point(2, 3, 4), Vector(1, 0, 0));

    REQUIRE(r.position(0) == Point(2, 3, 4));
    REQUIRE(r.position(1) == Point(3, 3, 4));
    REQUIRE(r.position(-1) == Point(1, 3, 4));
    REQUIRE(r.position(2.5f) == Point(4.5f, 3, 4));
}

TEST_CASE("A ray intersects a sphere at two points") {
    Ray r(Point(0, 0, -5), Vector(0, 0, 1));
    Sphere s = Sphere();

    std::array<Intersection, 8> xs;
    uint8_t xs_count = 0;

    s.intersect(r, xs, xs_count);
    REQUIRE(xs_count == 2);
    REQUIRE(xs[0].t == Catch::Approx(4.0f));
    REQUIRE(xs[1].t == Catch::Approx(6.0f));
}

TEST_CASE("A ray intersects a sphere at a tangent") {
    Ray r(Point(0, 1, -5), Vector(0, 0, 1));
    Sphere s = Sphere();

    std::array<Intersection, 8> xs;
    uint8_t xs_count = 0;

    s.intersect(r, xs, xs_count);
    REQUIRE(xs_count == 2);
    REQUIRE(xs[0].t == Catch::Approx(5.0f));
    REQUIRE(xs[1].t == Catch::Approx(5.0f));
}

TEST_CASE("A ray misses a sphere") {
    Ray r(Point(0, 2, -5), Vector(0, 0, 1));
    Sphere s = Sphere();

    std::array<Intersection, 8> xs;
    uint8_t xs_count = 0;

    s.intersect(r, xs, xs_count);
    REQUIRE(xs_count == 0);
}

TEST_CASE("A ray originates inside a sphere") {
    Ray r(Point(0, 0, 0), Vector(0, 0, 1));
    Sphere s = Sphere();

    std::array<Intersection, 8> xs;
    uint8_t xs_count = 0;

    s.intersect(r, xs, xs_count);
    REQUIRE(xs_count == 2);
    REQUIRE(xs[0].t == Catch::Approx(-1.0f));
    REQUIRE(xs[1].t == Catch::Approx(1.0f));
}

TEST_CASE("A sphere is behind a ray") {
    Ray r(Point(0, 0, 5), Vector(0, 0, 1));
    Sphere s = Sphere();

    std::array<Intersection, 8> xs;
    uint8_t xs_count = 0;

    s.intersect(r, xs, xs_count);
    REQUIRE(xs_count == 2);
    REQUIRE(xs[0].t == Catch::Approx(-6.0f));
    REQUIRE(xs[1].t == Catch::Approx(-4.0f));
}

TEST_CASE("An intersection encapsulates t and object") {
    Sphere s = Sphere();
    Intersection i(3.5f, s);

    REQUIRE(i.t == Catch::Approx(3.5f));
    REQUIRE(i.m_object == &s);
}

TEST_CASE("Aggregating intersections") {
    Sphere s = Sphere();
    Intersection i1(1, s);
    Intersection i2(2, s);

    // Kinda redundant tests due to my code architecture
    std::array<Intersection, 8> xs{i1, i2};
    uint8_t xs_count = 2;

    REQUIRE(xs[0].t == Catch::Approx(1.0f));
    REQUIRE(xs[1].t == Catch::Approx(2.0f));
    REQUIRE(xs_count == 2);
}

TEST_CASE("Intersect sets the object on the intersection") {
    Sphere s = Sphere();
    Ray r(Point(0, 0, -5), Vector(0, 0, 1));

    // Kinda redundant tests due to my code architecture
    std::array<Intersection, 8> xs;
    uint8_t xs_count = 0;

    s.intersect(r, xs, xs_count);

    REQUIRE(xs_count == 2);
    REQUIRE(xs[0].m_object == &s);
    REQUIRE(xs[1].m_object == &s);
}

TEST_CASE("The hit when all intersections have positive t") {
    Sphere s = Sphere();
    Intersection i1(1, s);
    Intersection i2(2, s);

    // Kinda redundant tests due to my code architecture
    std::array<Intersection, 8> xs{i2, i1};
    uint8_t xs_count = 2;

    Intersection::sort_by_t(xs.data(), xs_count);

    Intersection i;
    Intersection::hit(xs.data(), xs_count, i);

    REQUIRE(i == i1);
}

TEST_CASE("The hit when some intersections have negative t") {
    Sphere s = Sphere();
    Intersection i1(-1, s);
    Intersection i2(1, s);

    // Kinda redundant tests due to my code architecture
    std::array<Intersection, 8> xs{i2, i1};
    uint8_t xs_count = 2;

    Intersection::sort_by_t(xs.data(), xs_count);

    Intersection i;
    Intersection::hit(xs.data(), xs_count, i);

    REQUIRE(i == i2);
}

TEST_CASE("The hit when all intersections have negative t") {
    Sphere s = Sphere();
    Intersection i1(-2, s);
    Intersection i2(-1, s);

    // Kinda redundant tests due to my code architecture
    std::array<Intersection, 8> xs{i2, i1};
    uint8_t xs_count = 2;

    Intersection::sort_by_t(xs.data(), xs_count);

    Intersection i;
    Intersection::hit(xs.data(), xs_count, i);

    REQUIRE(i.m_object == nullptr);
}

TEST_CASE("The hit is always the lowest nonnegative intersection") {
    Sphere s = Sphere();
    Intersection i1(5, s);
    Intersection i2(7, s);
    Intersection i3(-3, s);
    Intersection i4(2, s);

    // Kinda redundant tests due to my code architecture
    std::array<Intersection, 8> xs{i1, i2, i3, i4};
    uint8_t xs_count = 4;

    Intersection::sort_by_t(xs.data(), xs_count);

    Intersection i;
    Intersection::hit(xs.data(), xs_count, i);

    REQUIRE(i == i4);
}

TEST_CASE("Translating a ray", "[ray]") {
    Ray r(Point(1, 2, 3), Vector(0, 1, 0));
    Matrix<4,4> m = Matrix<4,4>::translation(3, 4, 5);
    Ray r2(Ray::transform(r, m));

    REQUIRE(r2.m_origin == Point(4, 6 ,8));
    REQUIRE(r2.m_direction == Vector(0, 1, 0));
}

TEST_CASE("Scaling a ray", "[ray]") {
    Ray r(Point(1, 2, 3), Vector(0, 1, 0));
    Matrix<4,4> m = Matrix<4,4>::scaling(2, 3, 4);
    Ray r2(Ray::transform(r, m));

    REQUIRE(r2.m_origin == Point(2, 6 ,12));
    REQUIRE(r2.m_direction == Vector(0, 3, 0));
}

TEST_CASE("A sphere's default transformation") {
    Sphere s = Sphere();

    REQUIRE(s.transform == Matrix<4,4>::identity());
}

TEST_CASE("Changing a sphere's transformation") {
    Sphere s = Sphere();
    Matrix<4,4> t = Matrix<4,4>::translation(2, 3, 4);
    s.set_transform(t);

    REQUIRE(s.transform == t);
}

TEST_CASE("Intersecting a scaled sphere with a ray") {
    Ray r(Point(0, 0, -5), Vector(0, 0, 1));
    Sphere s = Sphere();

    s.set_transform(Matrix<4,4>::scaling(2, 2, 2));

    std::array<Intersection, 8> xs;
    uint8_t xs_count = 0;

    s.intersect(r, xs, xs_count);
    REQUIRE(xs_count == 2);
    REQUIRE(xs[0].t == Catch::Approx(3.0f));
    REQUIRE(xs[1].t == Catch::Approx(7.0f));
}

TEST_CASE("Intersecting a translated sphere with a ray") {
    Ray r(Point(0, 0, 5), Vector(0, 0, 1));
    Sphere s = Sphere();

    s.set_transform(Matrix<4,4>::translation(5, 0, 0));

    std::array<Intersection, 8> xs;
    uint8_t xs_count = 0;

    s.intersect(r, xs, xs_count);
    REQUIRE(xs_count == 0);
}