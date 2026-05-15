#pragma once
#include <glm/glm.hpp>
#include "util.h"
#include <glm/matrix.hpp>
#include <glm/gtx/transform.hpp>
#include "Point.h"
#include "Vector.h"

template <glm::length_t M, glm::length_t N>
struct Matrix {
    glm::mat<M, N, float, glm::packed_highp> data;
    Matrix(const glm::mat<M, N, float, glm::packed_highp>& init) : data(init) {}

    static Matrix<N, M> view_transform(const Point& from, const Point& to, const Vector& up) {
        const Vector forward = Vector::normalize(to - from);
        const Vector left = Vector::cross(forward, Vector::normalize(up));
        Vector true_up = Vector::cross(left, forward);

        Matrix<4, 4> orientation = Matrix<M, N>(glm::mat4(left.data.x, true_up.data.x, -forward.data.x, 0.0f,
            left.data.y, true_up.data.y, -forward.data.y, 0.0f,
            left.data.z, true_up.data.z, -forward.data.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f));

        return orientation * Matrix<M, N>::translation(-from.data.x, -from.data.y, -from.data.z);
    }

    static Matrix<N, M> transpose(const Matrix<M, N>& mat) {
        return Matrix<N, M>(glm::transpose(mat.data));
    }

    static Matrix<M, N> identity() {
        return Matrix<M, N>(glm::mat<M, N, float, glm::packed_highp>{1.0f});
    }

    static float determinant(const Matrix<N, N>& mat) {
        return glm::determinant(mat.data);
    }

    static Matrix<N, N> inverse(const Matrix <N, N>& mat) {
        return glm::inverse(mat.data);
    }

    static Matrix translation(float x, float y, float z) {
        return Matrix<4, 4>(glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)));
    }

    static Matrix translation(glm::vec3 v) {
        return Matrix<4, 4>(glm::translate(glm::mat4(), v));
    }

    [[nodiscard]] Matrix translate(float x, float y, float z) const {
        return Matrix(Matrix<M, N>::translation(x, y, z).data * this->data);
    }

    [[nodiscard]] Matrix translate(const glm::vec3& v) const {
        return Matrix(Matrix<M, N>::translation(v).data * this->data);
    }

    static Matrix scaling(glm::vec3 v) {
        return Matrix<4, 4>(glm::scale(glm::mat4(1.0f), v));
    }

    static Matrix scaling(float x, float y, float z) {
        return Matrix<4, 4>(glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z)));
    }

    [[nodiscard]] Matrix scale(float x, float y, float z) const {
        return Matrix(Matrix<M, N>::scaling(x, y, z).data * this->data);
    }

    [[nodiscard]] Matrix scale(const glm::vec3& v) const {
        return Matrix(Matrix<M, N>::scaling(v).data * this->data);
    }

    static Matrix rotation_x(float rad) {
        return Matrix<4, 4>(glm::rotate(rad, glm::vec3(1.0f, 0.0f, 0.0f)));
    }

    [[nodiscard]] Matrix rotate_x(float rad) const {
        return Matrix(Matrix<M, N>::rotation_x(rad).data * this->data);
    }

    static Matrix rotation_y(float rad) {
        return Matrix<4, 4>(glm::rotate(rad, glm::vec3(0.0f, 1.0f, 0.0f)));
    }

    [[nodiscard]] Matrix rotate_y(float rad) const {
        return Matrix(Matrix<M, N>::rotation_y(rad).data * this->data );
    }

    static Matrix rotation_z(float rad) {
        return Matrix<4, 4>(glm::rotate(rad, glm::vec3(0.0f, 0.0f, 1.0f)));
    }

    [[nodiscard]] Matrix rotate_z(float rad) const {
        return Matrix(Matrix<M, N>::rotation_z(rad).data * this->data);
    }

    static Matrix shearing(float xy, float xz, float yx, float yz, float zx, float zy) {
        return Matrix<4, 4>(glm::mat4(1.0f, yx, zx, 0.0f, xy, 1.0f, zy, 0.0f, xz, yz, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
    }

    Matrix shear(float xy, float xz, float yx, float yz, float zx, float zy) {
        return Matrix(this->data * Matrix<M, N>::shearing(xy, xz, yx, yz, zx, zy).data);
    }

    bool operator==(const Matrix<M, N>& other) const {
        bool Result = true;
        for (glm::length_t i = 0; i < N; ++i) {
            Result = Result && glm::all(glm::epsilonEqual(this->data[i], other.data[i], EPSILON));
        }
        return Result;
    }

    bool operator!=(const Matrix<M, N>& other) const {
        return !(*this == other);
    }

    template<glm::length_t P>
    Matrix<M, P> operator*(const Matrix<N, P>& other) const {
        return Matrix<M, P>(this->data * other.data);
    }

    Tuple operator*(const Tuple& other) const {
        return Tuple(this->data * other.data);
    }

    Point operator*(const Point& other) const {
        return Point(this->data * other.data);
    }

    Vector operator*(const Vector& other) const {
        return Vector(this->data * other.data);
    }
};