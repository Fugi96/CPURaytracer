#pragma once
#include "util/Color.h"
#include "util/Point.h"

struct Light {
    Color m_intensity{ 1.0f, 1.0f, 1.0f };
    Point m_position{ 0.0f, 0.0f, 0.0f };

    Light() {}
    Light(Point position, Color intensity) : m_position(position), m_intensity(intensity) {}

    inline bool operator==(const Light& other) const {
        return ((this->m_intensity == other.m_intensity)
            && (this->m_position == other.m_position));
    }
};