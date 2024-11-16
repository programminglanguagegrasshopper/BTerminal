# BTerminal
=======================

BTerminal is a simple terminal made in C that uses some of the WinAPI libraries and C libraries,
like `windows.h`, `stdlib.h`, etc.

## Libraries for runtime
BTerminal also needs some libraries that help in the runtime, like for DPI Awareness, or windows.
Here is all of them:
`windows.h` - not necessary, but still recommended.
`libgcc_s_dw2-1.dll` - **Necessary for runtime, helps in C code and compiling.**
This library comes along MinGW/GCC. Don't worry, the library is in the folder.
Please report issues if you find any.

## Explanation
`windows.h` is a library provided by Microsoft, it also comes along with the multiplatform MinGW
package. Here are some uses:
```c
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

void enableANSI() { // Enables ANSI if running on unsupported ANSI program
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
```
The code above turns on ANSI to change colors in the terminal, that uses WinAPI and
`ENABLE_VIRTUAL_TERMINAL_PROCESSING` to change.

```c
#include <windows.h>

// Function to process Windows messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
```
This code is to process Windows messages, and uses `hwnd`, `uMsg`, `wParam` and `lParam`.
