#ifndef __MATH_MATRIX_H
#define __MATH_MATRIX_H
#include "Vector3.h"
#include "Vector4.h"

/**
*   @file Matrix.h
*   @author Ewan Burnett (EwanBurnettSK@outlook.com)
*   @date 2024/02/23
*/

namespace VKR {
    namespace Math {
    
        template<typename T = float>
        struct Matrix4x4 {
            Matrix4x4() {
                //*this = Matrix4x4::Identity(); 
            }

            union {
                Vector4<T> vec[4];
                T arr[16];
            };

            /**
             * @return Returns the 4x4 Identity Matrix.
            */
            inline static constexpr Matrix4x4& Identity() {
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
             * @brief Builds a 4x4 Translation Matrix from a Vector.  
             * @param translation 
             * @return 
            */
            inline static constexpr Matrix4x4& Translation(const Vector3<T> translation) {
                Matrix4x4 mat = Matrix4x4::Identity(); 

                mat.arr[12] = translation.x;
                mat.arr[13] = translation.y;
                mat.arr[14] = translation.z;

                return mat;
            }
        };


    }
}

#endif