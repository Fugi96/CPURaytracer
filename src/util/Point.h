#pragma once
#include "Tuple.h"
#include "Vector.h"

struct Point : public Tuple {
    Point() : Tuple(0.0f, 0.0f, 0.0f, 1.0f) {}
    Point(float x, float y, float z) : Tuple(x, y, z, 1.0f) {}
    Point(const glm::vec4& p) : Tuple(p) {}
    explicit Point(const glm::vec3& p) : Tuple(glm::vec4(p, 1.0f)) {}

    Point operator+(const Vector& other) const {
        return {data + other.data};
    }

    Vector operator-(const Point& other) const {
        return {data - other.data};
    }

    Point operator-(const Vector& other) const {
        return {data - other.data};
    }
};