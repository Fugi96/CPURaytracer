#pragma once
#include "Tuple.h"
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

struct Vector : public Tuple {
    Vector() = default;
    Vector(float x, float y, float z) : Tuple(x, y, z, 0.0f) {}
    Vector(const glm::vec4& v) : Tuple(v) {}
    explicit Vector(const glm::vec3& v) : Tuple(glm::vec4(v, 0.0f)) {}

    Vector operator+(const Vector& other) const {
        return {data + other.data};
    }

    Vector operator-(const Vector& other) const {
        return {data - other.data};
    }

    Vector operator-() const {
        return {-data};
    }

    Vector operator*(float t) const {
        return {data * t};
    }

    static float magnitude(const Vector& v) {
        return glm::length(v.data);
    }

    static Vector normalize(const Vector& v) {
        return glm::normalize(v.data);
    }

    static Vector reflect(const Vector& v, const Vector& normal) {
        return glm::reflect(v.data, normal.data);
    }

    static float dot(const Vector& v1, const Vector& v2) {
        return glm::dot(v1.data, v2.data);
    }

    static Vector cross(const Vector& v1, const Vector& v2) {
        return Vector(glm::cross(glm::vec3(v1.data), glm::vec3(v2.data))); //Check if bug
    }
};