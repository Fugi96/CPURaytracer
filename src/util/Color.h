#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include "util.h"
#include <algorithm>

struct Color {
    glm::vec3 rgb;

    Color() : rgb(0.0f) {}
    Color(float r, float g, float b) : rgb(r, g, b) {}
    explicit Color(glm::vec3 rgb) : rgb(rgb) {}

    static Color red() { return Color(1.0f, 0.0f, 0.0f); }
    static Color green() { return Color(0.0f, 1.0f, 0.0f); }
    static Color blue() { return Color(0.0f, 0.0f, 1.0f); }

    inline Color operator+(const Color& other) const {
        return Color(this->rgb + other.rgb);
    }

    inline Color operator-(const Color& other) const {
        return Color(this->rgb - other.rgb);
    }

    inline Color operator*(const float& scalar) const {
        return Color(this->rgb * scalar);
    }

    inline Color operator*(const Color& other) const {
        return Color(this->rgb * other.rgb);
    }

    inline Color& operator+=(const Color& other) {
        rgb += other.rgb;
        return *this;
    }

    inline Color& operator-=(const Color& other) {
        rgb -= other.rgb;
        return *this;
    }

    inline bool operator==(const Color& other) const {
        return glm::all(glm::epsilonEqual(this->rgb, other.rgb, EPSILON));
    }

    inline glm::vec3 outValue() const {
        float r = std::round(clipColorValue(this->rgb.r)*255u);
        float g = std::round(clipColorValue(this->rgb.g)*255u);
        float b = std::round(clipColorValue(this->rgb.b)*255u);

        return glm::vec3(r, g, b);
    }

private:
    inline float clipColorValue(float v) const {
        return std::max(0.0f, std::min(v, 1.0f));
    }
};