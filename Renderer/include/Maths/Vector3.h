#ifndef __MATH_VECTOR3_H
#define __MATH_VECTOR3_H

/**
*   @file Vector3.h
*   @author Ewan Burnett (EwanBurnettSK@outlook.com)
*   @date 2024/04/23
*/


#include <type_traits>

namespace VKR {
    namespace Math {
        /**
         * @brief A Two-Component continuous Vector
         * @tparam T The type to contain within the vector. This is stored as a union, with {x, y}, {u, v} and array index members.
         * @note The size of a Vector2 is always sizeof(T) * 2.
         * @remark Data alignment is implementation defined.
        */
        template<typename T>
        struct Vector3 {
            Vector3(T X = static_cast<T>(0), T Y = static_cast<T>(0), T Z = static_cast<T>(0)) {
                static_assert(std::is_arithmetic<T>(), "Error: T is not Arithmetic!\n");
                x = X;
                y = Y;
                z = Z;
            }

            union {
                struct { T x, y, z; };
                struct { T u, v, w; };
                T arr[3];
            };

            friend Vector3<T> operator +(Vector3<T> lhs, const Vector3<T>& rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z }; }
            friend Vector3<T> operator -(Vector3<T> lhs, const Vector3<T>& rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z }; }
            friend Vector3<T> operator -(Vector3<T> lhs) {
                return { -lhs.x, -lhs.y, -lhs.z };
            }
            friend Vector3<T> operator *(Vector3<T> lhs, const T& rhs) { return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs }; }
            friend Vector3<T> operator /(Vector3<T> lhs, const T& rhs) { return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs }; }

            inline Vector3& operator +=(const Vector3<T>& rhs) { this->x += rhs.x; this->y += rhs.y; this->z += rhs.z; return *this; }
            inline Vector3& operator -=(const Vector3<T>& rhs) { this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z; return *this; }
            inline Vector3& operator *=(const Vector3<T>& rhs) { this->x *= rhs.x; this->y *= rhs.y; this->z *= rhs.z; return *this; }
            inline Vector3& operator /=(const Vector3<T>& rhs) { this->x /= rhs.x; this->y /= rhs.y; this->z /= rhs.z; return *this; }

            inline Vector3& operator +=(const T& rhs) { this->x += rhs; this->y += rhs; this->z += rhs; return *this; }
            inline Vector3& operator -=(const T& rhs) { this->x -= rhs; this->y -= rhs; this->z -= rhs; return *this; }
            inline Vector3& operator *=(const T& rhs) { this->x *= rhs; this->y *= rhs; this->z *= rhs; return *this; }
            inline Vector3& operator /=(const T& rhs) { this->x /= rhs; this->y /= rhs; this->z /= rhs; return *this; }

            friend bool operator ==(const Vector3<T>& lhs, const Vector3<T>& rhs) { return { lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z }; }
            friend bool operator !=(const Vector3<T>& lhs, const Vector3<T>& rhs) { return !(lhs == rhs); }

            /**
             * @brief Equivalent to Vector3<T>(0, 1, 0)
             * @return A direction Vector pointing Upwards
            */
            inline static constexpr Vector3<T> Up() { return{ static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0) }; }

            /**
             * @brief Equivalent to Vector3<T>(0, -1, 0)
             * @return A direction Vector pointing Upwards
            */
            inline static constexpr Vector3<T> Down() { return{ static_cast<T>(0.0), static_cast<T>(-1.0), static_cast<T>(0.0) }; }

            /**
             * @brief Equivalent to Vector3<T>(-1, 0, 0)
             * @return A direction Vector pointing Left
            */
            inline static constexpr Vector3<T> Left() { return{ static_cast<T>(-1.0), static_cast<T>(0.0), static_cast<T>(0.0) }; }

            /**
             * @brief Equivalent to Vector3<T>(1, 0, 0)
             * @return A direction Vector pointing Right
            */
            inline static constexpr Vector3<T> Right() { return{ static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0) }; }

            /**
             * @brief Equivalent to Vector3<T>(0, 0, 1)
             * @return A direction Vector pointing Forwards
            */
            inline static constexpr Vector3<T> Forwards() { return{ static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0) }; }

            /**
             * @brief Equivalent to Vector3<T>(0, 0, -1)
             * @return A direction Vector pointing Backwards
            */
            inline static constexpr Vector3<T> Backwards() { return{ static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(-1.0) }; }



            /**
             * @brief Computes the dot product of two vectors.
            */
            inline double Dot(const Vector3<T>& other) { return (double)((x * other.x) + (y * other.y) + (z * other.z)); }

            /**
             * @brief Computes the Magnitude of a Vector.
            */
            inline double Length() { return sqrt((x * x) + (y * y) + (z * z)); }

            /**
             * @brief Computes the Squared Length of a Vector.
            */
            inline double LengthSquared() { return (x * x) + (y * y) + (z * z); }

            /**
             * @brief Returns the Normalized form of a vector, dividing each component by its length.
             * @return The normalized vector.
            */
            inline Vector3 Normalize() { return (*this / this->Length()); }

            /**
             * @brief Computes the Cross product of two vectors.
            */
            inline Vector3 Cross(const Vector3<T>& other) {
                return { (x * other.z) - (other.y * z), -((x * other.z) - (other.x * z)), (x * other.y) - (other.x * y) };
            };

            /**
             * @brief Sets each component of this Vector to a value.
             * @param val The value to set.
            */
            inline void Set(const T& val) { x = val; y = val; z = val; }


        };
        typedef Vector3<int> Vector3i;
        typedef Vector3<float> Vector3f;
        typedef Vector3<double> Vector3d;
        template class VKR::Math::Vector3<int>;
        template class VKR::Math::Vector3<float>;
        template class VKR::Math::Vector3<double>;
    }

}
#endif