#ifndef __MATH_VECTOR2_H
#define __MATH_VECTOR2_H
/**
*   @file Vector2.h
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
        struct Vector2 {
            Vector2(T X = static_cast<T>(0), T Y = static_cast<T>(0)) {
                x = X;
                y = Y;
            }

            union {
                struct { T x, y; };
                struct { T u, v; };
                T arr[2];
            };

            friend Vector2<T> operator +(Vector2<T> lhs, const Vector2<T>& rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y }; }
            friend Vector2<T> operator -(Vector2<T> lhs, const Vector2<T>& rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y }; }
            friend Vector2<T> operator *(Vector2<T> lhs, const T& rhs) { return { lhs.x * rhs, lhs.y * rhs }; }
            friend Vector2<T> operator /(Vector2<T> lhs, const T& rhs) { return { lhs.x / rhs, lhs.y / rhs }; }

            inline Vector2& operator +=(const Vector2<T>& rhs) { this->x += rhs.x; this->y += rhs.y; return *this; }
            inline Vector2& operator -=(const Vector2<T>& rhs) { this->x -= rhs.x; this->y -= rhs.y; return *this; }
            inline Vector2& operator *=(const Vector2<T>& rhs) { this->x *= rhs.x; this->y *= rhs.y; return *this; }
            inline Vector2& operator /=(const Vector2<T>& rhs) { this->x /= rhs.x; this->y /= rhs.y; return *this; }

            inline Vector2& operator +=(const T& rhs) { this->x += rhs; this->y += rhs; return *this; }
            inline Vector2& operator -=(const T& rhs) { this->x -= rhs; this->y -= rhs; return *this; }
            inline Vector2& operator *=(const T& rhs) { this->x *= rhs; this->y *= rhs; return *this; }
            inline Vector2& operator /=(const T& rhs) { this->x /= rhs; this->y /= rhs; return *this; }

            friend bool operator ==(const Vector2<T>& lhs, const Vector2<T>& rhs) { return { lhs.x == rhs.x && lhs.y == rhs.y }; }
            friend bool operator !=(const Vector2<T>& lhs, const Vector2<T>& rhs) { return !(lhs == rhs); }


            /**
             * @brief Equivalent to Vector2<T>(0, 1)
             * @return A direction Vector pointing Upwards
            */
            inline const Vector2<T> Up() const { return{ static_cast<T>(0.0), static_cast<T>(1.0) }; }

            /**
             * @brief Equivalent to Vector2<T>(0, -1)
             * @return A direction Vector pointing Down
            */
            inline const Vector2<T> Down() const { return{ static_cast<T>(0.0), static_cast<T>(-1.0) }; }

            /**
             * @brief Equivalent to Vector2<T>(-1, 0)
             * @return A direction Vector pointing Left
            */
            inline const Vector2<T> Left() const { return{ static_cast<T>(-1.0), static_cast<T>(0.0) }; }

            /**
             * @brief Equivalent to Vector2<T>(1, 0)
             * @return A direction Vector pointing Right
            */
            inline const Vector2<T> Right() const { return{ static_cast<T>(1.0), static_cast<T>(0.0) }; }

            /**
             * @brief Computes the dot product of two vectors.
            */
            inline double Dot(const Vector2<T>& other) { return (double)(x * other.x + y * other.y); }

            /**
             * @brief Computes the Magnitude of a Vector.
            */
            inline double Length() const { return sqrt((x * x) + (y * y)); }

            /**
             * @brief Computes the squared length of a vector.
             * @return
            */
            inline double LengthSquared() const { return (x * x) + (y * y); }

            /**
             * @brief Returns the Normalized form of a vector, dividing each component by its length.
             * @return The normalized vector.
            */
            inline const Vector2<T> Normalize() { return Vector2<T>(*this / this->Length()); }

            /**
             * @brief Sets each component of this Vector to a value.
             * @param val The value to set.
            */
            inline void Set(const T& val) { x = val; y = val; }
        };

        typedef Vector2<int> Vector2i;
        typedef Vector2<float> Vector2f;
        typedef Vector2<double> Vector2d;

        template struct VKR::Math::Vector2<int>;
        template struct VKR::Math::Vector2<float>;
        template struct VKR::Math::Vector2<double>;

    }
}
#endif