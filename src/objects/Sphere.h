#pragma once
#include "Object.h"
#include "util/Intersection.h"
#include <string>

struct Sphere : public Object {
    Sphere() = default;

    Sphere(std::string name) : Object(std::move(name)) {}

    bool local_intersect(const Ray& r, std::array<Intersection, MAX_XS>& xs, uint8_t& count) const override;
    uint8_t local_intersect8(const RayPacket8& rayPacket, IntersectionPacket& xs) const override;

    [[nodiscard]] Vector local_normal_at(const Point& p) const override;

private:
    virtual bool equals(const Object& other) {
        if (!Object::equals(other)) return false;

        const Sphere& sph = static_cast<const Sphere&>(other);

        // Implement actual logic here if Sphere gets Sphere-specific parameters
        return true;
    }
};