#include <assert.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "../include/VKR/Logger.h"
#include "../include/VKR/Types.h"


void VKR::Log::Print(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _Output(ELogColour::WHITE, stdout, fmt, args);
    va_end(args);
}

void VKR::Log::Debug(const char* fmt, ...)
{
#if VKR_DEBUG
    va_list args;
    va_start(args, fmt);
    _Output(ELogColour::LIGHTGREEN, stdout, "Debug:   ");
    _Output(ELogColour::LIGHTGREEN, stdout, fmt, args);
    va_end(args);
#endif
}

void VKR::Log::Message(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _Output(ELogColour::LIGHTCYAN, stdout, "Message: ");
    _Output(ELogColour::LIGHTCYAN, stdout, fmt, args);
    va_end(args);
}

void VKR::Log::Success(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _Output(ELogColour::GREEN, stdout, "Success: ");
    _Output(ELogColour::GREEN, stdout, fmt, args);
    va_end(args);
}

void VKR::Log::Failure(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _Output(ELogColour::BROWN, stdout, "Failure: ");
    _Output(ELogColour::BROWN, stdout, fmt, args);
    va_end(args);
}

void VKR::Log::Warning(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _Output(ELogColour::YELLOW, stdout, "Warning: ");
    _Output(ELogColour::YELLOW, stdout, fmt, args);
    va_end(args);
}

void VKR::Log::Error(const char* file, int line, const char* function, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _Output(ELogColour::LIGHTRED, stderr, "Error\nFILE: %s\n\tLINE: %d\n\tFUNCTION: %s\n", file, line, function);
    _Output(ELogColour::LIGHTRED, stderr, fmt, args);
    va_end(args);
}

void VKR::Log::Fatal(const char* file, int line, const char* function, const bool shouldBreak, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _Output(ELogColour::RED, stderr, "FATAL ERROR\nFILE: %s\n\tLINE: %d\n\tFUNCTION: %s\n", file, line, function);
    _Output(ELogColour::RED, stderr, fmt, args);
    va_end(args);

    if (shouldBreak) {
        assert(false && "A Fatal Error has Occurred!");
    }
}



void VKR::Log::_Output(ELogColour colour, FILE* stream, const char* fmt, va_list args) {

    //Change the output colour 
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (WORD)colour);
#else //Assume the program is running on linux
    switch (colour) {
    case ELogColour::RED:
        fprintf(stream, "\033[31m");
        break;

    case ELogColour::GREEN:
        fprintf(stream, "\033[32m");
        break;


    case ELogColour::YELLOW:
    case ELogColour::BROWN:
        fprintf(stream, "\033[33m");
        break;


    case ELogColour::BLUE:
        fprintf(stream, "\033[34m");
        break;


    case ELogColour::CYAN:
        fprintf(stream, "\033[36m");
        break;


    case ELogColour::WHITE:
        fprintf(stream, "\033[37m");
        break;

    default:
        fprintf(stream, "\033[35m");
        break;
    }
#endif

    vfprintf(stream, fmt, args);
    //Reset the output colour
#ifdef _WIN32
    SetConsoleTextAttribute(hConsole, (WORD)ELogColour::WHITE);
#else
    fprintf(stream, "\033[0m");
#endif
}

void VKR::Log::_Output(ELogColour colour, FILE* stream, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _Output(colour, stream, fmt, args);
    va_end(args);
}

