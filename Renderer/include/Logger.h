#ifndef __VKRENDERER_LOGGER_H
#define __VKRENDERER_LOGGER_H
/**
*   @file Logger.h
*   @brief Console Logging Utilities
*   @author Ewan Burnett (EwanBurnettSK@Outlook.com)
*   @date 2024/04/23
*/

#include <cstdio>
#include <cstdarg>

//Use the equivalent macro on Microsoft compilers
#ifdef _MSC_VER 
#define __PRETTY_FUNCTION__ __FUNCSIG__ 
#endif

namespace VKR {
    class Log {
    public:
        enum class ELogColour {
            BLACK = 0,
            BLUE,
            GREEN,
            CYAN,
            RED,
            MAGENTA,
            BROWN,
            LIGHTGRAY,
            DARKGRAY,
            LIGHTBLUE,
            LIGHTGREEN,
            LIGHTCYAN,
            LIGHTRED,
            LIGHTMAGENTA,
            YELLOW,
            WHITE,
        };

        static void Print(const char* fmt, ...);
        static void Debug(const char* fmt, ...);
        static void Message(const char* fmt, ...);
        static void Success(const char* fmt, ...);
        static void Failure(const char* fmt, ...);
        static void Warning(const char* fmt, ...);
        static void Error(const char* file, int line, const char* function, const char* fmt, ...);
        static void Fatal(const char* file, int line, const char* function, const bool shouldBreak, const char* fmt, ...);

    private:
        static void _Output(ELogColour colour, FILE* stream, const char* fmt, va_list args);
        static void _Output(ELogColour colour, FILE* stream, const char* fmt, ...);
    };
}

#endif