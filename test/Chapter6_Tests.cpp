#include "objects/Sphere.h"
#include "shading/Light.h"
#include "shading/Shading.h"
#include <catch2/catch_all.hpp>
#include <cmath>

TEST_CASE("The normal on a sphere at a point on the x axis", "[normal]")
{
    Sphere s;

    Vector n = s.normal_at(Point(1, 0, 0));

    REQUIRE(n == Vector(1, 0, 0));
}

TEST_CASE("The normal on a sphere at a point on the y axis", "[sphere][normal]")
{
    Sphere s;

    Vector n = s.normal_at(Point(0, 1, 0));

    REQUIRE(n == Vector(0, 1, 0));
}

TEST_CASE("The normal on a sphere at a point on the z axis", "[sphere][normal]")
{
    Sphere s;

    Vector n = s.normal_at(Point(0, 0, 1));

    REQUIRE(n == Vector(0, 0, 1));
}

TEST_CASE("The normal on a sphere at a nonaxial point", "[sphere][normal]")
{
    Sphere s;

    const float v = std::sqrtf(3.0f) / 3.0f;

    Vector n = s.normal_at(Point(v, v, v));

    REQUIRE(n == Vector(v, v, v));
}

TEST_CASE("The normal is a normalized vector", "[sphere][normal]")
{
    Sphere s;

    const float v = std::sqrtf(3.0f) / 3.0f;

    Vector n = s.normal_at(Point(v, v, v));

    REQUIRE(Vector::normalize(n) == n);
}

TEST_CASE("Computing the normal on a translated sphere", "[sphere][normal][transform]")
{
    Sphere s;
    s.set_transform(Matrix<4,4>::translation(0, 1, 0));

    Vector n = s.normal_at(Point(0, 1.70711f, -0.70711f));

    REQUIRE(n == Vector(0, 0.70711f, -0.70711f));
}

TEST_CASE("Computing the normal on a transformed sphere", "[sphere][normal][transform]")
{
    Sphere s;

    Matrix<4,4> m =
        Matrix<4,4>::scaling(1, 0.5f, 1) *
        Matrix<4,4>::rotation_z(PI / 5.0f);

    s.set_transform(m);

    const float v = std::sqrtf(2.0f) / 2.0f;

    Vector n = s.normal_at(Point(0, v, -v));

    REQUIRE(n == Vector(0, 0.97014f, -0.24254f));
}

TEST_CASE("Reflecting a vector approaching at 45°", "[vector][reflect]")
{
    Vector v(1, -1, 0);
    Vector n(0, 1, 0);

    Vector r = Vector::reflect(v, n);

    REQUIRE(r == Vector(1, 1, 0));
}

TEST_CASE("Reflecting a vector off a slanted surface", "[vector][reflect]")
{
    Vector v(0, -1, 0);

    const float s = std::sqrtf(2.0f) / 2.0f;
    Vector n(s, s, 0);

    Vector r = Vector::reflect(v, n);

    REQUIRE(r == Vector(1, 0, 0));
}

TEST_CASE("A point light has a position and intensity", "[light]")
{
    Color intensity(1, 1, 1);
    Point position(0, 0, 0);

    Light light(position, intensity);

    REQUIRE(light.m_position == position);
    REQUIRE(light.m_intensity == intensity);
}

TEST_CASE("The default material", "[material]")
{
    Material m;

    REQUIRE(m.m_color == Color(1, 1, 1));
    REQUIRE(equal(m.m_ambient, 0.1f));
    REQUIRE(equal(m.m_diffuse, 0.9f));
    REQUIRE(equal(m.m_specular, 0.9f));
    REQUIRE(equal(m.m_shininess, 200.0f));
}

TEST_CASE("A sphere has a default material", "[sphere][material]")
{
    Sphere s;

    Material m = s.material;

    REQUIRE(m == Material());
}

TEST_CASE("A sphere may be assigned a material", "[sphere][material]")
{
    Sphere s;
    Material m;
    m.m_ambient = 1.0f;

    s.set_material(m);

    REQUIRE(s.material == m);
}

TEST_CASE("Lighting with the eye between the light and the surface", "[lighting]")
{
    Material m;
    Point position(0, 0, 0);

    Vector eyev(0, 0, -1);
    Vector normalv(0, 0, -1);
    Light light(Point(0, 0, -10), Color(1, 1, 1));

    Color result = Shading::lighting(m, Sphere(), light, position, eyev, normalv, false);

    REQUIRE(result == Color(1.9f, 1.9f, 1.9f));
}

TEST_CASE("Lighting with the eye between light and surface, eye offset 45°", "[lighting]")
{
    Material m;
    Point position(0, 0, 0);

    const float s = std::sqrtf(2.0f) / 2.0f;

    Vector eyev(0, s, -s);
    Vector normalv(0, 0, -1);
    Light light(Point(0, 0, -10), Color(1, 1, 1));

    Color result = Shading::lighting(m, Sphere(), light, position, eyev, normalv, false);

    REQUIRE(result == Color(1.0f, 1.0f, 1.0f));
}

TEST_CASE("Lighting with eye opposite surface, light offset 45°", "[lighting]")
{
    Material m;
    Point position(0, 0, 0);

    Vector eyev(0, 0, -1);
    Vector normalv(0, 0, -1);
    Light light(Point(0, 10, -10), Color(1, 1, 1));

    Color result = Shading::lighting(m, Sphere(), light, position, eyev, normalv, false);

    REQUIRE(result == Color(0.7364f, 0.7364f, 0.7364f));
}

TEST_CASE("Lighting with eye in the path of the reflection vector", "[lighting]")
{
    Material m;
    Point position(0, 0, 0);

    const float s = std::sqrt(2.0f) / 2.0f;

    Vector eyev(0, -s, -s);
    Vector normalv(0, 0, -1);
    Light light(Point(0, 10, -10), Color(1, 1, 1));

    Color result = Shading::lighting(m, Sphere(), light, position, eyev, normalv, false);

    REQUIRE(result == Color(1.6364f, 1.6364f, 1.6364f));
}

TEST_CASE("Lighting with the light behind the surface", "[lighting]")
{
    Material m;
    Point position(0, 0, 0);

    Vector eyev(0, 0, -1);
    Vector normalv(0, 0, -1);
    Light light(Point(0, 0, 10), Color(1, 1, 1));

    Color result = Shading::lighting(m, Sphere(), light, position, eyev, normalv, false);

    REQUIRE(result == Color(0.1f, 0.1f, 0.1f));
}