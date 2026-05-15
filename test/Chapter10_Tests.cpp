#include <catch2/catch_all.hpp>
#include "catch2/catch_test_macros.hpp"
#include "objects/Material.h"
#include "objects/Pattern.h"
#include "objects/Sphere.h"
#include "shading/Light.h"
#include "shading/Shading.h"
#include "util/Color.h"
#include "util/Matrix.h"
#include "util/Vector.h"

const Color WHITE = Color(1, 1,1);
const Color BLACK = Color(0, 0, 0);


TEST_CASE("Creating a stripe pattern") {
    Pattern p = Patterns::Stripe(WHITE, BLACK);

    REQUIRE(std::get<StripePattern>(p.type).a == Color(1, 1, 1));
    REQUIRE(std::get<StripePattern>(p.type).b == Color(0, 0, 0));
}


TEST_CASE("A stripe pattern is constant in y") {
    Pattern p = Patterns::Stripe(WHITE, BLACK);

    REQUIRE(Patterns::pattern_at(p, Point(0, 0, 0)) == WHITE);
    REQUIRE(Patterns::pattern_at(p, Point(0, 1, 0)) == WHITE);
    REQUIRE(Patterns::pattern_at(p, Point(0, 2, 0)) == WHITE);
}

TEST_CASE("A stripe pattern is constant in z") {
    Pattern p = Patterns::Stripe(WHITE, BLACK);

    REQUIRE(Patterns::pattern_at(p, Point(0, 0, 0)) == WHITE);
    REQUIRE(Patterns::pattern_at(p, Point(0, 0, 1)) == WHITE);
    REQUIRE(Patterns::pattern_at(p, Point(0, 0, 2)) == WHITE);
}

TEST_CASE("A stripe pattern alternates in x") {
    Pattern p = Patterns::Stripe(WHITE, BLACK);

    REQUIRE(Patterns::pattern_at(p, Point(0, 0, 0)) == WHITE);
    REQUIRE(Patterns::pattern_at(p, Point(0.9f, 0, 0)) == WHITE);
    REQUIRE(Patterns::pattern_at(p, Point(1, 0, 0)) == BLACK);
    REQUIRE(Patterns::pattern_at(p, Point(-0.1f, 0, 0)) == BLACK);
    REQUIRE(Patterns::pattern_at(p, Point(-1, 0, 0)) == BLACK);
    REQUIRE(Patterns::pattern_at(p, Point(-1.1f, 0, 0)) == WHITE);
}

TEST_CASE("Lighting with a pattern applied") {
    Material m = Material();
    m.pattern = Patterns::Stripe(WHITE, BLACK);
    m.m_ambient = 1;
    m.m_diffuse = 0;
    m.m_specular = 0;
    Vector eyev(0, 0, -1);
    Vector normalv(0, 0, -1);
    Light point_light(Point(0, 0, -10), Color(1, 1,1));
    Sphere s = Sphere();
    s.set_material(m);

    Color c1 = Shading::lighting(m, s, point_light, Point(0.9f, 0, 0), eyev, normalv, false);
    Color c2 = Shading::lighting(m, s, point_light, Point(1.1f, 0, 0), eyev, normalv, false);

    REQUIRE(c1 == WHITE);
    REQUIRE(c2 == BLACK);    
}

TEST_CASE("Stripes with an object transformation") {
    Sphere object = Sphere();
    object.set_transform(Matrix<4, 4>::scaling(2, 2, 2));
    Pattern pattern = Patterns::Stripe(WHITE, BLACK);
}

TEST_CASE("A gradient linearly interpolates between colors") {
    Pattern pattern = Patterns::Gradient(WHITE, BLACK);

    REQUIRE(Patterns::pattern_at(pattern, Point(0, 0, 0)) == WHITE);
    REQUIRE(Patterns::pattern_at(pattern, Point(0.25f, 0, 0)) == Color(0.75f, 0.75f, 0.75f));
    REQUIRE(Patterns::pattern_at(pattern, Point(0.5f, 0, 0)) == Color(0.5f, 0.5f, 0.5f));
    REQUIRE(Patterns::pattern_at(pattern, Point(0.75f, 0, 0)) == Color(0.25f, 0.25f, 0.25f));
}

TEST_CASE("A ring should extend in both x and z") {
    Pattern pattern = Patterns::Ring(WHITE, BLACK);

    REQUIRE(Patterns::pattern_at(pattern, Point(0, 0, 0)) == WHITE);
    REQUIRE(Patterns::pattern_at(pattern, Point(1, 0, 0)) == BLACK);
    REQUIRE(Patterns::pattern_at(pattern, Point(0, 0, 1)) == BLACK);
    REQUIRE(Patterns::pattern_at(pattern, Point(0.708f, 0, 0.708f)) == BLACK);
}

TEST_CASE("Checkers should repeat in x") {
    Pattern pattern = Patterns::Checkers(WHITE, BLACK);

    REQUIRE(Patterns::pattern_at(pattern, Point(0, 0, 0)) == WHITE);
    REQUIRE(Patterns::pattern_at(pattern, Point(0.99f, 0, 0)) == WHITE);
    REQUIRE(Patterns::pattern_at(pattern, Point(1.01f, 0, 0)) == BLACK);
}

TEST_CASE("Checkers should repeat in y") {
    Pattern pattern = Patterns::Checkers(WHITE, BLACK);

    REQUIRE(Patterns::pattern_at(pattern, Point(0, 0, 0)) == WHITE);
    REQUIRE(Patterns::pattern_at(pattern, Point(0, 0.99f, 0)) == WHITE);
    REQUIRE(Patterns::pattern_at(pattern, Point(0, 1.01f, 0)) == BLACK);
}

TEST_CASE("Checkers should repeat in z") {
    Pattern pattern = Patterns::Checkers(WHITE, BLACK);

    REQUIRE(Patterns::pattern_at(pattern, Point(0, 0, 0)) == WHITE);
    REQUIRE(Patterns::pattern_at(pattern, Point(0, 0, 0.99f)) == WHITE);
    REQUIRE(Patterns::pattern_at(pattern, Point(0, 0, 1.01f)) == BLACK);
}