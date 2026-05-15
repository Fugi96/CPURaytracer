#pragma once
#include <glm/glm.hpp>
#include "util.h"

struct Tuple {
    glm::vec4 data;

    Tuple() { data = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); }
    Tuple(float x, float y, float z, float w) : data(x, y, z, w) {}
    Tuple(const glm::vec4& data) : data(data) {}

    inline float& x() { return data.x; }
    inline float& y() { return data.y; }
    inline float& z() { return data.z; }
    inline float& w() { return data.w; }

    [[nodiscard]] inline const float x() const { return data.x; }
    [[nodiscard]] inline const float y() const { return data.y; }
    [[nodiscard]] inline const float z() const { return data.z; }
    [[nodiscard]] inline const float w() const { return data.w; }

    bool operator==(const Tuple& other) const {
        return glm::all(glm::epsilonEqual(this->data, other.data, EPSILON));
    }

    inline Tuple operator*(const float& scalar) const {
        return Tuple(this->data * scalar);
    }

    inline Tuple operator/(const float& scalar) const {
        return Tuple(this->data / scalar);
    }

    inline Tuple operator+(const Tuple& other) const {
        return Tuple(this->data + other.data);
    }

    inline Tuple operator-(const Tuple& other) const {
        return Tuple(this->data - other.data);
    }

    inline Tuple operator-() const {
        return Tuple(-data);
    }

protected:
    Tuple(const Tuple& other) : data(other.data) {}
    glm::vec3 xyz() const { return glm::vec3(data.x, data.y, data.z); };
};