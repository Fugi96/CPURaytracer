#pragma once
#include "objects/Material.h"
#include "Light.h"
#include "util/Vector.h"
#include "util/Point.h"
#include "util/Color.h"
#include "objects/Object.h"
#include <math.h>

namespace Shading {
    static const Color lighting(const Material& material, const Object& object, const Light& light, const Point& point, const Vector& eye_v, const Vector& normal_v, bool shadowed) {
        Color color = material.m_color;

        if (material.pattern)
            color = object.pattern_at(point);

        Color effective_color = color * light.m_intensity;
        Color ambient = effective_color * material.m_ambient;

        if (shadowed) return ambient;

        Vector light_v = Vector::normalize(light.m_position - point);
        Color diffuse{};
        Color specular{};

        float light_dot_normal = Vector::dot(light_v, normal_v);

        if (light_dot_normal >= 0) {
            diffuse = effective_color * material.m_diffuse * light_dot_normal;
        }
        
        Vector reflect_v = Vector::reflect(-light_v, normal_v);
        float reflect_dot_eye = Vector::dot(reflect_v, eye_v);

        if (reflect_dot_eye > 0) {
            float factor = std::powf(reflect_dot_eye, material.m_shininess);
            specular = light.m_intensity * material.m_specular * factor;
        }

        return ambient + diffuse + specular;
    }
}