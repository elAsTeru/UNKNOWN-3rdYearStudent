#include "Logger.h"
#include <cstdio>
#include <cstdarg>
#include <Windows.h>

void OutputLog(const char* _Format, ...)
{
    char msg[2048];
    memset(msg, '\0', sizeof(msg));
    va_list arg;

    va_start(arg, _Format);
    vsprintf_s(msg, _Format, arg);
    va_end(arg);

    // コンソールに出力.
    printf_s("%s", msg);

    // Visual Studioの出力ウィンドウにも表示.
    OutputDebugStringA(msg);
}