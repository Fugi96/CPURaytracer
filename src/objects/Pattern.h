#pragma once

#include "util/Color.h"
#include "util/Point.h"
#include "util/Matrix.h"
#include <cmath>
#include <variant>

struct StripePattern {
    Color a;
    Color b;
};

struct GradientPattern {
    Color a;
    Color b;
};

struct RingPattern {
    Color a;
    Color b;
};

struct CheckersPattern {
    Color a;
    Color b;
};

struct Pattern {
    Matrix<4, 4> transform = Matrix<4, 4>::identity();
    Matrix<4, 4> transform_inv = Matrix<4, 4>::identity();

    void set_transform(const Matrix<4, 4>& transformation) {
        this->transform = transformation;
        this->transform_inv = Matrix<4, 4>::inverse(transformation);
    }

    std::variant<StripePattern, 
                 GradientPattern, 
                 RingPattern,
                 CheckersPattern> type;
};

namespace Patterns {

inline Color local_pattern_at(
    const StripePattern& pattern,
    const Point& point)
{
    if (static_cast<int>(std::floorf(point.data.x)) % 2 == 0)
        return pattern.a;

    return pattern.b;
}

inline Color local_pattern_at(
    const GradientPattern& pattern,
    const Point& point)
{
    return pattern.a 
        + (pattern.b - pattern.a) * (point.data.x - std::floorf(point.data.x));
}

inline Color local_pattern_at(
    const RingPattern& pattern,
    const Point& point)
{
    if (static_cast<int>(std::floorf(sqrtf(
        point.data.x * point.data.x 
        + point.data.z * point.data.z))) == 0) {
        return pattern.a;
    }
    return pattern.b;
}

inline Color local_pattern_at(
    const CheckersPattern& pattern,
    const Point& point)
{
    if (static_cast<int>(std::floorf(point.data.x) 
                       + std::floorf(point.data.y) 
                       + std::floorf(point.data.z))%2 == 0) {
        return pattern.a;
    }
    return pattern.b;
}

inline Color pattern_at(const Pattern& pattern, const Point& point) {
    return std::visit(
        [&](const auto& type)
        {
            return local_pattern_at(type, point);
        },
        pattern.type
    );
}

inline Pattern Stripe(Color a, Color b) {
    return Pattern{
        .transform = Matrix<4,4>::identity(),
        .type = StripePattern{
            .a = a,
            .b = b
        }
    };
}

inline Pattern Gradient(Color a, Color b) {
    return Pattern{
        .transform = Matrix<4, 4>::identity(),
        .type = GradientPattern{
            .a = a,
            .b = b
        }
    };
}

inline Pattern Ring(Color a, Color b) {
       return Pattern{
        .transform = Matrix<4, 4>::identity(),
        .type = RingPattern{
            .a = a,
            .b = b
        }
    }; 
}

inline Pattern Checkers(Color a, Color b) {
       return Pattern{
        .transform = Matrix<4, 4>::identity(),
        .type = CheckersPattern{
            .a = a,
            .b = b
        }
    }; 
}
}