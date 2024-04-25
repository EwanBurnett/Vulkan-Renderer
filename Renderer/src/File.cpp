#include "../include/File.h"
#include "../include/Logger.h"
#include <fstream>
#include <easy/profiler.h>

/*
#if defined(_WIN32)
#include <windows.h>
#else
#define MAX_PATH 65536
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
*/

const bool VKR::IO::FileExists(const char* filePath)
{
    EASY_FUNCTION(profiler::colors::Blue600);
    FILE* file = fopen(filePath, "r");
    if (file) {
        fclose(file);
        return true;
    }
    else {
        return false;
    }

}

VKR::Status VKR::IO::ReadFile(const char* filePath, std::vector<char>& blob)
{
    EASY_FUNCTION(profiler::colors::Blue600);
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        Log::Warning("[VKR]\tFailed to open file \"%s\" for reading.\n", filePath);
        return Status::FAILED;
    }

    size_t fileSize = (size_t)file.tellg();
    blob.resize(fileSize);

    file.seekg(0);
    file.read(blob.data(), fileSize);

    file.close();

    return Status::SUCCESS;
}

VKR::Status VKR::IO::WriteFile(const char* filePath, const void* pData, const size_t size)
{
    EASY_FUNCTION(profiler::colors::Blue600);
    if (!pData) {
        Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "[VKR]\tWriteFile() was called, but pData was nullptr!\n"); 
        return Status::FAILED; 
    }

    std::ofstream file(filePath, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        Log::Warning("[VKR]\tFailed to open file \"%s\" for writing.\n", filePath);
        return Status::FAILED;
    }
    file.write((char*)pData, size);
    file.close();

    return Status::SUCCESS;
}

VKR::Status VKR::IO::CreateDirectory(const char* dirPath)
{
    EASY_FUNCTION(profiler::colors::Blue600);
    return Status::NOT_IMPLEMENTED;
}

VKR::Status VKR::IO::RemoveDirectory(const char* dirPath)
{
    EASY_FUNCTION(profiler::colors::Blue600);
    return Status::NOT_IMPLEMENTED;
}
