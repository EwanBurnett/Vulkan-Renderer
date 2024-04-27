#ifndef __MATH_MATRIX_H
#define __MATH_MATRIX_H
/**
*   @file Matrix.h
*   @brief Matrix Helpers
*   @author Ewan Burnett (EwanBurnettSK@outlook.com)
*   @date 2024/04/24
*/
#include "Vector3.h"
#include "Vector4.h"
#include <easy/profiler.h>

namespace VKR {
    namespace Math {

        /**
         * @brief A 4x4 Matrix of elements.
         * @tparam T Type for the internal matrix elements. float by default. 
        */
        template<typename T = float>
        struct Matrix4x4 {
            Matrix4x4() {
                //memset(arr, 0, sizeof(T) * 16); 
            }

            union {
                Vector4<T> vec[4];
                T arr[16];
            };

            friend Matrix4x4 operator *(Matrix4x4& lhs, const Matrix4x4& rhs) {
                EASY_FUNCTION(profiler::colors::Yellow800);
                Matrix4x4 mat = {};
                Matrix4x4 b = Matrix4x4::Transpose(rhs);

                mat.arr[0] = lhs.vec[0].Dot(b.vec[0]);
                mat.arr[1] = lhs.vec[0].Dot(b.vec[1]);
                mat.arr[2] = lhs.vec[0].Dot(b.vec[2]);
                mat.arr[3] = lhs.vec[0].Dot(b.vec[3]);

                mat.arr[4] = lhs.vec[1].Dot(b.vec[0]);
                mat.arr[5] = lhs.vec[1].Dot(b.vec[1]);
                mat.arr[6] = lhs.vec[1].Dot(b.vec[2]);
                mat.arr[7] = lhs.vec[1].Dot(b.vec[3]);

                mat.arr[8] = lhs.vec[2].Dot(b.vec[0]);
                mat.arr[9] = lhs.vec[2].Dot(b.vec[1]);
                mat.arr[10] = lhs.vec[2].Dot(b.vec[2]);
                mat.arr[11] = lhs.vec[2].Dot(b.vec[3]);

                mat.arr[12] = lhs.vec[3].Dot(b.vec[0]);
                mat.arr[13] = lhs.vec[3].Dot(b.vec[1]);
                mat.arr[14] = lhs.vec[3].Dot(b.vec[2]);
                mat.arr[15] = lhs.vec[3].Dot(b.vec[3]);

                return mat;
            }


            /**
             * @return Returns the 4x4 Identity Matrix.
            */
            inline static constexpr Matrix4x4 Identity() {
                EASY_FUNCTION(profiler::colors::Yellow800);
                Matrix4x4 mat;

                for (uint8_t i = 0; i < 16; i++) {
                    mat.arr[i] = 0.0;
                }

                mat.arr[0] = 1.0;
                mat.arr[5] = 1.0;
                mat.arr[10] = 1.0;
                mat.arr[15] = 1.0;

                return mat;
            }


            /**
             * @brief Builds a 4x4 Translation Matrix from a Vector
             * @param translation 
             * @remarks 
             * @return
            */
            inline static constexpr Matrix4x4 Translation(const Vector3<T> translation) {
                EASY_FUNCTION(profiler::colors::Yellow800);
                Matrix4x4 mat = Matrix4x4::Identity();

                mat.arr[12] = translation.x;
                mat.arr[13] = translation.y;
                mat.arr[14] = translation.z;

                return mat;
            }

            inline static constexpr Matrix4x4 XRotation(const T radians) {
                EASY_FUNCTION(profiler::colors::Yellow800);
                Matrix4x4 mat = Matrix4x4::Identity();


                mat.arr[5] = cos(radians);
                mat.arr[6] = sin(radians);
                mat.arr[9] = -mat.arr[6];
                mat.arr[10] = mat.arr[5];

                return mat;
            }

            inline static constexpr Matrix4x4 YRotation(const T radians) {
                EASY_FUNCTION(profiler::colors::Yellow800);
                Matrix4x4 mat = Matrix4x4::Identity();

                mat.arr[0] = cos(radians);
                mat.arr[2] = -sin(radians);
                mat.arr[8] = -mat.arr[2];
                mat.arr[10] = mat.arr[0];

                return mat;
            }


            inline static constexpr Matrix4x4 ZRotation(const T radians) {
                EASY_FUNCTION(profiler::colors::Yellow800);
                Matrix4x4 mat = Matrix4x4::Identity();

                mat.arr[0] = cos(radians);
                mat.arr[1] = sin(radians);
                mat.arr[4] = -mat.arr[1];
                mat.arr[5] = mat.arr[0];

                return mat;
            }

            inline static constexpr Matrix4x4 XRotationFromDegrees(const T degrees) {
                EASY_FUNCTION(profiler::colors::Yellow800);
                const T radians = DegToRad(degrees);
                return Matrix4x4::XRotation(radians);
            }

            inline static constexpr Matrix4x4 YRotationFromDegrees(const T degrees) {
                EASY_FUNCTION(profiler::colors::Yellow800);
                const T radians = DegToRad(degrees);
                return Matrix4x4::YRotation(radians);
            }

            inline static constexpr Matrix4x4 ZRotationFromDegrees(const T degrees) {
                EASY_FUNCTION(profiler::colors::Yellow800);
                const T radians = DegToRad(degrees);
                return Matrix4x4::ZRotation(radians);
            }

            inline static constexpr Matrix4x4 Scaling(const Vector3<T> scaling) {
                EASY_FUNCTION(profiler::colors::Yellow800);
                Matrix4x4 mat = Matrix4x4::Identity();

                mat.arr[0] = scaling.x;
                mat.arr[5] = scaling.y;
                mat.arr[10] = scaling.z;

                return mat;
            }

            inline static constexpr Matrix4x4 View(Vector3<T> origin, Vector3<T> right = Vector3<T>::Right(), Vector3<T> up = Vector3<T>::Up(), Vector3<T> forwards = Vector3<T>::Forwards()) {
                EASY_FUNCTION(profiler::colors::Yellow800);
                Orthonormalize(right, up, forwards);

                Vector3<T> position = {
                    static_cast<T>(origin.Dot(right)),
                    static_cast<T>(origin.Dot(up)),
                    static_cast<T>(origin.Dot(forwards)),
                };

                Matrix4x4 mat = Matrix4x4::Identity();
                mat.vec[0] = right;
                mat.vec[1] = up;
                mat.vec[2] = forwards;

                mat = Transpose(mat);
                mat.vec[3] = -position;
                mat.arr[15] = 1.0;

                return mat;
            }

            inline static constexpr Matrix4x4 ProjectionFoVRadians(const double fovRadians, const double aspectRatio, const double nearPlane, const double farPlane) {
                EASY_FUNCTION(profiler::colors::Yellow800);
                Matrix4x4 mat = Matrix4x4::Identity();

                const double a = tan(fovRadians / 2.0);

                mat.arr[0] = 1.0 / (aspectRatio * a);
                mat.arr[5] = -(1.0 / a);
                mat.arr[10] = farPlane / (farPlane - nearPlane);
                mat.arr[11] = 1.0; 
                mat.arr[14] = -farPlane * nearPlane / (farPlane - nearPlane);
                mat.arr[15] = 0.0; 

                return mat;
            }

            inline static constexpr Matrix4x4 ProjectionFoVDegrees(const T fovDegrees, const T aspectRatio, const T nearPlane, const T farPlane) {
                EASY_FUNCTION(profiler::colors::Yellow800);
                const T fovRadians = DegToRad(fovDegrees);
                return ProjectionFoVRadians(fovRadians, aspectRatio, nearPlane, farPlane);
            }

            inline static constexpr Matrix4x4 ProjectionOrthographic(const T top, const T bottom, const T left, const T right, const T farPlane, const T nearPlane) {
                EASY_FUNCTION(profiler::colors::Yellow800);
                Matrix4x4 mat = Matrix4x4::Identity();

                const T x = (right - left);
                const T y = (top - bottom);
                const T z = (farPlane - nearPlane);

                mat[0] = 2.0 / x;
                mat[5] = 2.0 / y;
                mat[10] = -2.0 / z;
                mat[12] = -((right + left) / x);
                mat[13] = -((top + bottom) / y);
                mat[14] = -((farPlane + nearPlane) / z);

                return mat;
            }


            inline static constexpr Matrix4x4 Transpose(const Matrix4x4& matrix) {
                EASY_FUNCTION(profiler::colors::Yellow800);
                Matrix4x4 mat = matrix;

                //Preserve 0, 5, 10, 15
                mat.arr[1] = matrix.arr[4];
                mat.arr[2] = matrix.arr[8];
                mat.arr[3] = matrix.arr[12];
                mat.arr[4] = matrix.arr[1];
                mat.arr[6] = matrix.arr[9];
                mat.arr[7] = matrix.arr[13];
                mat.arr[8] = matrix.arr[2];
                mat.arr[9] = matrix.arr[6];
                mat.arr[11] = matrix.arr[14];
                mat.arr[12] = matrix.arr[3];
                mat.arr[13] = matrix.arr[7];
                mat.arr[14] = matrix.arr[11];

                return mat;
            }


            inline static constexpr Matrix4x4 Inverse(const Matrix4x4& matrix, bool& inverseExists) {
                EASY_FUNCTION(profiler::colors::Yellow800);

                Matrix4x4 inv = {};
                T det;
                int i;

                inv.arr[0] = matrix.arr[5] * matrix.arr[10] * matrix.arr[15] -
                    matrix.arr[5] * matrix.arr[11] * matrix.arr[14] -
                    matrix.arr[9] * matrix.arr[6] * matrix.arr[15] +
                    matrix.arr[9] * matrix.arr[7] * matrix.arr[14] +
                    matrix.arr[13] * matrix.arr[6] * matrix.arr[11] -
                    matrix.arr[13] * matrix.arr[7] * matrix.arr[10];

                inv.arr[4] = -matrix.arr[4] * matrix.arr[10] * matrix.arr[15] +
                    matrix.arr[4] * matrix.arr[11] * matrix.arr[14] +
                    matrix.arr[8] * matrix.arr[6] * matrix.arr[15] -
                    matrix.arr[8] * matrix.arr[7] * matrix.arr[14] -
                    matrix.arr[12] * matrix.arr[6] * matrix.arr[11] +
                    matrix.arr[12] * matrix.arr[7] * matrix.arr[10];

                inv.arr[8] = matrix.arr[4] * matrix.arr[9] * matrix.arr[15] -
                    matrix.arr[4] * matrix.arr[11] * matrix.arr[13] -
                    matrix.arr[8] * matrix.arr[5] * matrix.arr[15] +
                    matrix.arr[8] * matrix.arr[7] * matrix.arr[13] +
                    matrix.arr[12] * matrix.arr[5] * matrix.arr[11] -
                    matrix.arr[12] * matrix.arr[7] * matrix.arr[9];

                inv.arr[12] = -matrix.arr[4] * matrix.arr[9] * matrix.arr[14] +
                    matrix.arr[4] * matrix.arr[10] * matrix.arr[13] +
                    matrix.arr[8] * matrix.arr[5] * matrix.arr[14] -
                    matrix.arr[8] * matrix.arr[6] * matrix.arr[13] -
                    matrix.arr[12] * matrix.arr[5] * matrix.arr[10] +
                    matrix.arr[12] * matrix.arr[6] * matrix.arr[9];

                inv.arr[1] = -matrix.arr[1] * matrix.arr[10] * matrix.arr[15] +
                    matrix.arr[1] * matrix.arr[11] * matrix.arr[14] +
                    matrix.arr[9] * matrix.arr[2] * matrix.arr[15] -
                    matrix.arr[9] * matrix.arr[3] * matrix.arr[14] -
                    matrix.arr[13] * matrix.arr[2] * matrix.arr[11] +
                    matrix.arr[13] * matrix.arr[3] * matrix.arr[10];

                inv.arr[5] = matrix.arr[0] * matrix.arr[10] * matrix.arr[15] -
                    matrix.arr[0] * matrix.arr[11] * matrix.arr[14] -
                    matrix.arr[8] * matrix.arr[2] * matrix.arr[15] +
                    matrix.arr[8] * matrix.arr[3] * matrix.arr[14] +
                    matrix.arr[12] * matrix.arr[2] * matrix.arr[11] -
                    matrix.arr[12] * matrix.arr[3] * matrix.arr[10];

                inv.arr[9] = -matrix.arr[0] * matrix.arr[9] * matrix.arr[15] +
                    matrix.arr[0] * matrix.arr[11] * matrix.arr[13] +
                    matrix.arr[8] * matrix.arr[1] * matrix.arr[15] -
                    matrix.arr[8] * matrix.arr[3] * matrix.arr[13] -
                    matrix.arr[12] * matrix.arr[1] * matrix.arr[11] +
                    matrix.arr[12] * matrix.arr[3] * matrix.arr[9];

                inv.arr[13] = matrix.arr[0] * matrix.arr[9] * matrix.arr[14] -
                    matrix.arr[0] * matrix.arr[10] * matrix.arr[13] -
                    matrix.arr[8] * matrix.arr[1] * matrix.arr[14] +
                    matrix.arr[8] * matrix.arr[2] * matrix.arr[13] +
                    matrix.arr[12] * matrix.arr[1] * matrix.arr[10] -
                    matrix.arr[12] * matrix.arr[2] * matrix.arr[9];

                inv.arr[2] = matrix.arr[1] * matrix.arr[6] * matrix.arr[15] -
                    matrix.arr[1] * matrix.arr[7] * matrix.arr[14] -
                    matrix.arr[5] * matrix.arr[2] * matrix.arr[15] +
                    matrix.arr[5] * matrix.arr[3] * matrix.arr[14] +
                    matrix.arr[13] * matrix.arr[2] * matrix.arr[7] -
                    matrix.arr[13] * matrix.arr[3] * matrix.arr[6];

                inv.arr[6] = -matrix.arr[0] * matrix.arr[6] * matrix.arr[15] +
                    matrix.arr[0] * matrix.arr[7] * matrix.arr[14] +
                    matrix.arr[4] * matrix.arr[2] * matrix.arr[15] -
                    matrix.arr[4] * matrix.arr[3] * matrix.arr[14] -
                    matrix.arr[12] * matrix.arr[2] * matrix.arr[7] +
                    matrix.arr[12] * matrix.arr[3] * matrix.arr[6];

                inv.arr[10] = matrix.arr[0] * matrix.arr[5] * matrix.arr[15] -
                    matrix.arr[0] * matrix.arr[7] * matrix.arr[13] -
                    matrix.arr[4] * matrix.arr[1] * matrix.arr[15] +
                    matrix.arr[4] * matrix.arr[3] * matrix.arr[13] +
                    matrix.arr[12] * matrix.arr[1] * matrix.arr[7] -
                    matrix.arr[12] * matrix.arr[3] * matrix.arr[5];

                inv.arr[14] = -matrix.arr[0] * matrix.arr[5] * matrix.arr[14] +
                    matrix.arr[0] * matrix.arr[6] * matrix.arr[13] +
                    matrix.arr[4] * matrix.arr[1] * matrix.arr[14] -
                    matrix.arr[4] * matrix.arr[2] * matrix.arr[13] -
                    matrix.arr[12] * matrix.arr[1] * matrix.arr[6] +
                    matrix.arr[12] * matrix.arr[2] * matrix.arr[5];

                inv.arr[3] = -matrix.arr[1] * matrix.arr[6] * matrix.arr[11] +
                    matrix.arr[1] * matrix.arr[7] * matrix.arr[10] +
                    matrix.arr[5] * matrix.arr[2] * matrix.arr[11] -
                    matrix.arr[5] * matrix.arr[3] * matrix.arr[10] -
                    matrix.arr[9] * matrix.arr[2] * matrix.arr[7] +
                    matrix.arr[9] * matrix.arr[3] * matrix.arr[6];

                inv.arr[7] = matrix.arr[0] * matrix.arr[6] * matrix.arr[11] -
                    matrix.arr[0] * matrix.arr[7] * matrix.arr[10] -
                    matrix.arr[4] * matrix.arr[2] * matrix.arr[11] +
                    matrix.arr[4] * matrix.arr[3] * matrix.arr[10] +
                    matrix.arr[8] * matrix.arr[2] * matrix.arr[7] -
                    matrix.arr[8] * matrix.arr[3] * matrix.arr[6];

                inv.arr[11] = -matrix.arr[0] * matrix.arr[5] * matrix.arr[11] +
                    matrix.arr[0] * matrix.arr[7] * matrix.arr[9] +
                    matrix.arr[4] * matrix.arr[1] * matrix.arr[11] -
                    matrix.arr[4] * matrix.arr[3] * matrix.arr[9] -
                    matrix.arr[8] * matrix.arr[1] * matrix.arr[7] +
                    matrix.arr[8] * matrix.arr[3] * matrix.arr[5];

                inv.arr[15] = matrix.arr[0] * matrix.arr[5] * matrix.arr[10] -
                    matrix.arr[0] * matrix.arr[6] * matrix.arr[9] -
                    matrix.arr[4] * matrix.arr[1] * matrix.arr[10] +
                    matrix.arr[4] * matrix.arr[2] * matrix.arr[9] +
                    matrix.arr[8] * matrix.arr[1] * matrix.arr[6] -
                    matrix.arr[8] * matrix.arr[2] * matrix.arr[5];

                det = matrix.arr[0] * inv.arr[0] + matrix.arr[1] * inv.arr[4] + matrix.arr[2] * inv.arr[8] + matrix.arr[3] * inv.arr[12];

                if (det == 0) {
                    inverseExists = false;
                    return Matrix4x4::Identity();
                }

                inverseExists = true;
                det = 1.0 / det;

                for (i = 0; i < 16; i++) {
                    inv.arr[i] = inv.arr[i] * det;
                }

                return inv;
            }

        };


    }
}

#endif