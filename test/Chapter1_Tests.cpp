#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "util/Point.h"
#include "util/Vector.h"
#include "util/Tuple.h"
#include <catch2/catch_all.hpp>

TEST_CASE("A tuple with w=1.0 is a point") {
    Tuple a(4.3f, -4.2f, 3.1f, 1.0f);

    REQUIRE(a.x() == Catch::Approx(4.3f));
    REQUIRE(a.y() == Catch::Approx(-4.2f));
    REQUIRE(a.z() == Catch::Approx(3.1f));
    REQUIRE(a.w() == Catch::Approx(1.0f));

    // Kind of redundant since Tuple is a different class than Point/Vector,
    // only here for sake of completeness of the books covered tests.
    REQUIRE(a.w() == Catch::Approx(1.0f)); // is a point
    REQUIRE(a.w() != Catch::Approx(0.0f)); // is not a vector
}

TEST_CASE("A tuple with w=0.0 is a vector") {
    Tuple a(4.3f, -4.2f, 3.1f, 0.0f);

    REQUIRE(a.x() == Catch::Approx(4.3f));
    REQUIRE(a.y() == Catch::Approx(-4.2f));
    REQUIRE(a.z() == Catch::Approx(3.1f));
    REQUIRE(a.w() == Catch::Approx(0.0f));

    // Kind of redundant since Tuple is a different class than Point/Vector,
    // only here for sake of completeness of the books covered tests.
    REQUIRE(a.w() != Catch::Approx(1.0f)); // is not a point
    REQUIRE(a.w() == Catch::Approx(0.0f)); // is a vector
}

TEST_CASE("Point() creates tuples with w=1") {
    Point p(4.0f, -4.0f, 3.0f);

    REQUIRE(p == Tuple(4.0f, -4.0f, 3.0f, 1.0f));
}

TEST_CASE("Vector() creates tuples with w=0") {
    Vector v(4.0f, -4.0f, 3.0f);

    REQUIRE(v == Tuple(4.0f, -4.0f, 3.0f, 0.0f));
}

TEST_CASE("Adding two tuples") {
    Tuple a1(3.0f, -2.0f, 5.0f, 1.0f);
    Tuple a2(-2.0f, 3.0f, 1.0f, 0.0f);

    REQUIRE((a1 + a2) == Tuple(1.0f, 1.0f, 6.0f, 1.0f));
}

TEST_CASE("Subtracting two points") {

    Point p1(3.0f, 2.0f, 1.0f);
    Point p2(5.0f, 6.0f, 7.0f);

    REQUIRE((p1-p2) == Vector(-2.0f, -4.0f, -6.0f));
}

TEST_CASE("Subtracting a vector from a point") {
    Point p(3.0f, 2.0f, 1.0f);
    Vector v(5.0f, 6.0f, 7.0f);

    REQUIRE((p-v) == Point(-2.0f, -4.0f, -6.0f));
}

TEST_CASE("Subtracting two vectors") {
    Vector v1(3.0f, 2.0f, 1.0f);
    Vector v2(5.0f, 6.0f, 7.0f);

    REQUIRE((v1-v2) == Vector(-2.0f, -4.0f, -6.0f));
}

TEST_CASE("Subtracting a vector from the zero vector") {
    Vector zero(0.0f, 0.0f, 0.0f);
    Vector v(1.0f, -2.0f, 3.0f);

    REQUIRE((zero-v) == Vector(-1.0f, 2.0f, -3.0f));
}

TEST_CASE("Negating a tuple") {
    Tuple a(1.0f, -2.0f, 3.0f, -4.0f);

    REQUIRE(-a == Tuple(-1.0f, 2.0f, -3.0f, 4.0f));
}

TEST_CASE("Multiplying a tuple by a scalar") {
    Tuple a(1.0f, -2.0f, 3.0f, -4.0f);

    REQUIRE((a*3.5f) == Tuple(3.5f, -7.0f, 10.5f, -14.0f));
}

TEST_CASE("Multiplying a tuple by a fraction") {
    Tuple a(1.0f, -2.0f, 3.0f, -4.0f);

    REQUIRE((a*0.5f) == Tuple(0.5f, -1.0f, 1.5f, -2.0f));
}

TEST_CASE("Computing the magnitude of vector(1, 0, 0)") {
    Vector v(1.0f, 0.0f, 0.0f);

    REQUIRE(Vector::magnitude(v) == Catch::Approx(1.0f));
}

TEST_CASE("Computing the magnitude of vector(0, 1, 0)") {
    Vector v(0.0f, 1.0f, 0.0f);

    REQUIRE(Vector::magnitude(v) == Catch::Approx(1.0f));
}

TEST_CASE("Computing the magnitude of vector(0, 0, 1)") {
    Vector v(0.0f, 0.0f, 1.0f);

    REQUIRE(Vector::magnitude(v) == Catch::Approx(1.0f));
}

TEST_CASE("Computing the magnitude of vector(1, 2, 3)") {
    Vector v(1.0f, 2.0f, 3.0f);

    REQUIRE(Vector::magnitude(v) == Catch::Approx(std::sqrt(14.0f)));
}

TEST_CASE("Computing the magnitude of vector(-1, -2, -3)") {
    Vector v(-1.0f, -2.0f, -3.0f);

    REQUIRE(Vector::magnitude(v) == Catch::Approx(std::sqrt(14.0f)));
}

TEST_CASE("Normalizing vector(4, 0, 0) gives (1, 0, 0)") {
    Vector v(4.0f, 0.0f, 0.0f);

    REQUIRE(Vector::normalize(v) == Vector(1.0f, 0.0f, 0.0f));
}

TEST_CASE("Normalizing vector(1, 2, 3)") {
    Vector v(1.0f, 2.0f, 3.0f);

    Vector norm = Vector::normalize(v);

    REQUIRE(norm.x() == Catch::Approx(1.0f / std::sqrt(14.0f)));
    REQUIRE(norm.y() == Catch::Approx(2.0f / std::sqrt(14.0f)));
    REQUIRE(norm.z() == Catch::Approx(3.0f / std::sqrt(14.0f)));
    REQUIRE(norm.w() == Catch::Approx(0.0f));
}

TEST_CASE("The dot pruduct of two tuples") {
    Vector a(1.0f, 2.0f, 3.0f);
    Vector b(2.0f, 3.0f, 4.0f);

    REQUIRE(Vector::dot(a, b) == Catch::Approx(20.0f));
}

TEST_CASE("The cross product of two vectors") {
    Vector a(1.0f, 2.0f, 3.0f);
    Vector b(2.0f, 3.0f, 4.0f);

    REQUIRE(Vector::cross(a, b) == Vector(-1.0f, 2.0f, -1.0f));
    REQUIRE(Vector::cross(b, a) == Vector(1.0f, -2.0f, 1.0f));
}