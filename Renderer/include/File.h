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
#include <vector> 

namespace VKR {
    namespace IO {

        /*
#ifdef _WIN32
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
#ifdef _WIN32
            void* osHandle;
#endif
        };
        */

        /**
         * @brief Tests whether a file already exists or not.
         * @param filePath A path to a file to test. 
         * @return true if the given file can be found, false otherwise.  
        */
        const bool FileExists(const char* filePath);

        /**
         * @brief Reads a binary file into a blob.
         * @param filePath Path to the file to read.
         * @param blob A byte array to hold the file's contents. 
         * @return SUCCESS on successful read, FAILED otherwise. 
        */
        Status ReadFile(const char* filePath, std::vector<char>& blob);

        /**
         * @brief Attempts to Write data into a given file. 
         * @param filePath Path to the file to write into. 
         * @param pData pointer to the data to write. 
         * @param size number of bytes to write. 
         * @return SUCCESS on successful write, FAILED otherwise. 
        */
        Status WriteFile(const char* filePath, const void* pData, const size_t size);


        Status CreateDirectory(const char* dirPath);
        Status RemoveDirectory(const char* dirPath);

        //const Directory CurrentDirectory();



    }
}


#endif