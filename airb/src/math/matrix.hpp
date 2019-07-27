#ifndef MATH_MATRIX_HPP
#define MATH_MATRIX_HPP

#include <array>

#include <cassert>
#include <cmath>
#include <ostream>

#include <glew.h>

enum
{
    X, R = X,
    Y, G = Y,
    Z, B = Z,
    W, A = W
};

namespace math {

template<typename T, size_t rows, size_t cols> struct Matrix;

template<typename T> using Matrix3 = Matrix<T, 3, 3>;
template<typename T> using Matrix4 = Matrix<T, 4, 4>;
using Matrix3i = Matrix3<GLint>;
using Matrix3f = Matrix3<GLfloat>;
using Matrix4i = Matrix4<GLint>;
using Matrix4f = Matrix4<GLfloat>;
template<typename T, size_t comps> using Vector = Matrix<T, comps, 1>;
template<typename T> using Vector2 = Vector<T, 2>;
template<typename T> using Vector3 = Vector<T, 3>;
template<typename T> using Vector4 = Vector<T, 4>;
using Vector2i = Vector2<GLint>;
using Vector2f = Vector2<GLfloat>;
using Vector3i = Vector3<GLint>;
using Vector3f = Vector3<GLfloat>;
using Vector4i = Vector4<GLint>;
using Vector4f = Vector4<GLfloat>;

template<typename T, size_t rows, size_t cols>
struct Matrix final
{
    T elements[rows * cols];

    Matrix()
        : elements ()
    { }

    Matrix(std::array<T, rows * cols> const& elems)
        : elements ()
    {
        for (size_t i = 0; i < rows * cols; ++i)
            elements[i] = elems[i];
    }

    template<typename U>
    explicit Matrix(Matrix<U, rows, cols> const& mat)
        : elements ()
    {
        for (size_t i = 0; i < rows * cols; ++i)
            elements[i] = static_cast<T>(mat[i]);
    }

    ~Matrix() = default;

    Matrix<T, rows, cols>& operator=(Matrix<T, rows, cols> const& rhs)
    {
        for (size_t i = 0; i < rows * cols; ++i)
            elements[i] = rhs[i];
        return *this;
    }

    T& operator[](size_t const n)
    {
        return elements[n];
    }

    T const& operator[](size_t const n) const
    {
        return elements[n];
    }

    T& operator()(size_t const i, size_t const j)
    {
        return elements[i * cols + j];
    }

    T const& operator()(size_t const i, size_t const j) const
    {
        return elements[i * cols + j];
    }

    Matrix<T, rows, cols>& operator+=(Matrix<T, rows, cols> const& rhs)
    {
        for (size_t i = 0; i < rows * cols; ++i)
            elements[i] += rhs[i];
        return *this;
    }

    Matrix<T, rows, cols>& operator-=(Matrix<T, rows, cols> const& rhs)
    {
        for (size_t i = 0; i < rows * cols; ++i)
            elements[i] -= rhs[i];
        return *this;
    }

    Matrix<T, rows, cols>& operator*=(T const rhs)
    {
        for (size_t i = 0; i < rows * cols; ++i)
            elements[i] *= rhs;
        return *this;
    }

    Matrix<T, rows, cols>& operator/=(T const rhs)
    {
        for (size_t i = 0; i < rows * cols; ++i)
            elements[i] /= rhs;
        return *this;
    }

    T dot(Vector<T, rows> const& rhs) const
    {
        static_assert(cols == 1, "Dot product can only be calculated for vectors (Nx1 matrices).");
        T ret = static_cast<T>(0);
        for (size_t i = 0; i < rows; ++i)
            ret += elements[i] * rhs[i];
        return ret;
    }

    T magnitude() const
    {
        static_assert(cols == 1, "Magnitude can only be calculated for vectors (Nx1 matrices).");
        return static_cast<T>(sqrt(static_cast<double>(dot(*this))));
    }

    Vector<T, rows> normalized() const
    {
        static_assert(cols == 1, "Normalization is only applicable to vectors (Nx1 matrices).");
        T const mag = magnitude();
        assert(mag != static_cast<T>(0));
        return *this / mag;
    }

    Vector<T, rows> projected_onto(Vector<T, rows> const& vec) const
    {
        static_assert(cols == 1, "Vector projection can only be calculated for vectors (Nx1 matrices).");
        Vector<T, rows> const n = vec.normalized();
        return n * dot(n);
    }

    static Matrix<T, rows, cols> zero()
    {
        Matrix<T, rows, cols> ret;
        for (size_t i = 0; i < rows * cols; ++i)
            ret[i] = static_cast<T>(0);
        return ret;
    }

    static Matrix<T, rows, cols> one()
    {
        Matrix<T, rows, cols> ret;
        for (size_t i = 0; i < rows * cols; ++i)
            ret[i] = static_cast<T>(1);
        return ret;
    }

    static Matrix<T, rows, rows> identity()
    {
        static_assert(rows == cols, "Identity matrix must be an NxN matrix.");
        Matrix<T, rows, rows> ret;
        for (size_t i = 0; i < rows * rows; ++i)
            ret[i] = i / rows == i % rows ? static_cast<T>(1) : static_cast<T>(0);
        return ret;
    }

    static Matrix<T, rows, rows> translate(Vector<T, rows - 1> const& t)
    {
        static_assert(rows == cols, "Translation matrix must be an NxN matrix.");
        Matrix<T, rows, rows> ret = identity();
        for (size_t i = 0; i < rows - 1; ++i)
            ret(i, cols - 1) = t[i];
        return ret;
    }

    static Matrix<T, rows, rows> scale(Vector<T, rows - 1> const& s)
    {
        static_assert(rows == cols, "Scale matrix must be an NxN matrix.");
        Matrix<T, rows, rows> ret = identity();
        for (size_t i = 0; i < rows - 1; ++i)
            ret(i, i) = s[i];
        return ret;
    }

    static Matrix<T, rows, rows> perspective(T const fov, T const aspect_r, T const f, T const n)
    {
        static_assert(rows == 4 && cols == 4, "Perspective matrix must be a 4x4 matrix.");
        Matrix<T, rows, rows> ret = identity();

        static double const DEG2RAD = 3.14159265358979323846 / 180.0;
        T const q = static_cast<T>(1) / static_cast<T>(tan(0.5 * static_cast<double>(fov) * DEG2RAD));
        T const a = q / aspect_r;

        T const n_sub_f = n - f;

        T const b = (n + f) / n_sub_f;
        T const c = (static_cast<T>(2) * n * f) / n_sub_f;

        ret(0, 0) = a;
        ret(1, 1) = q;
        ret(2, 2) = b;
        ret(2, 3) = c;
        ret(3, 2) = static_cast<T>(-1);

        return ret;
    }

    static Matrix<T, rows, rows> ortho(T const r, T const l,
                                       T const t, T const b,
                                       T const f, T const n)
    {
        static_assert(rows == 4 && cols == 4, "Orthographic matrix must be a 4x4 matrix.");
        Matrix<T, rows, rows> ret = identity();

        T const r_sub_l = r - l;
        T const t_sub_b = t - b;
        T const f_sub_n = f - n;

        ret(0, 0) =  static_cast<T>(2) / r_sub_l;
        ret(1, 1) =  static_cast<T>(2) / t_sub_b;
        ret(2, 2) = -static_cast<T>(2) / f_sub_n;

        ret(0, 3) = -(r + l) / r_sub_l;
        ret(1, 3) = -(t + b) / t_sub_b;
        ret(2, 3) = -(f + n) / f_sub_n;

        return ret;
    }

    static Vector<T, rows> right()
    {
        static_assert(cols == 1, "Must be a vector (Nx1 matrix).");
        static_assert(rows >= 1, "Vector must have at least 1 component.");
        Vector<T, rows> ret = zero();
        ret[X] = static_cast<T>(1);
        return ret;
    }

    static Vector<T, rows> left()
    {
        static_assert(cols == 1, "Must be a vector (Nx1 matrix).");
        static_assert(rows >= 1, "Vector must have at least 1 component.");
        Vector<T, rows> ret = zero();
        ret[X] = static_cast<T>(-1);
        return ret;
    }

    static Vector<T, rows> up()
    {
        static_assert(cols == 1, "Must be a vector (Nx1 matrix).");
        static_assert(rows >= 2, "Vector must have at least 2 components.");
        Vector<T, rows> ret = zero();
        ret[Y] = static_cast<T>(1);
        return ret;
    }

    static Vector<T, rows> down()
    {
        static_assert(cols == 1, "Must be a vector (Nx1 matrix).");
        static_assert(rows >= 2, "Vector must have at least 2 components.");
        Vector<T, rows> ret = zero();
        ret[Y] = static_cast<T>(-1);
        return ret;
    }

    static Vector<T, rows> forward()
    {
        static_assert(cols == 1, "Must be a vector (Nx1 matrix).");
        static_assert(rows >= 3, "Vector must have at least 3 components.");
        Vector<T, rows> ret = zero();
        ret[Z] = static_cast<T>(1);
        return ret;
    }

    static Vector<T, rows> back()
    {
        static_assert(cols == 1, "Must be a vector (Nx1 matrix).");
        static_assert(rows >= 3, "Vector must have at least 3 components.");
        Vector<T, rows> ret = zero();
        ret[Z] = static_cast<T>(-1);
        return ret;
    }
};

template<typename T>
inline Matrix3<T> rotate_2d(T const rad)
{
    Matrix3<T> ret = Matrix3<T>::identity();
    T const sin_t = static_cast<T>(sin(static_cast<double>(rad)));
    T const cos_t = static_cast<T>(cos(static_cast<double>(rad)));
    ret[0] =  cos_t;
    ret[1] = -sin_t;
    ret[3] =  sin_t;
    ret[4] =  cos_t;
    return ret;
}

template<typename T>
inline Matrix4<T> rotate_3d(Vector3<T> const& axis, T const rad)
{
    Matrix4<T> ret = Matrix4<T>::identity();

    T const cos_t = static_cast<T>(cos(static_cast<T>(rad)));
    T const sin_t = static_cast<T>(sin(static_cast<T>(rad)));

    T const& x = axis.elements[X];
    T const& y = axis.elements[Y];
    T const& z = axis.elements[Z];
    T const xx = x * x;
    T const yy = y * y;
    T const zz = z * z;
    T const xy = x * y;
    T const xz = x * z;
    T const yz = y * z;

    T const x_sin_t = x * sin_t;
    T const y_sin_t = y * sin_t;
    T const z_sin_t = z * sin_t;

    T const one_sub_cos = static_cast<T>(1) - cos_t;

    ret(0, 0) = xx * one_sub_cos + cos_t;
    ret(0, 1) = xy * one_sub_cos - z_sin_t;
    ret(0, 2) = xz * one_sub_cos + y_sin_t;

    ret(1, 0) = xy * one_sub_cos + z_sin_t;
    ret(1, 1) = yy * one_sub_cos + cos_t;
    ret(1, 2) = yz * one_sub_cos - x_sin_t;

    ret(2, 0) = xz * one_sub_cos - y_sin_t;
    ret(2, 1) = yz * one_sub_cos - x_sin_t;
    ret(2, 2) = zz * one_sub_cos + cos_t;

    return ret;
}
template<typename T, unsigned M, unsigned N>
inline bool operator!=(Matrix<T, M, N> const& lhs, Matrix<T, M, N> const& rhs)
{
    for (unsigned i = 0; i < M*N; ++i)
        if (lhs[i] != rhs[i])
            return true;
    return false;
}
namespace VectorOperations {

template<typename T>
inline Vector2<T> rotated_2d(Vector2<T> const& vec, T const rad)
{
    Matrix3<T> const rot_m = rotate_2d(rad);
    Vector3<T> const vec_h({ vec[X], vec[Y], static_cast<T>(1) });
    Vector3<T> const res = rot_m * vec_h;
    return Vector2<T>({ res[X], res[Y] });
}

template<typename T>
inline Vector3<T> rotated_3d(Vector3<T> const& vec, Vector3<T> const& axis, T const rad)
{
    Matrix4<T> const rot_m = rotate_3d(axis, rad);
    Vector4<T> const vec_h({ vec[X], vec[Y], vec[Z], static_cast<T>(1) });
    Vector4<T> const res = rot_m * vec_h;
    return Vector3<T>({ res[X], res[Y], res[Z] });
}

template<typename T>
inline T cross_2d(Vector2<T> const& lhs, Vector2<T> const& rhs)
{
    return lhs[X] * rhs[Y] - lhs[Y] * rhs[X];
}

template<typename T>
inline Vector3<T> cross_3d(Vector3<T> const& lhs, Vector3<T> const& rhs)
{
    return Vector3<T>({ lhs[Y] * rhs[Z] - lhs[Z] * rhs[Y],
                        lhs[Z] * rhs[X] - lhs[X] * rhs[Z],
                        lhs[X] * rhs[Y] - lhs[Y] * rhs[X] });
}

} // namespace VectorOperations

template<typename T, size_t rows, size_t cols>
inline Matrix<T, rows, cols> operator-(Matrix<T, rows, cols> const& rhs)
{
    Matrix<T, rows, cols> ret;
    for (size_t i = 0; i < rows * cols; ++i)
        ret[i] = -rhs[i];
    return ret;
}

template<typename T, size_t rows, size_t cols>
inline Matrix<T, rows, cols> operator+(Matrix<T, rows, cols> const& lhs,
                                       Matrix<T, rows, cols> const& rhs)
{
    Matrix<T, rows, cols> ret;
    for (size_t i = 0; i < rows * cols; ++i)
        ret[i] = lhs[i] + rhs[i];
    return ret;
}

template<typename T, size_t rows, size_t cols>
inline Matrix<T, rows, cols> operator-(Matrix<T, rows, cols> const& lhs,
                                       Matrix<T, rows, cols> const& rhs)
{
    Matrix<T, rows, cols> ret;
    for (size_t i = 0; i < rows * cols; ++i)
        ret[i] = lhs[i] - rhs[i];
    return ret;
}

template<typename T, size_t M, size_t N, size_t P>
inline Matrix<T, M, P> operator*(Matrix<T, M, N> const& lhs,
                                 Matrix<T, N, P> const& rhs)
{
    Matrix<T, M, P> ret;

    for (size_t i = 0; i < M; ++i)
    {
        for (size_t j = 0; j < P; ++j)
        {
            T& e = ret(i, j);
            e = static_cast<T>(0);

            for (size_t k = 0; k < N; ++k)
                e += lhs(i, k) * rhs(k, j);
        }
    }

    return ret;
}

template<typename T, size_t rows, size_t cols>
inline Matrix<T, rows, cols> operator*(Matrix<T, rows, cols> const& lhs, T const rhs)
{
    Matrix<T, rows, cols> ret;
    for (size_t i = 0; i < rows * cols; ++i)
        ret[i] = lhs[i] * rhs;
    return ret;
}

template<typename T, size_t rows, size_t cols>
inline Matrix<T, rows, cols> operator*(T const lhs, Matrix<T, rows, cols> const& rhs)
{
    Matrix<T, rows, cols> ret;
    for (size_t i = 0; i < rows * cols; ++i)
        ret[i] = rhs[i] * lhs;
    return ret;
}

template<typename T, size_t rows, size_t cols>
inline Matrix<T, rows, cols> operator/(Matrix<T, rows, cols> const& lhs, T const rhs)
{
    Matrix<T, rows, cols> ret;
    for (size_t i = 0; i < rows * cols; ++i)
        ret[i] = lhs[i] / rhs;
    return ret;
}

template<typename T, size_t rows, size_t cols>
std::ostream& operator<<(std::ostream& os, Matrix<T, rows, cols> const& mat)
{
    for (size_t i = 0; i < rows; ++i)
    {
        for (size_t j = 0; j < cols; ++j)
            os << mat(i, j) << " ";
        os << "\n";
    }
    return os;
}

} // namespace math

#endif // MATH_MATRIX_HPP