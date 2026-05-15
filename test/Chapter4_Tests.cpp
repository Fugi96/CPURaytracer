#include "catch2/catch_test_macros.hpp"
#include <catch2/catch_all.hpp>
#include "util/Matrix.h"
#include "util/Point.h"

TEST_CASE("Multiplying by a translation matrix", "[matrix]") {
    Matrix<4,4> transform = Matrix<4, 4>::translation(5.0f, -3.0f, 2.0f);
    Point p(-3.0f, 4.0f, 5.0f);

    REQUIRE((transform*p) == Point(2.0f, 1.0f, 7.0f));
}

TEST_CASE("Multiplying by the inverse of a translation matrix") {
    Matrix<4,4> transform = Matrix<4, 4>::translation(5.0f, -3.0f, 2.0f);
    Point p(-3.0f, 4.0f, 5.0f);
    Matrix<4,4> inv = Matrix<4,4>::inverse(transform);

    REQUIRE(inv*p == Point(-8.0f, 7.0f, 3.0f));
}

TEST_CASE("Translation does not affect vectors") {
    Matrix<4,4> transform = Matrix<4, 4>::translation(5.0f, -3.0f, 2.0f);
    Vector v(-3.0f, 4.0f, 5.0f);

    REQUIRE(transform*v == v);
}

TEST_CASE("A scaling matrix applied to a point") {
    Matrix<4,4> transform = Matrix<4, 4>::scaling(2, 3, 4);
    Point p(-4,6,8);

    REQUIRE(transform*p == Point(-8,18,32));
}

TEST_CASE("A scaling matrix applied to a vector") {
    Matrix<4,4> transform = Matrix<4, 4>::scaling(2, 3, 4);
    Vector v(-4,6,8);

    REQUIRE(transform*v == Vector(-8,18,32));
}

TEST_CASE("Multiplying by the inverse of a scaling matrix") {
    Matrix<4,4> transform = Matrix<4,4>::scaling(2,3,4);
    Matrix<4,4> inv = Matrix<4,4>::inverse(transform);
    Vector v(-4,6,8);

    REQUIRE(inv*v == Vector(-2,2,2));
}

TEST_CASE("Reflection is scaling by a negative value") {
    Matrix<4,4> transform = Matrix<4,4>::scaling(-1,1,1);
    Point p(2,3,4);

    REQUIRE(transform*p == Point(-2,3,4));
}

TEST_CASE("Rotating a point around the x axis") {
    Point p(0,1,0);
    Matrix<4,4> half_quarter = Matrix<4,4>::rotation_x(PI/4);
    Matrix<4,4> full_quarter = Matrix<4,4>::rotation_x(PI/2);

    REQUIRE(half_quarter*p == Point(0, std::sqrtf(2)/2, std::sqrtf(2)/2));
    REQUIRE(full_quarter*p == Point(0,0,1));
}

TEST_CASE("The inverse of an x-rotation rotates in the opposite direction") {
    Point p(0,1,0);
    Matrix<4,4> half_quarter = Matrix<4,4>::rotation_x(PI/4);
    Matrix<4,4> inv = Matrix<4,4>::inverse(half_quarter);

    REQUIRE(inv*p == Point(0, std::sqrtf(2)/2, -std::sqrtf(2)/2));
}

TEST_CASE("Rotating a point around the y axis") {
    Point p(0,0,1);
    Matrix<4,4> half_quarter = Matrix<4,4>::rotation_y(PI/4);
    Matrix<4,4> full_quarter = Matrix<4,4>::rotation_y(PI/2);

    REQUIRE(half_quarter*p == Point(std::sqrtf(2)/2, 0, std::sqrtf(2)/2));
    REQUIRE(full_quarter*p == Point(1,0,0));
}

TEST_CASE("Rotating a point around the z axis") {
    Point p(0,1,0);
    Matrix<4,4> half_quarter = Matrix<4,4>::rotation_z(PI/4);
    Matrix<4,4> full_quarter = Matrix<4,4>::rotation_z(PI/2);

    REQUIRE(half_quarter*p == Point(-std::sqrtf(2)/2, std::sqrtf(2)/2, 0));
    REQUIRE(full_quarter*p == Point(-1,0,0));
}

TEST_CASE("A shearing transformation moves x in proportion to y") {
    Matrix<4,4> transform = Matrix<4,4>::shearing(1, 0, 0, 0, 0, 0);
    Point p(2,3,4);

    REQUIRE(transform*p == Point(5,3,4));
}

TEST_CASE("A shearing transformation moves x in proportion to z") {
    Matrix<4,4> transform = Matrix<4,4>::shearing(0, 1, 0, 0, 0, 0);
    Point p(2,3,4);

    REQUIRE(transform*p == Point(6,3,4));
}

TEST_CASE("A shearing transformation moves y in proportion to x") {
    Matrix<4,4> transform = Matrix<4,4>::shearing(0, 0, 1, 0, 0, 0);
    Point p(2,3,4);

    REQUIRE(transform*p == Point(2,5,4));
}

TEST_CASE("A shearing transformation moves y in proportion to z") {
    Matrix<4,4> transform = Matrix<4,4>::shearing(0, 0, 0, 1, 0, 0);
    Point p(2,3,4);

    REQUIRE(transform*p == Point(2,7,4));
}

TEST_CASE("A shearing transformation moves z in proportion to x") {
    Matrix<4,4> transform = Matrix<4,4>::shearing(0, 0, 0, 0, 1, 0);
    Point p(2,3,4);

    REQUIRE(transform*p == Point(2,3,6));
}

TEST_CASE("A shearing transformation moves z in proportion to y") {
    Matrix<4,4> transform = Matrix<4,4>::shearing(0, 0, 0, 0, 0, 1);
    Point p(2,3,4);

    REQUIRE(transform*p == Point(2,3,7));
}

TEST_CASE("Individual transformations are applied in sequence") {
    Point p(1,0,1);
    Matrix<4,4> A = Matrix<4,4>::rotation_x(PI/2);
    Matrix<4,4> B = Matrix<4,4>::scaling(5, 5, 5);
    Matrix<4,4> C = Matrix<4,4>::translation(10, 5, 7);

    Point p2(A*p);
    REQUIRE(p2 == Point(1, -1, 0));

    Point p3(B*p2);
    REQUIRE(p3 == Point(5, -5, 0));

    Point p4(C*p3);
    REQUIRE(p4 == Point(15, 0, 7));    
}

TEST_CASE("Chained transformations must be applied in reverse order") {
    Point p(1, 0, 1);
    Matrix<4,4> A = Matrix<4,4>::rotation_x(PI/2);
    Matrix<4,4> B = Matrix<4,4>::scaling(5, 5, 5);
    Matrix<4,4> C = Matrix<4,4>::translation(10, 5, 7);

    Matrix<4,4> T = C*B*A;

    REQUIRE(T*p == Point(15, 0, 7));
}