#pragma once
#include "Point.h"
#include "Vector.h"
#include "Matrix.h"
#include <immintrin.h>

struct Ray {
    Point m_origin;
    Vector m_direction;
    Vector from_origin;

    Ray() = default;

    Ray(const Point& origin, const Vector& direction) : 
        m_origin(origin), 
        m_direction(direction), 
        from_origin(Vector(glm::vec3(origin.data))) {
    }

    [[nodiscard]] Point position(const float& t) const {
        return Point(this->m_origin + this->m_direction * t);
    }

    // This doesn't vectorize well and kills most performance
    static Ray transform(const Ray& r, const Matrix<4,4>& transformation) {
        return {transformation * r.m_origin, transformation * r.m_direction};
    }
};