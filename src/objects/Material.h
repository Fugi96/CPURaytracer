#pragma once
#include "util/Color.h"
#include "util/util.h"
#include "Pattern.h"
#include <optional>

struct Material {
    Color m_color{ 1.0f, 1.0f, 1.0f };
    std::optional<Pattern> pattern;
    float m_ambient{ 0.1f };
    float m_diffuse{ 0.9f };
    float m_specular{ 0.9f };
    float m_shininess{ 200.0f };
    float m_reflective{ 0.0f };
    float m_transparency{ 0.0f };
    float m_refractive_index{ 1.0f };

    Material() {}
    Material(Color color, float ambient, float diffuse, float specular, float shininess) : 
        m_color(color), m_ambient(ambient), m_diffuse(diffuse), m_specular(specular), m_shininess(shininess) { }

    Material(Color color) : m_color(color)  {}

    inline bool operator==(const Material& other) const {
        return ((this->m_color == other.m_color)
            && (equal(this->m_ambient, other.m_ambient))
            && (equal(this->m_diffuse, other.m_diffuse))
            && (equal(this->m_specular, other.m_specular))
            && (equal(this->m_shininess, other.m_shininess)));
    }
};