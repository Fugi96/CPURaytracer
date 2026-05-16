#pragma once
#include "objects/Plane.h"
#include "shading/Light.h"
#include "Object.h"
#include "Sphere.h"
#include "Material.h"
#include "util/Color.h"
#include "util/Matrix.h"
#include "util/Intersection.h"
#include "shading/Shading.h"
#include "util/Vector.h"
#include <array>
#include <sys/types.h>
#include <vector>
#include <memory>

constexpr int MAX_RAY_BOUNCES = 5;

struct World {
    Light light;
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<Sphere> spheres;
    std::vector<Plane> planes;

    World() = default;

    void add_object(const std::shared_ptr<Object>& object) {
        if (auto sphere = dynamic_cast<Sphere*>(object.get()))
            spheres.emplace_back(*sphere);
        else {
            if (auto plane = dynamic_cast<Plane*>(object.get()))
                planes.emplace_back(*plane);
        }
        objects.push_back(object);
    }

    static World default_world() {
        World w = World();
        w.light = Light(Point(-10.0f, 10.0f, -10.0f), Color(1.0f, 1.0f, 1.0f));

        std::shared_ptr<Object> s1 = std::make_shared<Sphere>(Sphere());
        s1->material = Material(Color(0.8f, 1.0f, 0.6f), 0.1f, 0.7f, 0.2f, 200.0f);
        std::shared_ptr<Object> s2 = std::make_shared<Sphere>(Sphere());
        s2->set_transform(Matrix<4, 4>::scaling(0.5f, 0.5f, 0.5f));
        w.objects.push_back(s1);
        w.objects.push_back(s2);
        return w;
    }

    Color shade_hit(const PreCompute& comps, int remaining = MAX_RAY_BOUNCES) {
        bool shadowed = this->is_shadowed(comps.over_point);

        Color surface = Shading::lighting(comps.m_object->material, *comps.m_object,
            this->light, comps.over_point, comps.eyev, comps.normalv, shadowed);

        Color reflected = reflected_color(comps, remaining);
        Color refracted = refracted_color(comps, remaining);
        return surface + reflected + refracted;
    }

    //std::array<Color, 8> shade_hit8(const PreCompute* comps) {
    //    Point over_points[8];
    //    for (size_t i = 0; i < 8; ++i) {
    //        over_points[i] = comps[i].over_point;
    //    }
//
    //    u_int8_t shadowed = this->is_shadowed8(over_points);
//
    //    std::array<Color, 8> result;
//
    //    for (size_t i = 0; i < 8; ++i) {
    //        result[i] = Shading::lighting(
    //            comps[i].m_object->material, 
    //            *comps[i].m_object, 
    //            this->light, 
    //            over_points[i], 
    //            comps[i].eyev, 
    //            comps[i].normalv,
    //            (shadowed & (1 << i)));
    //    }
    //    return result;
    //}

    void direct_intersect(const Ray& r, std::array<Intersection, MAX_XS>& xs, uint8_t& count) {
        for (const auto& obj : this->objects) {
            obj->intersect(r, xs, count);
        }
    }

    void intersect8(const Ray* rays, IntersectionPacket& xs) {
        for (const Sphere& s : this->spheres)
            s.intersect8(rays, xs);

        for (const Plane& p : this->planes)
            p.intersect8(rays, xs);
    }

    std::array<Color, 8> color_at_batch(const Ray* rays, int remaining = MAX_RAY_BOUNCES) {
        IntersectionPacket xs;

        this->intersect8(rays, xs);

        std::array<Color, 8> result;

        for (int i = 0; i < 8; ++i) {
            auto& per_ray = xs.xs[i];
            uint8_t c = xs.count[i];

            if (c == 0) {
                result[i] = Color(0.0f, 0.0f, 0.0f);
                continue;
            }

            Intersection hit{};

            Intersection::sort_by_t(per_ray, c);

            if (!Intersection::hit_sorted(per_ray, c, hit)) {
                result[i] = Color(0.0f,0.0f, 0.0f);
                continue;
            }
            PreCompute comps = PreCompute(hit, rays[i]);
            result[i] = shade_hit(comps, remaining);
        }

        return result;
    }

    Color color_at(const Ray& r, int remaining = MAX_RAY_BOUNCES) {
        std::array<Intersection, MAX_XS> xs;
        uint8_t xs_count = 0;
        this->direct_intersect(r, xs, xs_count);
        Intersection::sort_by_t(xs.data(), xs_count);
        Intersection hit{};

        if (Intersection::hit_sorted(xs.data(), xs_count, hit)) {
            PreCompute comps = PreCompute(hit, r);
            return shade_hit(comps, remaining);
        }

        return Color{0.0f, 0.0f, 0.0f};
    }

    //u_int8_t is_shadowed8(const Point* points) {
    //    Vector v[8];
    //    float distance[8];
    //    Vector direction[8];
    //    Ray shadow_rays[8];
//
    //    for (size_t i = 0; i < 8; ++i) {
    //        v[i] = this->light.m_position - points[i];
    //        distance[i] = Vector::magnitude(v[i]);
    //        direction[i] = Vector::normalize(v[i]);
    //        shadow_rays[i] = Ray(points[i], direction[i]);
    //    }
//
    //    Intersection shadow_hits[8];
//
    //    for (const auto& obj : this-> objects) {
    //        obj->intersect8(shadow_rays, shadow_hits);
    //    }
//
    //    u_int8_t hits = 0b00000000;
    //    for (size_t i = 0; i < 8; ++i) {
    //        hits |= (shadow_hits[i].m_object != nullptr) << i;
    //    }
//
    //    return hits;
    //}

    bool is_shadowed(const Point& p) {
        Vector v = this->light.m_position - p;
        float distance = Vector::magnitude(v);
        Vector direction = Vector::normalize(v);

        Ray r = Ray(p, direction);

        std::array<Intersection, 8> xs_shadow;
        
        for (const auto& obj : this->objects) {
            uint8_t xs_shadow_count = 0;
            obj->intersect(r, xs_shadow, xs_shadow_count);
            Intersection::sort_by_t(xs_shadow.data(), xs_shadow_count);
            Intersection shadow_hit{};

            if (Intersection::hit_sorted(xs_shadow.data(), xs_shadow_count, shadow_hit) && shadow_hit.t < distance) {
                return true;
            }
        }

        return false;
    }

    Color reflected_color(const PreCompute& comps, int remaining = MAX_RAY_BOUNCES) {
        if (remaining <= 0 || comps.m_object->material.m_reflective == 0)
            return {0, 0, 0};

        Ray reflect_ray(comps.over_point, comps.reflectv);
        return color_at(reflect_ray, remaining - 1)*comps.m_object->material.m_reflective;
    }

    Color refracted_color(const PreCompute& comps, int remaining = MAX_RAY_BOUNCES) {
        if (remaining == 0 || comps.m_object->material.m_transparency == 0.0f)
            return {0.0f, 0.0f, 0.0f};


        float n_ratio = comps.n1/comps.n2;
        float cos_i = Vector::dot(comps.eyev, comps.normalv);
        float sin2_t = n_ratio*n_ratio*(1.0f - (cos_i*cos_i));

        if (sin2_t > 1.0f)
            return {0.0f, 0.0f, 0.0f};

        float cos_t = std::sqrtf(1.0f - sin2_t);
        Vector direction = comps.normalv * (n_ratio*cos_i-cos_t) - comps.eyev*n_ratio;

        Ray refract_ray(comps.under_point, direction);

        return color_at(refract_ray, remaining - 1);

    }
};