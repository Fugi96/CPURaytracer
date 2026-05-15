#pragma once
#include "objects/Object.h"
#include "Point.h"
#include "Vector.h"
#include "util.h"
#include <algorithm>
#include <limits>
#include <vector>

static constexpr int MAX_XS = 8;

struct Intersection {
    float t;
    const Object* m_object;

    Intersection() : t(std::numeric_limits<float>::infinity()), m_object(nullptr) {}
    Intersection(float t, const Object& object) : t(t), m_object(&object) {}

    bool operator==(const Intersection& other) const {
        return (this->t == other.t && this->m_object == other.m_object);
    }

    static void sort_by_t(Intersection* xs, uint8_t count) {
        std::ranges::sort(xs, xs + count,
            [](const Intersection& a, const Intersection& b) {
                return a.t < b.t;
            });
    }

    static bool hit(Intersection* xs, uint8_t count, Intersection& hit) {     
    bool found = false;

        for (int i = 0; i < count; ++i) {
            if (xs[i].t > 0 && (!found || xs[i].t < hit.t)) {
                hit = xs[i];
                found = true;
            }     
        }
        return false;
    }

    static bool hit_sorted(Intersection* xs, uint8_t count, Intersection& hit) {
        for (size_t i = 0; i < count; ++i) {
            if (xs[i].t >= 0) {
                hit = xs[i];
                return true;
            }
        }

        return false;
    }
};

// Intersection tracking packet can store up to MAX_XS intersections
// If a ray has more than MAX_XS intersections fall back to slow heap allocated
// vectors for tracking.
struct IntersectionPacket {
    uint8_t count[8] = {};
    alignas(32) Intersection xs[8][MAX_XS];

    IntersectionPacket() = default;
};

struct PreCompute {
    float t;
    const Object* m_object;
    Point point;
    Vector eyev;
    Vector reflectv;
    Vector normalv;
    Point over_point;
    bool inside{ false };

    PreCompute(const Intersection& i, const Ray& r) : t(i.t),
        m_object(i.m_object), point(r.position(i.t)), eyev(-r.m_direction),
        normalv(i.m_object->normal_at(point)) {
        if (Vector::dot(this->normalv, this->eyev) < 0) {
            this->inside = true;
            this->normalv = -this->normalv;
        }
        this->reflectv = Vector::reflect(r.m_direction, this->normalv);
        this->over_point = this->point + this->normalv * EPSILON;
    }
};
