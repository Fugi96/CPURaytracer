#pragma once
#include <immintrin.h>
#include "util/Ray.h"

// This kinda reimplements Matrix-Vector multiplication, but I cant find a clean way to use
// glm while also vectorizing.
struct Vec3Packet {
    __m256 x, y, z;

    Vec3Packet() = default;

    Vec3Packet(__m256 x_, __m256 y_, __m256 z_)
        : x(x_), y(y_), z(z_) {}
};

struct RayPacket8 {
    Vec3Packet origin;
    Vec3Packet direction;

    static RayPacket8 make_packet(const Ray* rays) {
        alignas(32) float ox[8], oy[8], oz[8];
        alignas(32) float dx[8], dy[8], dz[8];

        for (size_t i = 0; i < 8; ++i) {
            ox[i] = rays[i].m_origin.x();
            oy[i] = rays[i].m_origin.y();
            oz[i] = rays[i].m_origin.z();

            dx[i] = rays[i].m_direction.x();
            dy[i] = rays[i].m_direction.y();
            dz[i] = rays[i].m_direction.z();
        }

        return {
            .origin=Vec3Packet(_mm256_load_ps(ox),
                       _mm256_load_ps(oy),
                       _mm256_load_ps(oz)),
        
            .direction=Vec3Packet(_mm256_load_ps(dx),
                       _mm256_load_ps(dy),
                       _mm256_load_ps(dz))
        };
    }

    RayPacket8 transform_packet(const Matrix<4,4>& m) const {
        __m256 m00 = _mm256_set1_ps(m.data[0][0]);
        __m256 m01 = _mm256_set1_ps(m.data[1][0]);
        __m256 m02 = _mm256_set1_ps(m.data[2][0]);
        __m256 m03 = _mm256_set1_ps(m.data[3][0]);

        __m256 m10 = _mm256_set1_ps(m.data[0][1]);
        __m256 m11 = _mm256_set1_ps(m.data[1][1]);
        __m256 m12 = _mm256_set1_ps(m.data[2][1]);
        __m256 m13 = _mm256_set1_ps(m.data[3][1]);

        __m256 m20 = _mm256_set1_ps(m.data[0][2]);
        __m256 m21 = _mm256_set1_ps(m.data[1][2]);
        __m256 m22 = _mm256_set1_ps(m.data[2][2]);
        __m256 m23 = _mm256_set1_ps(m.data[3][2]);

        __m256 tox =
            _mm256_add_ps(
                _mm256_add_ps(
                    _mm256_mul_ps(m00, origin.x),
                    _mm256_mul_ps(m01, origin.y)
                ),
                _mm256_add_ps(
                    _mm256_mul_ps(m02, origin.z),
                    m03
                )
            );

        __m256 toy =
            _mm256_add_ps(
                _mm256_add_ps(
                    _mm256_mul_ps(m10, origin.x),
                    _mm256_mul_ps(m11, origin.y)
                ),
                _mm256_add_ps(
                    _mm256_mul_ps(m12, origin.z),
                    m13
                )
            );

        __m256 toz =
            _mm256_add_ps(
                _mm256_add_ps(
                    _mm256_mul_ps(m20, origin.x),
                    _mm256_mul_ps(m21, origin.y)
                ),
                _mm256_add_ps(
                    _mm256_mul_ps(m22, origin.z),
                    m23
                )
            );


        __m256 tdx =
            _mm256_add_ps(
                _mm256_add_ps(
                    _mm256_mul_ps(m00, direction.x),
                    _mm256_mul_ps(m01, direction.y)
                ),
                _mm256_mul_ps(m02, direction.z)
            );

        __m256 tdy =
            _mm256_add_ps(
                _mm256_add_ps(
                    _mm256_mul_ps(m10, direction.x),
                    _mm256_mul_ps(m11, direction.y)
                ),
                _mm256_mul_ps(m12, direction.z)
            );

        __m256 tdz =
            _mm256_add_ps(
                _mm256_add_ps(
                    _mm256_mul_ps(m20, direction.x),
                    _mm256_mul_ps(m21, direction.y)
                ),
                _mm256_mul_ps(m22, direction.z)
            );

        return {
            .origin = Vec3Packet{tox, toy, toz},
            .direction = Vec3Packet{tdx, tdy, tdz}
        };
    }
};

namespace simd {
    static __m256 dot(const Vec3Packet& v1, const Vec3Packet& v2) {
        return _mm256_add_ps(
            _mm256_add_ps(
                _mm256_mul_ps(v1.x, v2.x),
                _mm256_mul_ps(v1.y, v2.y)),
                _mm256_mul_ps(v1.z, v2.z)
            );
    }
}