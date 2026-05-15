#pragma once
#include "Object.h"
#include <cstdint>
#include <string>

struct Plane : public Object {
    Plane() = default;

    Plane(std::string name) : Object(std::move(name)) {}

    bool local_intersect(const Ray& r, std::array<Intersection, 8>& xs, uint8_t& count) const override;
    uint8_t local_intersect8(const RayPacket8& rayPacket, IntersectionPacket& xs) const override;

    [[nodiscard]] Vector local_normal_at(const Point& p) const override;

private:
    virtual bool equals(const Object& other) {
        if (!Object::equals(other)) return false;

        const Plane& plane = static_cast<const Plane&>(other);

        return true;
    }
};