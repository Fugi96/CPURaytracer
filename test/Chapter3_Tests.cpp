#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include <catch2/catch_all.hpp>
#include "util/Matrix.h"

TEST_CASE("Constructing and inspecting a 4x4 matrix", "[matrix]")
{
    Matrix<4,4> M{{
        1, 2, 3, 4,
        5.5f, 6.5f, 7.5f, 8.5f,
        9, 10, 11, 12,
        13.5f, 14.5f, 15.5f, 16.5f
    }};

    REQUIRE(M.data[0][0] == Catch::Approx(1));
    REQUIRE(M.data[0][3] == Catch::Approx(4));
    REQUIRE(M.data[1][0] == Catch::Approx(5.5f));
    REQUIRE(M.data[1][2] == Catch::Approx(7.5f));
    REQUIRE(M.data[2][2] == Catch::Approx(11));
    REQUIRE(M.data[3][0] == Catch::Approx(13.5f));
    REQUIRE(M.data[3][2] == Catch::Approx(15.5f));
}

TEST_CASE("A 2x2 matrix ought to be representable", "[matrix]")
{
    Matrix<2,2> M{{
        -3, 5,
         1, -2
    }};

    REQUIRE(M.data[0][0] == Catch::Approx(-3));
    REQUIRE(M.data[0][1] == Catch::Approx(5));
    REQUIRE(M.data[1][0] == Catch::Approx(1));
    REQUIRE(M.data[1][1] == Catch::Approx(-2));
}

TEST_CASE("Matrix equality with identical matrices", "[matrix]")
{
    Matrix<4,4> A{{
        1,2,3,4,
        5,6,7,8,
        9,8,7,6,
        5,4,3,2
    }};

    Matrix<4,4> B{{
        1,2,3,4,
        5,6,7,8,
        9,8,7,6,
        5,4,3,2
    }};

    REQUIRE(A == B);
}

TEST_CASE("Matrix equality with different matrices", "[matrix]")
{
    Matrix<4,4> A{{
        1,2,3,4,
        5,6,7,8,
        9,8,7,6,
        5,4,3,2
    }};

    Matrix<4,4> B{{
        2,3,4,5,
        6,7,8,9,
        8,7,6,5,
        4,3,2,1
    }};

    REQUIRE(A != B);
}

TEST_CASE("Multiplying two matrices", "[matrix]")
{
    Matrix<4,4> A{{
        1,5,9,5,
        2,6,8,4,
        3,7,7,3,
        4,8,6,2
    }};

    Matrix<4,4> B{{
        -2,3,4,1,
        1,2,3,2,
        2,1,6,7,
        3,-1,5,8
    }};

    Matrix<4,4> Expected{{
        20,44,40,16,
        22,54,58,26,
        50,114,110,46,
        48,108,102,42
    }};

    REQUIRE(A * B == Expected);
}

TEST_CASE("A matrix multiplied by a tuple", "[matrix]")
{
    Matrix<4,4> A{{
        1,2,8,0,
        2,4,6,0,
        3,4,4,0,
        4,2,1,1
    }};

    Tuple b(1,2,3,1);

    REQUIRE(A * b == Tuple(18,24,33,1));
}

TEST_CASE("Multiplying a matrix by the identity matrix", "[matrix]")
{
    Matrix<4,4> A{{
        0,1,2,4,
        1,2,4,8,
        2,4,8,16,
        4,8,16,32
    }};

    Matrix<4,4> I = Matrix<4,4>::identity();

    REQUIRE(A * I == A);
}

TEST_CASE("Multiplying the identity matrix by a tuple", "[matrix]")
{
    Matrix<4,4> I = Matrix<4,4>::identity();
    Tuple a(1,2,3,4);

    REQUIRE(I * a == a);
}

TEST_CASE("Transposing a matrix", "[matrix]")
{
    Matrix<4,4> A{{
        0,9,3,0,
        9,8,0,8,
        1,8,5,3,
        0,0,5,8
    }};

    Matrix<4,4> Expected{{
        0,9,1,0,
        9,8,8,0,
        3,0,5,5,
        0,8,3,8
    }};

    REQUIRE(Matrix<4,4>::transpose(A) == Expected);
}

TEST_CASE("Calculating the determinant of a 2x2 matrix", "[matrix]")
{
    Matrix<2,2> A{{
        1,5,
        -3,2
    }};

    REQUIRE(Matrix<2,2>::determinant(A) == Catch::Approx(17));
}

TEST_CASE("Scenario: Calculating the inverse of a matrix", "[matrix]")
{
    Matrix<4,4> A{{
        -5, 2, 6, -8,
         1, -5, 1, 8,
         7, 7, -6, -7,
         1, -3, 7, 4
    }};

    Matrix<4,4> B = Matrix<4,4>::inverse(A);

    REQUIRE(Matrix<4,4>::determinant(A) == Catch::Approx(532));

    REQUIRE(B.data[3][2] == Catch::Approx(-160.0f / 532.0f));
    REQUIRE(B.data[2][3] == Catch::Approx(105.0f / 532.0f));

    Matrix<4,4> Expected{{
         0.21805f,  0.45113f,  0.24060f, -0.04511f,
        -0.80827f, -1.45677f, -0.44361f,  0.52068f,
        -0.07895f, -0.22368f, -0.05263f,  0.19737f,
        -0.52256f, -0.81391f, -0.30075f,  0.30639f
    }};

    REQUIRE(B == Expected);
}

TEST_CASE("Scenario: Calculating the inverse of another matrix", "[matrix]")
{
    Matrix<4,4> A{{
        8, -5, 9, 2,
        7, 5, 6, 1,
        -6, 0, 9, 6,
        -3, 0, -9, -4
    }};

    Matrix<4,4> B = Matrix<4,4>::inverse(A);

    Matrix<4,4> Expected{{
        -0.15385f, -0.15385f, -0.28205f, -0.53846f,
        -0.07692f,  0.12308f,  0.02564f,  0.03077f,
         0.35897f,  0.35897f,  0.43590f,  0.92308f,
        -0.69231f, -0.69231f, -0.76923f, -1.92308f
    }};

    REQUIRE(B == Expected);
}

TEST_CASE("Scenario: Calculating the inverse of a third matrix", "[matrix]")
{
    Matrix<4,4> A{{
        9, 3, 0, 9,
        -5, -2, -6, -3,
        -4, 9, 6, 4,
        -7, 6, 6, 2
    }};

    Matrix<4,4> B = Matrix<4,4>::inverse(A);

    Matrix<4,4> Expected{{
        -0.04074f, -0.07778f,  0.14444f, -0.22222f,
        -0.07778f,  0.03333f,  0.36667f, -0.33333f,
        -0.02901f, -0.14630f, -0.10926f,  0.12963f,
         0.17778f,  0.06667f, -0.26667f,  0.33333f
    }};

    REQUIRE(B == Expected);
}

TEST_CASE("Scenario: Multiplying a product by its inverse", "[matrix]")
{
    Matrix<4,4> A{{
        3, -9, 7, 3,
        3, -8, 2, -9,
        -4, 4, 4, 1,
        -6, 5, -1, 1
    }};

    Matrix<4,4> B{{
        8, 2, 2, 2,
        3, -1, 7, 0,
        7, 0, 5, 4,
        6, -2, 0, 5
    }};

    Matrix<4,4> C = A * B;

    REQUIRE(C * Matrix<4,4>::inverse(B) == A);
}