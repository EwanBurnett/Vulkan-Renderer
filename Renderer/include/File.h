#ifndef __VKRENDERER_FILE_H
#define __VKRENDERER_FILE_H
/**
*   @file File.h
*   @brief File I/O Utilities
*   @author Ewan Burnett (EwanBurnettSK@outlook.com)
*   @date 2024/02/23
*/
#include <cstdio> 
#include "Types.h"
#include <vector> //TODO: Replace with Fixed Vector

namespace VKR {
    namespace IO {

#if defined(VKR_WIN32)
        //Windows specific aliasing
        typedef struct __FILETIME {
            uint64_t dwLowDateTime;
            uint64_t dwHighDateTime;
        } FILETIME, * PFILETIME, * LPFILETIME;

        using FileTime == __FILETIME;
#endif

        using FileHandle = FILE*;

        constexpr uint32_t MAX_FILEPATH_LENGTH = 512;

        struct Directory {
            char path[MAX_FILEPATH_LENGTH];
#if defined(VKR_WIN32)
            void* osHandle;
#endif
        };

        const bool FileExists(const char* filePath);

        Status ReadFile(const char* filePath, std::vector<char>& blob);
        Status WriteFile(const char* filePath, const char* pData, const size_t size);


        Status CreateDirectory(const char* dirPath);
        Status RemoveDirectory(const char* dirPath);

        const Directory CurrentDirectory();



    }
}


#endif