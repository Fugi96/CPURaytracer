#include "Canvas.h"
#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "objects/Camera.h"
#include "objects/Material.h"
#include "objects/Sphere.h"
#include "objects/World.h"
#include "shading/Light.h"
#include "util/Color.h"
#include "util/Intersection.h"
#include "util/Matrix.h"
#include "util/Point.h"
#include "util/Vector.h"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <memory>

TEST_CASE("The default world") {
    Light light(Point(-10, 10, -10), Color(1, 1,1 ));
    Sphere s1 = Sphere();

    Material m = Material();
    m.m_color = Color(0.8f, 1.0f, 0.6f);
    m.m_diffuse = 0.7f;
    m.m_specular = 0.2f;

    s1.set_material(m);

    Sphere s2 = Sphere();
    s2.set_transform(Matrix<4, 4>::scaling(0.5f, 0.5f, 0.5f));

    World w = World::default_world();
    REQUIRE(w.light == light);
    REQUIRE(*w.objects[0] == s1);
    REQUIRE(*w.objects[1] == s2);
}

TEST_CASE("Intersect a world with a ray") {
    World w = World::default_world();
    Ray r(Point(0, 0, -5), Vector(0, 0, 1));

    std::array<Intersection, 8> xs;
    uint8_t xs_count = 0;
    w.direct_intersect(r, xs, xs_count);

    Intersection::sort_by_t(xs.data(), xs_count);

    REQUIRE(xs_count == 4);
    REQUIRE(xs[0].t == Catch::Approx(4.0f));
    REQUIRE(xs[1].t == Catch::Approx(4.5f));
    REQUIRE(xs[2].t == Catch::Approx(5.5f));
    REQUIRE(xs[3].t == Catch::Approx(6.0f));
}

TEST_CASE("Precompute the state of an intersection") {
    Ray r(Point(0, 0, -5), Vector(0, 0, 1));
    Sphere shape = Sphere();
    Intersection i(4, shape);

    PreCompute comps(i, r);

    REQUIRE(comps.t == i.t);
    REQUIRE(comps.m_object == &shape);
    REQUIRE(comps.point == Point(0, 0, -1));
    REQUIRE(comps.eyev == Vector(0, 0, -1));
    REQUIRE(comps.normalv == Vector(0, 0, -1));
}

TEST_CASE("The hit, when an intersection occurs on the outside") {
    Ray r(Point(0, 0, -5), Vector(0, 0, 1));
    Sphere shape = Sphere();
    Intersection i(4, shape);
    PreCompute comps(i, r);

    REQUIRE(comps.inside == false);
}

TEST_CASE("The hit, when an intersection occurs on the inside") {
    Ray r(Point(0, 0, 0), Vector(0, 0, 1));
    Sphere shape = Sphere();
    Intersection i(1, shape);
    PreCompute comps(i, r);

    REQUIRE(comps.point == Point(0, 0, 1));
    REQUIRE(comps.eyev == Vector(0, 0, -1));
    REQUIRE(comps.inside == true);
}

TEST_CASE("Shading an intersection") {
    World w = World::default_world();

    Ray r(Point(0, 0, -5), Vector(0, 0, 1));

    std::shared_ptr<Sphere> shape = std::dynamic_pointer_cast<Sphere>(w.objects[0]);

    Intersection i(4, *shape);
    PreCompute comps(i, r);

    Color c = w.shade_hit(comps);

    REQUIRE(c == Color(0.38066f, 0.47583f, 0.2855f));
}

TEST_CASE("Shading an intersection from the inside") {
    World w = World::default_world();
    w.light = Light(Point(0, 0.25f, 0), Color(1, 1, 1));

    Ray r(Point(0, 0, 0), Vector(0, 0, 1));

    std::shared_ptr<Sphere> shape = std::dynamic_pointer_cast<Sphere>(w.objects[1]);

    Intersection i(0.5f, *shape);
    PreCompute comps(i, r);

    Color c = w.shade_hit(comps);

    REQUIRE(c == Color(0.90498f, 0.90498f, 0.90498f));
}

TEST_CASE("The color when a ray misses") {
    World w = World::default_world();
    Ray r(Point(0, 0, -5), Vector(0, 1, 0));
    Color c = w.color_at(r);

    REQUIRE(c == Color(0, 0, 0));
}

TEST_CASE("The color when a ray hits") {
    World w = World::default_world();
    Ray r(Point(0, 0, -5), Vector(0, 0, 1));
    Color c = w.color_at(r);

    REQUIRE(c == Color(0.38066f, 0.47583f, 0.2855f));
}

TEST_CASE("The color with an intersection behind the ray") {
    World w = World::default_world();
    std::shared_ptr<Sphere> outer = std::dynamic_pointer_cast<Sphere>(w.objects[0]);
    std::shared_ptr<Sphere> inner = std::dynamic_pointer_cast<Sphere>(w.objects[1]);
    outer->material.m_ambient = 1;
    inner->material.m_ambient = 1;

    Ray r(Point(0, 0, 0.75), Vector(0, 0, -1));
    Color c = w.color_at(r);

    REQUIRE(c == inner->material.m_color);
}

TEST_CASE("The transformation matrix for the default orientation") {
    Point from(0, 0, 0);
    Point to(0, 0, -1);
    Vector up(0, 1, 0);
    Matrix<4,4> t = Matrix<4,4>::view_transform(from, to, up);

    REQUIRE(t == Matrix<4, 4>::identity());
}

TEST_CASE("The transformation matrix looking in positive z direction") {
    Point from(0, 0, 0);
    Point to(0, 0, 1);
    Vector up(0, 1, 0);
    Matrix<4,4> t = Matrix<4,4>::view_transform(from, to, up);

    REQUIRE(t == Matrix<4, 4>::scaling(-1, 1, -1));
}

TEST_CASE("The transformation moves the world") {
    Point from(0, 0, 8);
    Point to(0, 0, 0);
    Vector up(0, 1, 0);
    Matrix<4,4> t = Matrix<4,4>::view_transform(from, to, up);

    REQUIRE(t == Matrix<4, 4>::translation(0, 0, -8));
}

TEST_CASE("An arbitrary view transformation") {
    Point from(1, 3, 2);
    Point to(4, -2, 8);
    Vector up(1, 1, 0);
    Matrix<4,4> t = Matrix<4,4>::view_transform(from, to, up);

    Matrix<4,4> Expected{{
        -0.50709f, 0.76772f, -0.35857f, 0.0f,
         0.50709f, 0.60609f,  0.59761f, 0.0f,
         0.67612f, 0.12122f, -0.71714f, 0.0f,
        -2.36643f,-2.82843f,  0.0f,     1.0f
    }};

    REQUIRE(t == Expected);
}

TEST_CASE("Constructing a camera") {
    int hsize = 160;
    int vsize = 120;
    float field_of_view = PI/2;

    Camera c(hsize, vsize, field_of_view);
    REQUIRE(c.hsize == 160);
    REQUIRE(c.vsize == 120);
    REQUIRE(c.field_of_view == Catch::Approx(PI/2));
    REQUIRE(c.transform == Matrix<4, 4>::identity());    
}

TEST_CASE("The pixel size for a horizontal canvas") {
    Camera c(200, 125, PI/2);

    REQUIRE(c.pixel_size == Catch::Approx(0.01f));
}

TEST_CASE("The pixel size for a vertical canvas") {
    Camera c(125, 200, PI/2);

    REQUIRE(c.pixel_size == Catch::Approx(0.01f));
}

TEST_CASE("Constructing a ray through the center of a the canvas") {
    Camera c(201, 101, PI/2);
    Ray r = c.ray_for_pixel(100, 50);
    
    REQUIRE(r.m_origin == Point(0, 0, 0));
    REQUIRE(r.m_direction == Vector(0, 0, -1));
}

TEST_CASE("Constructing a ray through the corner of a the canvas") {
    Camera c(201, 101, PI/2);
    Ray r = c.ray_for_pixel(0, 0);
    
    REQUIRE(r.m_origin == Point(0, 0, 0));
    REQUIRE(r.m_direction == Vector(0.66519f, 0.33259f, -0.66851f));
}

TEST_CASE("Constructing a ray when the camera is transformed") {
    Camera c(201, 101, PI/2);
    c.set_transform(Matrix<4, 4>::rotation_y(PI/4)*Matrix<4, 4>::translation(0, -2, 5));
    Ray r = c.ray_for_pixel(100, 50);
    
    REQUIRE(r.m_origin == Point(0, 2, -5));
    REQUIRE(r.m_direction == Vector(sqrtf(2)/2, 0, -sqrtf(2)/2));
}

TEST_CASE("Rendering a world with a camera") {
    World w = World::default_world();
    Camera c(11, 11, PI/2);

    Point from(0, 0, -5);
    Point to(0, 0, 0);
    Vector up(0, 1, 0);
    c.set_transform(Matrix<4,4>::view_transform(from, to, up));
    Canvas image = Canvas(11, 11);
    bool should_restart = false;
    c.render(w, image, should_restart);

    REQUIRE(image.pixel_at(5, 5) == Color(0.38066f, 0.47583f, 0.2855f));
}