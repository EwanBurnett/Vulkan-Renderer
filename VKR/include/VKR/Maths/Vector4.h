#ifndef __MATH_VECTOR4_H
#define __MATH_VECTOR4_H

/**
*   @file Vector4.h
*   @author Ewan Burnett (EwanBurnettSK@outlook.com)
*   @date 2024/04/23
*/

#include <type_traits>

namespace VKR {
    namespace Math {
        /**
         * @brief A Four-Component continuous Vector
         * @tparam T The type to contain within the vector. This is stored as a union, with {x, y, z, w} and array index members.
         * @note The size of a Vector4 is always sizeof(T) * 4.
         * @remark Data alignment is implementation defined.
        */
        template<typename T>
        struct Vector4 {
     
            Vector4(Vector3<T> vec) {
                    x = vec.x;
                    y = vec.y;
                    z = vec.z; 
                    w = static_cast<T>(0.0); 
                }


            Vector4(T X = static_cast<T>(0), T Y = static_cast<T>(0), T Z = static_cast<T>(0), T W = static_cast<T>(0)) {
                x = X;
                y = Y;
                z = Z; 
                w = W; 
            }

            union {
                struct { T x, y, z, w; };
                T arr[4];
            };

            friend Vector4<T> operator +(Vector4<T> lhs, const Vector4<T>& rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y }; }
            friend Vector4<T> operator -(Vector4<T> lhs, const Vector4<T>& rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y }; }
            friend Vector4<T> operator *(Vector4<T> lhs, const T& rhs) { return { lhs.x * rhs, lhs.y * rhs }; }
            friend Vector4<T> operator /(Vector4<T> lhs, const T& rhs) { return { lhs.x / rhs, lhs.y / rhs }; }

            inline Vector4& operator +=(const Vector4<T>& rhs) { this->x += rhs.x; this->y += rhs.y; return *this; }
            inline Vector4& operator -=(const Vector4<T>& rhs) { this->x -= rhs.x; this->y -= rhs.y; return *this; }
            inline Vector4& operator *=(const Vector4<T>& rhs) { this->x *= rhs.x; this->y *= rhs.y; return *this; }
            inline Vector4& operator /=(const Vector4<T>& rhs) { this->x /= rhs.x; this->y /= rhs.y; return *this; }

            inline Vector4& operator +=(const T& rhs) { this->x += rhs; this->y += rhs; return *this; }
            inline Vector4& operator -=(const T& rhs) { this->x -= rhs; this->y -= rhs; return *this; }
            inline Vector4& operator *=(const T& rhs) { this->x *= rhs; this->y *= rhs; return *this; }
            inline Vector4& operator /=(const T& rhs) { this->x /= rhs; this->y /= rhs; return *this; }

            friend bool operator ==(const Vector4<T>& lhs, const Vector4<T>& rhs) { return { lhs.x == rhs.x && lhs.y == rhs.y }; }
            friend bool operator !=(const Vector4<T>& lhs, const Vector4<T>& rhs) { return !(lhs == rhs); }


            /**
             * @brief Computes the dot product of two vectors.
            */
            inline T Dot(const Vector4<T>& other) { return ((x * other.x) + (y * other.y) + (z * other.z) + (w * other.w)); }

            /**
             * @brief Computes the Magnitude of a Vector.
            */
            inline double Length() { return sqrt((x * x) + (y * y)); }

            /**
             * @brief Returns the Normalized form of a vector, dividing each component by its length.
             * @return The normalized vector.
            */
            inline Vector4 Normalize() { return (*this / this->Length()); }

            /**
             * @brief Sets each component of this Vector to a value.
             * @param val The value to set.
            */
            inline void Set(const T& val) { x = val; y = val; z = val; w = val; }
        };

        typedef Vector4<int> Vector4i;
        typedef Vector4<float> Vector4f;
        typedef Vector4<double> Vector4d;

        template struct VKR::Math::Vector4<int>;
        template struct VKR::Math::Vector4<float>;
        template struct VKR::Math::Vector4<double>;

    }
}
#endif