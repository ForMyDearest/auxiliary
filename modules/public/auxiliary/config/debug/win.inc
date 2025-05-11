#ifndef AUXILIARY_HEADER_SCOPE_DEFINING
#   error "this file can only be included in <auxiliary/config/debug.h>"
#endif

#include "../string.h"

#include <Windows.h>

#ifdef UNICODE

#define AUXILIARY_WARNING_WINDOW(message)                                   \
    do {                                                                    \
        MessageBeep(MB_ICONWARNING);                                        \
        HWND hwnd = GetConsoleWindow();                                     \
        MessageBox(hwnd, L ## message, L"Warning", MB_OK | MB_TOPMOST);     \
    } while (0)

#define AUXILIARY_ERROR_WINDOW(message)                                     \
    do {                                                                    \
        MessageBeep(MB_ICONERROR);                                          \
        HWND hwnd = GetConsoleWindow();                                     \
        MessageBox(hwnd, L ## message, L"Error", MB_OK | MB_TOPMOST);       \
        AUXILIARY_DEBUG_BREAK();                                            \
    } while (0)

#else

#define AUXILIARY_WARNING_WINDOW(message)                               \
    do {                                                                \
        MessageBeep(MB_ICONWARNING);                                    \
        HWND hwnd = GetConsoleWindow();                                 \
        MessageBox(hwnd, message, "Warning", MB_OK | MB_TOPMOST);       \
    } while (0)

#define AUXILIARY_ERROR_WINDOW(message)                                 \
    do {                                                                \
        MessageBeep(MB_ICONERROR);                                      \
        HWND hwnd = GetConsoleWindow();                                 \
        MessageBox(hwnd, message, "Error", MB_OK | MB_TOPMOST);         \
        AUXILIARY_DEBUG_BREAK();                                        \
    } while (0)

#endif