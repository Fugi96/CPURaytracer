#include "Plane.h"
#include "util/Intersection.h"
#include "util/SimdMath.h"
#include "util/util.h"
#include <cstddef>
#include <cstdint>
#include <immintrin.h>

Vector Plane::local_normal_at(const Point& p) const {
    return {0.0f, 1.0f, 0.0f};
}

bool Plane::local_intersect(const Ray& r, std::array<Intersection, MAX_XS>& xs, uint8_t& count) const {
    if (abs(r.m_direction.data.y) < EPSILON)
        return false;

    float t = (-r.m_origin.data.y / r.m_direction.data.y);
    
    xs[count++] = Intersection(t, *this);
    return true;
}

uint8_t Plane::local_intersect8(const RayPacket8& rays, IntersectionPacket& xs) const {
    const __m256 zero = _mm256_setzero_ps();

    const __m256 absY = _mm256_andnot_ps(_mm256_set1_ps(-0.0f), rays.direction.y);

    const __m256 valid = _mm256_cmp_ps(absY, _mm256_set1_ps(EPSILON), _CMP_GE_OQ);

    const __m256 t = _mm256_div_ps(
        _mm256_xor_ps(rays.origin.y, _mm256_set1_ps(-0.0f)),
        rays.direction.y
    );


    uint8_t mask = (uint8_t)_mm256_movemask_ps(valid);

    if (!mask)
        return 0;

    alignas(32) float tValues[8];
    _mm256_store_ps(tValues, t);

    uint8_t written = 0;

    for (size_t i = 0; i < 8; ++i) {
        if (!(mask & (1 << i)))
            continue;

        uint8_t& count = xs.count[i];

        xs.xs[i][count++] = Intersection(tValues[i], *this);

        written |= (1u << i);
    }

    return written;
}