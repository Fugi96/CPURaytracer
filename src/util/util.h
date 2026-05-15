#pragma once
#include <cmath>
#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
constexpr auto EPSILON = 0.00001f;
constexpr float PI = glm::pi<float>();

inline bool equal(float a, float b) {
    if (std::abs(a - b) < EPSILON) {
        return true;
    }
    else {
        return false;
    }
}