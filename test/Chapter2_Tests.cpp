#include "Canvas.h"
#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include <catch2/catch_all.hpp>
#include "util/Color.h"
#include <cstdint>

TEST_CASE("Colors are (red, green, blue) tuples") {
    Color c(-0.5f, 0.4f, 1.7f);
    REQUIRE(c.rgb.r == Catch::Approx(-0.5f));
    REQUIRE(c.rgb.g == Catch::Approx(0.4f));
    REQUIRE(c.rgb.b == Catch::Approx(1.7f));
}

TEST_CASE("Adding colors") {
    Color c1(0.9f, 0.6f, 0.75f);
    Color c2(0.7f, 0.1f, 0.25f);

    REQUIRE((c1+c2) == Color(1.6f, 0.7f, 1.0f));
}

TEST_CASE("Subtracting colors") {
    Color c1(0.9f, 0.6f, 0.75f);
    Color c2(0.7f, 0.1f, 0.25f);

    REQUIRE((c1-c2) == Color(0.2f, 0.5f, 0.5f));
}

TEST_CASE("Multiplying a color by a scalar") {
    Color c(0.2f, 0.3f, 0.4f);

    REQUIRE((c*2) == Color(0.4f, 0.6f, 0.8f));
}

TEST_CASE("Multiplying colors") {
    Color c1(1.0f, 0.2f, 0.4f);
    Color c2(0.9f, 1.0f, 0.1f);

    REQUIRE((c1*c2) == Color(0.9f, 0.2f, 0.04f));
}

TEST_CASE("Creating a Canvas", "[canvas]") {
    Canvas c(10, 20);

    SECTION("Dimensions are correct") {
        REQUIRE(c.m_width == Catch::Approx(10));
        REQUIRE(c.m_height == Catch::Approx(20));
    }

    SECTION("All Pixels were initialized as black (0, 0, 0)") {
        for (uint16_t y = 0; y < c.m_height; ++y) {

            for (uint16_t x = 0; x < c.m_width; ++x) {
                const Color& col = c.pixel_at(x, y);

                REQUIRE(col == Color(0.0f, 0.0f, 0.0f));
            }
        }
    }
}

TEST_CASE("Writing a pixel to a canvas") {
    Canvas c(10, 20);

    Color red(1.0f, 0.0f, 0.0f);

    c.write_pixel(2, 3, red);

    REQUIRE(c.pixel_at(2, 3) == red);
}

// Output ppm tests omitted since I output to a GLFW/OpenGL window