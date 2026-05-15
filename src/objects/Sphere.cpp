#include "Sphere.h"
#include "util/Intersection.h"
#include "util/SimdMath.h"
#include <cmath>
#include <cstdint>
#include <immintrin.h>

// Process 8 Rays at once using SIMD instructions for root computations
// Dot a, b, c computations using dot-products should already use SIMD by default through glm
//uint8_t Sphere::local_intersect8(const RayPacket8& rays, Intersection* bestHits) const {
//    const __m256 zero = _mm256_setzero_ps();
//    const __m256 one  = _mm256_set1_ps(1.0f);
//    const __m256 four = _mm256_set1_ps(4.0f);
//
//    // b = 2 * dot(d, o)
//    __m256 b = _mm256_mul_ps(
//        _mm256_set1_ps(2.0f),
//        simd::dot(rays.direction, rays.origin)
//    );
//
//    __m256 a = simd::dot(rays.direction, rays.direction);
//    __m256 c = simd::dot(rays.origin, rays.origin) - one;
//
//    // discriminant
//    __m256 discriminant = _mm256_sub_ps(
//        _mm256_mul_ps(b, b),
//        _mm256_mul_ps(four, _mm256_mul_ps(a, c))
//    );
//
//    // valid if discriminant >= 0 AND a is not ~0
//    __m256 valid = _mm256_cmp_ps(discriminant, zero, _CMP_GE_OQ);
//
//    // sqrt only meaningful lanes
//    __m256 sqrtD = _mm256_sqrt_ps(_mm256_max_ps(discriminant, zero));
//
//    __m256 inv2a = _mm256_div_ps(_mm256_set1_ps(0.5f), a);
//    __m256 negB  = _mm256_sub_ps(zero, b);
//
//    // t0/t1 computed always but masked later
//    __m256 t0 = _mm256_mul_ps(_mm256_sub_ps(negB, sqrtD), inv2a);
//    __m256 t1 = _mm256_mul_ps(_mm256_add_ps(negB, sqrtD), inv2a);
//
//    // ensure only valid discriminant lanes survive
//    t0 = _mm256_blendv_ps(zero, t0, valid);
//    t1 = _mm256_blendv_ps(zero, t1, valid);
//
//    __m256 tmin = _mm256_min_ps(t0, t1);
//
//    // load current best hits
//    alignas(32) float bestArr[8];
//    for (int i = 0; i < 8; i++)
//        bestArr[i] = bestHits[i].t;
//
//    __m256 best = _mm256_load_ps(bestArr);
//
//    // valid hit = t > 0 AND t < best
//    __m256 gtZero = _mm256_cmp_ps(tmin, zero, _CMP_GT_OQ);
//    __m256 better = _mm256_cmp_ps(tmin, best, _CMP_LT_OQ);
//
//    __m256 hit = _mm256_and_ps(valid, _mm256_and_ps(gtZero, better));
//
//    uint8_t mask = (uint8_t)_mm256_movemask_ps(hit);
//    if (!mask)
//        return 0;
//
//    alignas(32) float tvals[8];
//    _mm256_store_ps(tvals, tmin);
//
//    while (mask) {
//        int lane = __builtin_ctz(mask);
//
//        bestHits[lane].t = tvals[lane];
//        bestHits[lane].m_object = this;
//
//        mask &= (mask - 1);
//    }
//
//    return (uint8_t)_mm256_movemask_ps(hit);
//}

uint8_t Sphere::local_intersect8(const RayPacket8& rays, IntersectionPacket& xs) const {
    const __m256 zero = _mm256_setzero_ps();
    const __m256 one  = _mm256_set1_ps(1.0f);
    const __m256 four = _mm256_set1_ps(4.0f);

    // b = 2 * dot(d, o)
    __m256 b = _mm256_mul_ps(
        _mm256_set1_ps(2.0f),
        simd::dot(rays.direction, rays.origin)
    );

    __m256 a = simd::dot(rays.direction, rays.direction);
    __m256 c = simd::dot(rays.origin, rays.origin) - one;

    // discriminant
    __m256 discriminant = _mm256_sub_ps(
        _mm256_mul_ps(b, b),
        _mm256_mul_ps(four, _mm256_mul_ps(a, c))
    );

    // valid if discriminant >= 0 AND a is not ~0
    __m256 valid = _mm256_cmp_ps(discriminant, zero, _CMP_GE_OQ);

    // sqrt only meaningful lanes
    __m256 sqrtD = _mm256_sqrt_ps(_mm256_max_ps(discriminant, zero));

    __m256 inv2a = _mm256_div_ps(_mm256_set1_ps(0.5f), a);
    __m256 negB  = _mm256_sub_ps(zero, b);

    // t0/t1 computed always but masked later
    __m256 t0 = _mm256_mul_ps(_mm256_sub_ps(negB, sqrtD), inv2a);
    __m256 t1 = _mm256_mul_ps(_mm256_add_ps(negB, sqrtD), inv2a);

    int mask = _mm256_movemask_ps(valid);
    uint8_t written = 0;

    alignas(32) float t0s[8];
    alignas(32) float t1s[8];

    _mm256_store_ps(t0s, t0);
    _mm256_store_ps(t1s, t1);

    for (size_t i = 0; i < 8; ++i) {
        if ((mask & (1 << i)) == 0)
            continue;

        uint8_t& count = xs.count[i];

        xs.xs[i][count++] = Intersection(t0s[i], *this);
        xs.xs[i][count++] = Intersection(t1s[i], *this);

        written |= (1u << i);
    }

    return written;
}

bool Sphere::local_intersect(const Ray& r, std::array<Intersection, MAX_XS>& xs, uint8_t& count) const
{
    float a = Vector::dot(r.m_direction, r.m_direction);
    float b = 2 * Vector::dot(r.m_direction, r.from_origin);
    float c = Vector::dot(r.from_origin, r.from_origin) - 1.0f;

    float discriminant = (b * b) - (4 * a * c);

    if (discriminant < 0) return false;

    const float sqrtD = std::sqrtf(discriminant);
    const float inv2a = 0.5f / a;

    float t0 = (-b - sqrtD) * inv2a;
    float t1 = (-b + sqrtD) * inv2a;

    if (t0 > t1) std::swap(t0, t1);

    xs[count++] = Intersection(t0, *this);
    xs[count++] = Intersection(t1, *this);
    return true;
}

Vector Sphere::local_normal_at(const Point& p) const
{
    return p - Point(0.0f, 0.0f, 0.0f);
}
