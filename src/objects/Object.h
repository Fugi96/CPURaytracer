#pragma once
#include <array>
#include "objects/Pattern.h"
#include "util/Ray.h"
#include "Material.h"
#include "util/Matrix.h"
#include <cstdint>
#include <optional>
#include <string>
#include <typeinfo> // IWYU pragma: keep
#include <utility>
#include "util/SimdMath.h"

struct Intersection; // Forward declare
struct IntersectionPacket;

struct Object {
    Matrix<4, 4> transform{ Matrix<4,4>::identity() };
    Matrix<4, 4> transform_inv{ Matrix<4, 4>::identity() };
    Material material{ Material() };
    std::string name {""};

    Object() = default;

    Object(std::string name) : name(std::move(name)) {}

    virtual ~Object() = default;

    void set_transform(const Matrix<4, 4>& transformation) {
        this->transform = transformation;
        this->transform_inv = Matrix<4, 4>::inverse(transformation);
    }

    void set_material(const Material& material) {
        this->material = material;
    }

    bool operator==(const Object& other) const {
        if (typeid(*this) != typeid(other)) return false;
        return equals(other);
    }

    bool intersect(const Ray& r, std::array<Intersection, 8>& xs, uint8_t& count) const {
        return local_intersect(Ray::transform(r, this->transform_inv), xs, count);
    }

    uint8_t intersect8(const Ray* rays, IntersectionPacket& xs) const {
        RayPacket8 transformed_rays = RayPacket8::make_packet(rays).transform_packet(this->transform_inv);
        return local_intersect8(transformed_rays, xs);
    }

    [[nodiscard]] Vector normal_at(const Point& p) const {
        Point local_p = transform_inv * p;
        Vector local_n = local_normal_at(local_p);
        Vector world_n = Matrix<4,4>::transpose(transform_inv)*local_n;
        world_n.data.w = 0;

        return Vector::normalize(world_n);
    }

    virtual bool local_intersect(const Ray& r, std::array<Intersection, 8>& xs, uint8_t& count) const = 0;
    virtual uint8_t local_intersect8(const RayPacket8& rayPacket, IntersectionPacket& xs) const = 0;
    [[nodiscard]] virtual Vector local_normal_at(const Point& p) const = 0;
    [[nodiscard]] virtual Ray transform_ray(const Ray& r) const {
        return Ray::transform(r, this->transform_inv);
    }

    [[nodiscard]] Color pattern_at(const Point& p) const {
        Point object_point = this->transform_inv * p;

        const Pattern& pattern = *material.pattern;

        Point pattern_point = pattern.transform_inv * object_point;

        return Patterns::pattern_at(pattern, pattern_point);
    }

protected:
    [[nodiscard]] virtual bool equals(const Object& other) const {
        return ((this->transform == other.transform)
            && (this->material == other.material));
    }
};