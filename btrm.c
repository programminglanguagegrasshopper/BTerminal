#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <direct.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <psapi.h>
#include <conio.h>
#include <lmcons.h>

#include "colors.h"
#include "errhand.h"
#include "resource.h"
#include "splashes.h"

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif // If your PC supports 'ENABLE_VIRTUAL_TERMINAL_PROCESSING', delete it if you want.
// I recommend not deleting anything of this above.

#ifndef _WIN32
#include <unistd.h>
#endif

#ifdef _WIN32
    #include <conio.h>  // For Windows: _getch()
#else
    #include <termios.h> // For Linux/macOS terminal handling
    #include <unistd.h>

    // Function to capture a single key press on Linux/macOS
    int getch() {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#endif

#define EXIT_SUCCESS 0
#define MAX_COMMAND_LENGTH 65535

char prompt[256] = ">> ";  // default prompt
bool isdirprompt = false; // directory prompt flag
bool echoEnabled = true;      // 1 means ON, 0 means OFF
bool showPrompt = true;       // 1 means prompt is visible, 0 means prompt is hidden

// --INI Settings--
bool iniVerboseMode = false;
bool iniFullscreen = false;

// --Terminal Settings/Windows Complementaries--

void enableANSI() {
    // Enables ANSI if running on non-ANSI program

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    SetConsoleMode(hOut, dwMode);
}

int set_clipboard_text(const char *text) {
    const size_t len = strlen(text) + 1;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    if (!hMem) return 0;

    memcpy(GlobalLock(hMem), text, len);
    GlobalUnlock(hMem);

    if (!OpenClipboard(NULL)) {
        GlobalFree(hMem);
        return 0;
    }

    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem); // Ownership of hMem is transferred to the clipboard
    CloseClipboard();
    return 1;
}

// --Commands--

void showHelp() {
    printf("Executing BTerminal version 1x\n");
    printf("Command list and argument help:\n\n");

    printf("[COMMAND] [ARGUMENTS]            [DESCRIPTION]\n\n");
    printf("HELP                           - Show this help message\n");
    printf("#[TEXT]                        - Comment\n\n");

    printf("BEEP                           - Trigger a system beep\n");
    printf("BTRM                           - Reset settings and show welcome message\n\n");
    
    printf("CD                             - Change to another directory or show the current directory\n");
    printf("CHAR                           - Show the character for a given ASCII code\n");
    printf("CHDIR                          - Change to another directory or show the current directory\n");
    printf("CLEAR                          - Clear the screen\n");
    printf("CLIP                           - Copy text to the clipboard\n");
    printf("COL [TXTCOL] [(OPTIONAL)BGCOL] - Change the text or the background color of the terminal\n\n");
    
    printf("DATE                           - Show the current date\n");
    printf("DEL [FILE]                     - Delete a file\n\n");

    printf("ERROR [MSG]                    - Show an error message\n");
    printf("EXIT                           - Exit the terminal\n\n");
    printf("EXMEMUSE                       - Show the memory used by only the current program\n\n");

    printf("FPROMPT [TEXT]                 - Change the terminal prompt to a custom text\n\n");

    printf("GDI [TEXT]                     - Show a blank screen with centered GDI text\n");

    printf("INFO [MSG]                     - Show an information message\n\n");

    printf("LEARN [TOPIC]                  - Learn about a specific topic\n\n");

    printf("MD [DIR]                       - Create a new directory\n");
    printf("MEMINFO                        - Show the current memory information of the operating system\n");
    printf("MEMUSE                         - Show the current memory usage of the operating system\n");
    printf("MKDIR [DIR]                    - Create a new directory\n");
    printf("MKFILE [FILE]                  - Create a new file\n");
    printf("MSG [MSG] [TITLE]              - Show a message box with no icon\n\n");

    printf("NULPAUSE                       - Pause the terminal and wait for the user to type a key without message\n\n");
    printf("PAUSE                          - Pause the terminal and wait for the user to type a key.\n");
    printf("PRINT [TEXT]                   - Print a message to the terminal\n");
    printf("PROMPT [TYPE]                  - Change the terminal prompt\n\n");

    printf("QSTMSG [MSG] [TITLE]           - Show a question message\n\n");

    printf("RAND [MAX]                     - Generate a secure random number between 0 and MAX\n");
    printf("RD [DIR]                       - Delete a directory\n");
    printf("READ [FILE]                    - Read the contents of a file\n");
    printf("RESTART                        - Restart the system in 15 seconds\n");
    printf("RMDIR [DIR]                    - Delete a directory\n");
    printf("RMFILE [FILE]                  - Delete a file\n");
    printf("RUN [PROGRAM]                  - Run a program\n\n");

    printf("SHUTDOWN                       - Shut down the system in 15 seconds\n\n");

    printf("TIME                           - Show the current time\n");
    printf("TITLE [TEXT]                   - Change the terminal window title\n");
    printf("THROWERROR                     - Throw a detailed error message\n\n");

    printf("VER                            - Show the current version of the command line\n");
    printf("VERSION                        - Show the current version of the command line\n\n");

    printf("WARNING [MSG]                  - Show a warning message\n");
    printf("WHOAMI                         - Show the current user's name\n");
    printf("WRITE [FILE] [TEXT]            - Write text to a file\n\n");

    printf("You are allowed to distribute the source code of this program or distribute the program itself.\n");
}



void pauseTerminal() {
    int ch = getch();
}

void printUserName() {
    char userName[UNLEN + 1];
    DWORD userNameLen = UNLEN + 1;

    if (GetUserName(userName, &userNameLen)) {
        printf("Username: %s\n", userName);
    } else {
        printf("Failed to get user name.\n");
    }
}

void clearScreen();

void createBtrmIni() {
    FILE *file = fopen("btrm.ini", "w");
    if (file == NULL) {
        printf("Error creating btrm.ini file.\n");
        return;
    }
    fprintf(file, "; \"[btrm]\" is the main header. Do not change it's name.\n");

    fprintf(file, "[btrm]\n");
    fprintf(file, "verbose=false\n");
    fprintf(file, "fullscreen=false\n");
    fprintf(file, "\n");
    fclose(file);
}

void colorTest() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 0);
    printf(" ");
    SetConsoleTextAttribute(hConsole, BACKGROUND_RED);
    printf(" ");
    SetConsoleTextAttribute(hConsole, BACKGROUND_GREEN);
    printf(" ");
    SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_GREEN);
    printf(" ");
    SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE);
    printf(" ");
    SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_BLUE);
    printf(" ");
    SetConsoleTextAttribute(hConsole, BACKGROUND_GREEN | BACKGROUND_BLUE);
    printf(" ");
    SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
    printf(" ");
    SetConsoleTextAttribute(hConsole, BACKGROUND_INTENSITY);
    printf(" ");
    SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_INTENSITY);
    printf(" ");
    SetConsoleTextAttribute(hConsole, BACKGROUND_GREEN | BACKGROUND_INTENSITY);
    printf(" ");
    SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
    printf(" ");
    SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | BACKGROUND_INTENSITY);
    printf(" ");
    SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY);
    printf(" ");
    SetConsoleTextAttribute(hConsole, BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY);
    printf(" ");
    SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY);
    printf(" ");

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    printf("\n");
}

void drawTextCenter(const char* text) {
    HDC hdc;
    HWND hwnd = GetDesktopWindow();  // Use the desktop window to get the screen size
    RECT rect;
    GetClientRect(hwnd, &rect);  // Get the size of the screen

    hdc = GetDC(hwnd);  // Get device context

    // Select the System font
    HFONT hFont = CreateFont(64, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, 
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
                             DEFAULT_PITCH | FF_DONTCARE, TEXT("System"));

    SelectObject(hdc, hFont);

    // Set text color to black and background to transparent
    SetTextColor(hdc, RGB(0, 0, 0));
    SetBkMode(hdc, TRANSPARENT);

    // Calculate the center position for the text
    SIZE textSize;
    GetTextExtentPoint32(hdc, text, strlen(text), &textSize);  // Get the size of the text

    int x = (rect.right - textSize.cx) / 2;  // Horizontal center
    int y = (rect.bottom - textSize.cy) / 2; // Vertical center

    // Clear the screen by filling the area with white color
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(hdc, &rect, hBrush);

    // Draw the text in the center
    TextOut(hdc, x, y, text, strlen(text));

    // Cleanup
    DeleteObject(hFont);
    DeleteObject(hBrush);
    ReleaseDC(hwnd, hdc);
}

void addToPath() {
    char path[256];
    if (GetModuleFileName(NULL, path, sizeof(path)) == 0) {
        printf("Error: Could not get the path to the program. Error code: %lu\n", GetLastError());
        return;
    }

    // Remove the program name to get the directory
    char *lastSlash = strrchr(path, '\\');
    if (lastSlash != NULL) {
        *lastSlash = '\0';
    }

    // Get the current PATH environment variable
    char *currentPath = getenv("PATH");
    if (currentPath == NULL) {
        MessageBoxA(NULL, 
        "Directory error in the following inspection:\n    char *currentPath = getenv(\"PATH\")\n(LPSTR)currentPath returned null response", 

        "Critical Error", MB_ICONERROR | MB_OK);
        return;
    }

    // Create the new PATH value
    char newPath[1024];
    snprintf(newPath, sizeof(newPath), "%s;%s", path, currentPath);

    // Set the new PATH environment variable
    if (SetEnvironmentVariable("PATH", newPath)) {
        printf("Successfully added the program's directory to the system PATH.\n");
    } else {
        printf("Error: Could not set the PATH environment variable. Error code: %lu\n", GetLastError());
    }
}

void SetupP3() {
    // Get the current path to this program and add it to system path environment variable

    char path[256];
    if (GetModuleFileName(NULL, path, sizeof(path)) == 0) {
        MessageBoxA(NULL, "This program performed an illegal operation and will be shut down.", "Setup", MB_ICONERROR | MB_OK);
        exit(1);
        return;
    }

    char *currentPath = getenv("PATH");
    if (currentPath == NULL) {
        MessageBoxA(NULL, "This program performed an illegal operation and will be shut down.", "Setup", MB_ICONERROR | MB_OK);
        exit(1);
        return;
    }

    fflush(stdout);
    clearScreen();

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    DWORD charsWritten;
    COORD topLeft = {0, 0}; // Top-left corner of the console
    DWORD consoleSize;

    // Get the number of character cells in the current buffer
    GetConsoleScreenBufferInfo(hConsole, &screenBufferInfo);
    consoleSize = screenBufferInfo.dwSize.X * screenBufferInfo.dwSize.Y;

    FillConsoleOutputAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE, consoleSize, topLeft, &charsWritten);
    FillConsoleOutputCharacterA(hConsole, ' ', consoleSize, topLeft, &charsWritten);
    SetConsoleCursorPosition(hConsole, topLeft);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE);
    printf("BTerminal Setup\n");
    printf("==============================\n\n");
    printf("Would you like to add this program to system environment variables:PATH?\n");
    printf("Adding this program to PATH provides direct enter to this program without needing to search for it.\n\n");

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
    printf("    > To exit without setting up BTerminal, press F3.\n\n");
    printf("    + To add this program to PATH, press Y or y.\n\n");
    printf("    + To continue the Setup without adding this program to PATH, press N or n.\n\n\n");

    int chs2 = _getch();  // Get character without needing Enter key
    if (chs2 == 'Y' || chs2 == 'y') {
        char newPath[1024];
        snprintf(newPath, sizeof(newPath), "%s;%s", path, currentPath);

        // Set the new PATH environment variable
        if (SetEnvironmentVariable("PATH", newPath)) {
            printf("Successfully added the program to the system PATH.\n");
        } else {
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | BACKGROUND_BLUE);
            MessageBoxA(NULL, "Could not add the current program to system PATH.\nPress OK to cancel the operation.", "Setup", MB_ICONERROR | MB_OK);
        }
    } else if (chs2 == 'N' || chs2 == 'n') {
        printf("Skipping PATH confirmation.\n");
    } else if (chs2 == 0 || chs2 == 224) { // Check for F3
        chs2 = _getch(); // Get actual key code
        if (chs2 == 61) { // F3 key's scan code
            fflush(stdout);
            clearScreen();
            printf("Exiting setup...\n");
            FillConsoleOutputAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, consoleSize, topLeft, &charsWritten);
            FillConsoleOutputCharacterA(hConsole, ' ', consoleSize, topLeft, &charsWritten);
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            SetConsoleCursorPosition(hConsole, topLeft);

            return;
        }
    } else {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | BACKGROUND_BLUE);
        printf("Invalid input. Continuing without adding program to path.\n");
    }
}

void SetupP2() {
    fflush(stdout);
    clearScreen();

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    DWORD charsWritten;
    COORD topLeft = {0, 0}; // Top-left corner of the console
    DWORD consoleSize;

    // Get the number of character cells in the current buffer
    GetConsoleScreenBufferInfo(hConsole, &screenBufferInfo);
    consoleSize = screenBufferInfo.dwSize.X * screenBufferInfo.dwSize.Y;

    FillConsoleOutputAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE, consoleSize, topLeft, &charsWritten);
    FillConsoleOutputCharacterA(hConsole, ' ', consoleSize, topLeft, &charsWritten);
    SetConsoleCursorPosition(hConsole, topLeft);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE);
    printf("BTerminal Setup\n");
    printf("==============================\n\n");
    printf("Would you like to create an INI file to maintain settings of the terminal such as default fullscreen or default title?\n\n");
    printf("(This file will have comments for easiness of understanding)\n");
    printf("The file syntax is:\n");
    printf("    [header] = a container for settings\n");
    printf("    key = a placeholder for the value\n\n");
    printf("    ; comment = a comment\n");
    printf("    the \"=\" sign = a separator between key and value\n\n");

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
    printf("    > To exit without setting up BTerminal, press F3.\n\n");
    printf("    + To create the file, press Y or y.\n\n");
    printf("    + To continue the Setup without creating the file, press N or n.\n\n\n");

    int chs2 = _getch();  // Get character without needing Enter key
    if (chs2 == 'Y' || chs2 == 'y') {
        createBtrmIni();
        //
    } else if (chs2 == 'N' || chs2 == 'n') {
        printf("Skipping ini file creation.\n");
        //
    } else if (chs2 == 0 || chs2 == 224) { // Check for F3
        chs2 = _getch(); // Get actual key code
        if (chs2 == 61) { // F3 key's scan code
            fflush(stdout);
            clearScreen();
            printf("Exiting setup...\n");
            FillConsoleOutputAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, consoleSize, topLeft, &charsWritten);
            FillConsoleOutputCharacterA(hConsole, ' ', consoleSize, topLeft, &charsWritten);
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            SetConsoleCursorPosition(hConsole, topLeft);

            return;
        }
    } else {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
        printf("Invalid input. Continuing without creating ini file.\n");
        //
    }
}

void setupBTRM() {
    fflush(stdout);
    clearScreen();

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    DWORD charsWritten;
    COORD topLeft = {0, 0}; // Top-left corner of the console
    DWORD consoleSize;

    // Get the number of character cells in the current buffer
    GetConsoleScreenBufferInfo(hConsole, &screenBufferInfo);
    consoleSize = screenBufferInfo.dwSize.X * screenBufferInfo.dwSize.Y;

    FillConsoleOutputAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE, consoleSize, topLeft, &charsWritten);
    FillConsoleOutputCharacterA(hConsole, ' ', consoleSize, topLeft, &charsWritten);
    SetConsoleCursorPosition(hConsole, topLeft);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE);
    printf("BTerminal Setup\n");
    printf("==============================\n\n");
    printf("Welcome to BTerminal advanced setup. Setup will guide you on how to configure the terminal settings.\n");
    printf("In this setup you will learn to change the settings of the program, how to use commands and more.\n\n");

    printf("Thank you for using BTerminal.\n\n");

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
    printf("    > To exit without setting up BTerminal, press F3.\n\n");
    printf("    > To continue the setup, press any other key rather than F3.\n");

    int chs = _getch();  // Get character without needing Enter key

    if (chs == 0 || chs == 224) {  // Special keys (F keys, arrow keys, etc.)
        chs = _getch();  // Read the actual key
        if (chs == 61) {  // F3 key has scan code 61
            fflush(stdout);
            clearScreen();
            printf("Exiting setup...\n");
            FillConsoleOutputAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, consoleSize, topLeft, &charsWritten);
            FillConsoleOutputCharacterA(hConsole, ' ', consoleSize, topLeft, &charsWritten);
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            SetConsoleCursorPosition(hConsole, topLeft);

            return;
        }
    }

        // Continue the setup for all other keys (0-60 and 62-255)
    if (chs >= 0 && chs <= 60 || chs >= 62 && chs <= 255) {
        printf("Continuing setup...\n");
        SetupP2();
    }
}

void shutdownSystem() {
    char input[10];

    // Step 1: Warn the user
    printf("WARNING: This action will shut down the system!\n");
    printf("Do you want to proceed? (Y/N): ");
    fgets(input, 10, stdin);

    // Remove newline character from input
    input[strcspn(input, "\n")] = 0;

    // Step 2: Check for confirmation
    if (strcmp(input, "Y") == 0) {
        printf(RED);
        printf("Shutting down the system...\n");
        printf("15-second timeout (if you need to save things)\n");
        system("shutdown /s /f /t 15");
    } else {
        printf("Shutdown canceled.\n");
    }
}

void restartSystem() {
    char input[10];

    // Step 1: Warn the user
    printf("WARNING: This action will restart the system!\n");
    printf("Do you want to proceed? (Y/N): ");
    fgets(input, 10, stdin);

    // Remove newline character from input
    input[strcspn(input, "\n")] = 0;

    // Step 2: Check for confirmation
    if (strcmp(input, "Y") == 0) {
        printf(RED);
        printf("Restarting the system...\n");
        printf("15-second timeout (if you need to save things)\n");
        system("shutdown /r /f /t 15");
    } else {
        printf("Restart canceled.\n");
    }
}

void showMEMUsage() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);

    if (GlobalMemoryStatusEx(&memInfo)) {
        DWORDLONG totalPhysMemory = memInfo.ullTotalPhys;   // Total physical memory
        DWORDLONG freePhysMemory = memInfo.ullAvailPhys;    // Available physical memory
        DWORDLONG usedPhysMemory = totalPhysMemory - freePhysMemory;  // Currently used physical memory

        // Display memory values in bytes
        printf("Total Physical Memory: %llu bytes\n", totalPhysMemory);
        printf("Currently Used Physical Memory: %llu bytes\n", usedPhysMemory);
    } else {
        printf("Error: Unable to retrieve memory information.\n");
    }
}

void meminfo() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);

    if (GlobalMemoryStatusEx(&memInfo)) {
        DWORDLONG totalPhysMemory = memInfo.ullTotalPhys;       // Total physical memory
        DWORDLONG freePhysMemory = memInfo.ullAvailPhys;        // Available physical memory
        DWORDLONG totalVirtualMemory = memInfo.ullTotalVirtual; // Total virtual memory
        DWORDLONG freeVirtualMemory = memInfo.ullAvailVirtual;  // Available virtual memory

        printf("Total Physical Memory: %llu bytes\n", totalPhysMemory);
        printf("Available Physical Memory: %llu bytes\n", freePhysMemory);
        printf("Total Virtual Memory: %llu bytes\n", totalVirtualMemory);
        printf("Available Virtual Memory: %llu bytes\n", freeVirtualMemory);
    } else {
        printf("Error: Unable to retrieve memory information.\n");
    }
}

void showProgramMEM() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    
    // Get the current process handle
    HANDLE hProcess = GetCurrentProcess();
    if (hProcess == NULL) {
        printf("Failed to get process handle. Error: %lu\n", GetLastError());
        return;
    }

    // Get memory info for the current process
    if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        // PrivateUsage gives the exact memory used by the process
        printf("Memory used by this terminal process: %zu bytes\n", pmc.PrivateUsage);
    } else {
        // If there's an error, display it
        printf("Critical error has occurred in the program. Error: %lu\n", GetLastError());
        changeColor("yellow", NULL);
        printf("Press any key to exit the program...\n");
        pauseTerminal();
        eexit();
    }
    
    // Close the process handle (good practice)
    CloseHandle(hProcess);
}

void changeDirectory(const char *path) {
    if (_chdir(path) != 0) {
        perror("Error changing directory");
    } else {
        char cwd[256];
        if (isdirprompt == true) {
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                strcpy(prompt, cwd);
                strcat(prompt, "> ");  // Directory prompt
                isdirprompt = true;
            } else {
                printf("Error: Could not retrieve current directory.\n");
            }
        }
    }
}

void currentDate() {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[30]; // Buffer to hold the formatted date string

    // Get the current date and time
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Format the date as "DD-MM-YYYY, Weekday DD"
    strftime(buffer, sizeof(buffer), "%d-%m-%Y, %A %d", timeinfo);

    // Print the formatted date
    printf("Current date: %s\n", buffer);
}

void currentTime() {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[10];
    char meridiem[3]; // To hold "AM" or "PM"

    // Get the current time
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Check if it's AM or PM
    if (timeinfo->tm_hour >= 12) {
        strcpy(meridiem, "PM");
        if (timeinfo->tm_hour > 12) {
            timeinfo->tm_hour -= 12; // Convert to 12-hour format
        }
    }
    else {
        strcpy(meridiem, "AM");
        if (timeinfo->tm_hour == 0) {
            timeinfo->tm_hour = 12; // Midnight edge case for 12-hour format
        }
    }

    // Format the time as HH:MM:SS
    strftime(buffer, sizeof(buffer), "%I:%M:%S", timeinfo);

    // Print the formatted time with AM/PM
    printf("Current time: %s %s\n", buffer, meridiem);
}

void clearScreen() {
    printf("\033[H\033[J");

    fflush(stdout);
}

void readFile(const char *filename) {
    FILE *file = fopen(filename, "rb"); // Open in binary mode to support any file type
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    char buffer[1024];
    size_t bytesRead;
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        printf("%s", buffer);
    }

    printf("\n");
    fclose(file);
}

void writeFile(const char *filename, const char *content)
{
    FILE *pF = fopen(filename, "a"); // Open in append mode
    if (pF == NULL)
    {
        perror("Error opening file");
        return;
    }

    fprintf(pF, "%s\n", content); // Append the content with a new line
    fflush(pF);                   // Ensure all data is written to the file
    fclose(pF);
    printf("Content written to %s\n", filename);
}

// Function to create a file
void createFile(char* filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not create file %s\n", filename);
        return;
    }
    fclose(file);
    printf("File \"%s\" created successfully.\n", filename);
}

// Function to delete a file
void deleteFile(char* filename) {
    if (remove(filename) == 0) {
        printf("File \"%s\" deleted successfully.\n", filename);
    } else {
        printf("Error: Could not delete file %s.\n", filename);
    }
}

void flushinb() {
    int c;
    while ((c = getch()) != '\n' && c != EOF) {

    }
}

void charSearch(int charCode) {
    if (charCode < 0 || charCode > 255) {
        printf("Error: Invalid character code. Must be between 0 and 255.\n");
        return;
    }
    
    // Convert the number to the corresponding ASCII character (Codepage 437)
    unsigned char asciiChar = (unsigned char)charCode;
    printf("Character for code %d: %c\n", charCode, asciiChar);
}

void runProgram(const char *program) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // Zero memory for startup info and process info
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Try to create a process for the specified program
    if (!CreateProcess(
            NULL,           // No module name (use command line)
            (LPSTR)program, // Command line (program to run)
            NULL,           // Process handle not inheritable
            NULL,           // Thread handle not inheritable
            FALSE,          // Set handle inheritance to FALSE
            0,              // No creation flags
            NULL,           // Use parent's environment block
            NULL,           // Use parent's starting directory 
            &si,            // Pointer to STARTUPINFO structure
            &pi)            // Pointer to PROCESS_INFORMATION structure
    ) {
        printf("Error: Could not run the program '%s'. Error Code: %lu\n", program, GetLastError());
        return;
    }

    // Wait for the program to finish executing (if needed)
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if (GetLastError() != 0) {
        printf("%X %p\n\n",  GetLastError(), GetLastError());
    } else {
        printf("\n");
    }
}

void displayLoadingBar(int progress, int total) {
    int barWidth = 50;  // Width of the loading bar
    int pos = (progress * barWidth) / total;  // Calculate current position
    
    printf("[");
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) {
            printf("=");
        } else if (i == pos) {
            printf(">");
        } else {
            printf(" ");
        }
    }
    printf("] %d%%\r", (progress * 100) / total);  // Display percentage
    fflush(stdout);
}

void copyFile(const char *source, const char *destination) {
    if (!CopyFile(source, destination, FALSE)) {
        printf("Error: Unable to copy file. Error code: %lu\n", GetLastError());
    } else {
        printf("File copied successfully from %s to %s\n", source, destination);
    }
}

// --Handlers--

void handleCommand(char *command) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    char *arg1 = strtok(command, " "); // Get the argument/s

    if (command == NULL || strlen(command) == 0) {
        return;
    }
    else if (strcmp(arg1, "help") == 0) {
        showHelp();
    } else if (strcmp(arg1, "read") == 0) {
        char *filename = strtok(NULL, ""); // Capture the entire remaining string as the filename
        if (filename == NULL) {
            printf("Error: Filename is missing.\n");
        } else {
            readFile(filename); // Pass the filename to the function
            if (iniVerboseMode) {
                printf("Contents of file readed\n");
            }
        }
    } else if (strcmp(arg1, "write") == 0) {
        char *filename = strtok(NULL, " ");
        char *content = strtok(NULL, "");
        if (filename == NULL || content == NULL) {
            printf("Error: Missing filename or content for write command.\n");
        } else {
            writeFile(filename, content);
        }
    } else if (strcmp(arg1, "chdir") == 0 || strcmp(arg1, "cd") == 0) {
        char *path = strtok(NULL, "");
        if (path == NULL) {
            char chdir_path[256];
            getcwd(chdir_path, sizeof(chdir_path));

            printf("%s\n", chdir_path);
        } else {
            changeDirectory(path);
        }
    } else if (strcmp(arg1, "chdir..") == 0 || strcmp(arg1,"cd..") == 0) {
        changeDirectory("..");
    } else if (strcmp(arg1, "mkdir") == 0 || strcmp(arg1, "md") == 0) {
        char *mkdirName = strtok(NULL, "");
        if (mkdirName == NULL) {
            printf("Error: Directory name is missing.\n");
        } else {
            if (_mkdir(mkdirName) != 0) {
                perror("Error creating directory");
            }
        }
    } else if (strcmp(arg1, "rmdir") == 0 || strcmp(arg1, "rd") == 0) {
        char *rmdirName = strtok(NULL, "");
        if (rmdirName == NULL) {
            printf("Error: Directory name is missing.\n");
        } else {
            if (_rmdir(rmdirName) != 0) {
                perror("Error removing directory");
            }
        }
    } else if (strcmp(arg1, "col") == 0) {
        char *color = strtok(NULL, " ");
        char *bgColor = strtok(NULL, " "); // Background color is optional

        if (color) {
            if (bgColor) {
                changeColor(color, bgColor); // If both color and background are provided
            } else {
                changeColor(color, NULL); // Only change the text color if background is not provided
            }
            if (iniVerboseMode) {
                printf("Changed foreground color and/or background color\n");
            }
        }
        else {
            printf("Error: Foreground color is missing.\7\n");
        }
    } else if (strcmp(arg1, "exit") == 0) {
        eexit();
        // Obtain information and close
    } else if (strcmp(arg1, "clear") == 0 || strcmp(arg1, "cls") == 0) {
        clearScreen();
        fflush(stdout);
    } else if (strcmp(arg1, "print") == 0) {
        char *text = strtok(NULL, "");
        if (text != NULL) {
            printf("%s\n", text); // Output the text after "print"
        }
        if (!text) {
            printf("Error: No text specified\7\n");
        }
    } else if (strcmp(arg1, "time") == 0) {
        currentTime();
    } else if (strcmp(arg1, "date") == 0) {
        currentDate();
    } else if (strcmp(arg1, "memuse") == 0) {
        showMEMUsage();
    } else if (strcmp(arg1, "shutdown") == 0) {
        shutdownSystem();
    } else if (strcmp(arg1, "restart") == 0) {
        restartSystem();
    } else if (strcmp(arg1, "error") == 0) {
        char *errprompt = strtok(NULL, "");
        errmsg(errprompt);
    } else if (strcmp(arg1, "warning") == 0) {
        char *warnprompt = strtok(NULL, "");
        warnmsg(warnprompt);
        if (iniVerboseMode) {
            printf("Warning message shown\n");
        }
    } else if (strcmp(arg1, "info") == 0) {
        char *infoprompt = strtok(NULL, "");
        infomsg(infoprompt);
    } else if (strcmp(arg1, "beep") == 0) {
        printf("\7Started beep");
    } else if (strcmp(arg1, "title") == 0) {
        char *title = strtok(NULL, "");

        if (title) {
            SetConsoleTitleA(title);
            if (iniVerboseMode) {
                printf("Changed program title to \"%s\"\n", title);
            }
        } else if (title == NULL) {
            SetConsoleTitleA("Program");
            if (iniVerboseMode) {
                printf("No title specified. Changing title to \"Program\"\n");
            }
        }
    } else if (strcmp(arg1, "prompt") == 0) {
        char *argprompt = strtok(NULL, " ");

        if (argprompt == NULL || strlen(argprompt) == 0) {
            strcpy(prompt, ">> ");
            isdirprompt = false;
            if (iniVerboseMode) {
                printf("Prompt is now \">>\"\n");
            }
        } else if (strcmp(argprompt, "null") == 0) {
            strcpy(prompt, "");  // No prompt
            isdirprompt = false;
            if (iniVerboseMode) {
                printf("Prompt is now \"\"\n");
            }
        } else if (strcmp(argprompt, "dollar") == 0) {
            strcpy(prompt, "$ ");  // Dollar sign prompt
            isdirprompt = false;
            if (iniVerboseMode) {
                printf("Prompt is now \"$\"\n");
            }
        } else if (strcmp(argprompt, "dir") == 0) {
            char cwd[256];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                strcpy(prompt, cwd);
                strcat(prompt, "> ");  // Directory prompt
                isdirprompt = true;
                if (iniVerboseMode) {
                    printf("Prompt is now \"[Directory]>\"\n");
                }
            } else {
                printf("Error: Could not retrieve current directory.\n");
                isdirprompt = false;
                strcpy(prompt, ">> ");
            }
        } else if (strcmp(argprompt, "default") == 0) {
            isdirprompt = false;
            strcpy(prompt, ">> ");  // Default prompt
            if (iniVerboseMode) {
                printf("Prompt is now \">>\"\n");
            }
        } else {
            // Invalid prompt, show error
            printf("Error: Invalid prompt choice. Reverting to default prompt.\n");
            isdirprompt = false;
            strcpy(prompt, ">> ");  // Revert to default prompt
        }
    } else if (strcmp(arg1, "fprompt") == 0) {
        char *fprompt = strtok(NULL, "");
        fgets(fprompt, sizeof(fprompt), stdin);
        fprompt[strcspn(fprompt, "\n")] = 0;

        if (fprompt == NULL || strlen(fprompt) == 0) {
            printf("Error: No prompt specified for 'fprompt'\n");
            return;
        }
        strcpy(prompt, fprompt);
        isdirprompt = false;
    } else if (strcmp(arg1, "btrm") == 0) {
        SetConsoleTitleA("FishOS BTerminal");
        printf("BTerminal 1.6.9b\n");
        printf("Type \"help\" for all commands.\n");

        strcpy(prompt, ">> ");
        isdirprompt = false;
    } else if (strcmp(arg1, "mkfile") == 0) {
        char *mkprompt = strtok(NULL, "");
        if (mkprompt == NULL || strlen(mkprompt) == 0) {
            printf("Error: No filename specified\n");
            return;
        }
        createFile(mkprompt);  // Call the createFile function
    } else if (strcmp(arg1, "rmfile") == 0 || strcmp(arg1, "del") == 0) {
        char *rmprompt = strtok(NULL, "");
        if (rmprompt == NULL || strlen(rmprompt) == 0) {
            printf("Error: No filename specified.\n");
            return;
        }
        deleteFile(rmprompt);  // Call the deleteFile function
    } else if (strcmp(arg1, "char") == 0) {
        char *chsprompt = strtok(NULL, " ");
        if (chsprompt == NULL) {
            printf("Error: Missing argument.\n");
            return;
        }
        
        int charCode = atoi(chsprompt); // Convert the argument to an integer
        if (charCode == 0 && chsprompt[0] != '0') {
            printf("Error: Argument must be a valid number between 0 and 255.\n");
            return;
        }
        
        charSearch(charCode);
        return;
    } else if (strcmp(arg1, "exmemuse") == 0) {
        showProgramMEM();
    } else if (strcmp(arg1, "meminfo") == 0) {
        meminfo();
    } else if (strcmp(arg1, "throwerror") == 0) {
        throwError();
    } else if (strcmp(arg1, "run") == 0) {
        char *program = strtok(NULL, "");
        if (program == NULL) {
            printf("Error: No program specified for 'run'.\n");
            return;
        }
        runProgram(program);
    } else if (strcmp(arg1, "msg") == 0) {
        char *ma1 = strtok(NULL, " ");
        char *ma2 = strtok(NULL, "");

        nmsg(ma1, ma2);
        if (iniVerboseMode) {
            printf("Message shown\n");
        }
    } else if (strcmp(arg1, "qstmsg") == 0) {
        char *qma1 = strtok(NULL, " ");
        char *qma2 = strtok(NULL, "");
        
        qstmsg(qma1, qma2);
        if (iniVerboseMode) {
            printf("Question message shown\n");
        }
    } else if (strcmp(arg1, "pause") == 0) {
            printf("Press any key to continue...\n");
            pauseTerminal();
    } else if (strcmp(arg1, "nulpause") == 0) {
        pauseTerminal();
    } else if (strcmp(arg1, "version") == 0 || strcmp(arg1, "ver") == 0) {
        printf("Running BTerminal Beta 1.6.9_02\n");
        printf("Last edited in 06/23/2025\n");
    } else if (strcmp(arg1, "copy") == 0) {
        char *source = strtok(NULL, " ");
        char *destination = strtok(NULL, "");
        if (source == NULL || destination == NULL) {
            printf("Error: Missing source or destination for copy command.\n");
        } else {
            copyFile(source, destination);
        }
    } else if (strcmp(arg1, "setup") == 0) {
        setupBTRM();
    } else if (strcmp(arg1, "learn") == 0) {
        char *learnarg = strtok(NULL, " ");

        if (learnarg == NULL) {
            printf("Usage: learn [topic]\n");
            return;
        }
        if (strcmp(learnarg, "col") == 0) {
            printf("A terminal has two types of colors: foreground color and background color.\n");
            printf("The foreground color is the color of the text, while the background color is the color behind the text.\n");
            printf("Inside these 2 types, there are 16 color combinations, which are:\n");
            printf("Black = 0 | Red = 1 | Green = 2 | Yellow = 3 | Blue = 4 | Pink = 5 | Cyan = 6 | White = 7\n");

            printf("There are other variations of these 8 colors which are:\n");
            printf("Bright Black = 8 | Bright Red = 9 | Bright Green = 10 | Bright Yellow = 11 | Bright Blue = 12 | Bright Pink = 13 | Bright Cyan = 14 | Bright White = 15\n\n");

            printf("Warning: These colors may change depending on how your terminal handles them.\n");
            
            printf("Example:\n");

            // Change text color to black and background to black too
            SetConsoleTextAttribute(hConsole, 0);
            printf("0 ");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
            printf("1 ");
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
            printf("2 ");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
            printf("3 ");
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
            printf("4 ");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE);
            printf("5 ");
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE);
            printf("6 ");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            printf("7\n");

            SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
            printf("8 ");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
            printf("9 ");
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            printf("10 ");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            printf("11 ");
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            printf("12 ");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            printf("13 ");
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            printf("14 ");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            printf("15\n");

            printf("Warning: Final setting doesn't work on all text modes. You may need to change colors manually to fit with the default ones.\n");
        } else if (strcmp(learnarg, "wincol") == 0) {
            printf("Referencing \"Terminal colors\"; type \"learn col\" for more information.\n\n");

            printf("Windows terminals handle differently the colors, but we already talked about default colors.\n");
            printf("Windows terminals have commands to change the colors, but they are not the same as the default ones.\n");
            printf("    1. Numbers from 0 to 7 are default colors.\n");
            printf("    2. Numbers from A to F are bright colors.\n");
            printf("    3. Its syntax is [fcol][bcol], where fcol is the foreground color and bcol is the background color.\n\n");

            printf("Other way that Windows terminals handle colors is by these groups:\n");
            printf("    1. Red, Green, Blue. (RGB)\n");
            printf("        1.1. If nothing provided in any group of colors (foreground or background), it will be null depending on the group.\n");
            printf("    2. Two groups: FOREGROUND_INTENSITY and BACKGROUND_INTENSITY. (These two groups can change the intensity of the color or the brightness of the color depending on how your terminal handles it.\n\n");

            printf("Example:\n");

            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
            printf("    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);\n");
            printf("    SetConsoleTextAttribute (hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);\n");

            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

            printf("\"hConsole\" is a variable handle type. GetStdHandle call returns a handle of the current output to it.\n");
            printf("SetConsoleTextAttribute changes the output attributes of the current output handler.\n");
        } else if (strcmp(learnarg, "ansicol") == 0) {
            printf("Referencing \"Terminal colors\"; type \"learn col\" for more information.\n\n");

            printf("ANSI colors are a way to change the colors of the terminal using escape sequences.\n");
            printf("These sequences are used to change the text color, background color, and other text attributes.\n");
            printf("Its syntax is \\033[<arguments>m (only for C and C++, in C# its syntax is \"\\x1b\"), where <arguments> are the parameters of the output attribute change.\n\n");

            printf("This document won't cover all the ANSI escape code attributes, but we will show basic ones.\n");
            printf("    1. \\033[0m: Reset all attributes to default.\n");
            printf("    2. \\033[1m: Bold or increased intensity.\n");
            printf("    3. \\033[2m: Faint or decreased intensity.\n");
            printf("    4. \\033[3m: Italicized.\n");
            printf("    5. \\033[4m: Underlined.\n");
            printf("    6. \\033[5m: Blink.\n");
            printf("    7. \\033[7m: Inverted colors.\n");
            printf("    8. \\033[8m: Hidden text.\n");
            printf("    9. \\033[30m to \\033[37m: Set foreground color to black, red, green, yellow, blue, pink, cyan, or white. (0 to 7)\n");
            printf("    10. \\033[40m to \\033[47m: Set background color to black, red, green, yellow, blue, pink, cyan, or white. (0 to 7)\n");
            printf("    11. \\033[90m to \\033[97m: Set bright foreground color to black, red, green, yellow, blue, pink, cyan, or white. (0 to 7)\n");
            printf("    12. \\033[100m to \\033[107m: Set bright background color to black, red, green, yellow, blue, pink, cyan, or white. (0 to 7)\n\n");

            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
            printf("NOTE: ANSI attributes don't work on all console modes. You may need to change colors manually to fit with the default ones.\n\n");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            printf("NOTE 2: This color setting doesn't work on all text modes. You may need to change colors manually to fit with the default ones.\n");
        } else if (strcmp(learnarg, "btrm") == 0) {
            printf("BTerminal is a simple terminal program that runs on Windows systems.\n");
            printf("It provides a set of commands and features to interact with the system and the user.\n");
            printf("Although it contains Linux code, it may not run well in Linux systems.\n\n");

            printf("Here is some information about BTerminal:\n\n");

            printf("  +-------------------------------------------------------+\n");
            printf("  | * Type \"help\" to see all commands.                    |\n");
            printf("  | * Type \"learn\" to learn about a topic.                |\n");
            printf("  | * Type \"exit\" to close the terminal.                  |\n");
            printf("  | * Type \"setup\" to configure the terminal.             |\n");
            printf("  |                                                       |\n");
            printf("  | This console can cause damage, so use this securely.  |\n");
            printf("  +-------------------------------------------------------+\n\n");
        } else if (strcmp(learnarg, "operators") == 0) {
            printf("Operators are symbols that perform operations on variables and values.\n");
            printf("There are different types of operators in C, but we will cover only the mathemathical ones.\n\n");

            printf("Here are the basic math operators:\n");
            printf("    1. +: Addition - adds a value to another.\n");
            printf("    2. -: Subtraction - subtracts a value from another.\n");
            printf("    3. *: Multiplication - multiplies a value by another.\n");
            printf("    4. /: Division - divides a value by another.\n");
            printf("    5. %%: Modulus - returns the remainder of a division.\n");
            printf("    6. ++: Increment - increases the value of a variable by 1. This operator can be used as a prefix or suffix.\n");
            printf("    7. --: Decrement - decreases the value of a variable by 1. This operator can be used as a prefix or suffix.\n\n");
            printf("    8. =: Assignment - assigns a value to a variable.\n");
            printf("    9. +=: Addition assignment - adds a value to a variable and assigns the result to the variable.\n");
            printf("    10. -=: Subtraction assignment - subtracts a value from a variable and assigns the result to the variable.\n");
            printf("    11. *=: Multiplication assignment - multiplies a variable by a value and assigns the result to the variable.\n");
            printf("    12. /=: Division assignment - divides a variable by a value and assigns the result to the variable.\n");
            printf("    13. %%=: Modulus assignment - calculates the remainder of a division and assigns the result to the variable.\n\n");
            printf("    14. (), {}, []: Containers - used to specify the order of operations in an expression.\n");

            printf("Example:\n");
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
            printf("    int main() {\n");
            printf("        int x = 8;\n");
            printf("        int y = 4;\n");
            printf("        int sum = x + y;\n\n");
            printf("        return sum;\n");
            printf("    }\n");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

            printf("The main function returns 12, which is the sum of 8 and 4.\n");
        } else if (strcmp(learnarg, "conoperators") == 0) {
            printf("Conditional operators are used to perform different actions based on different conditions.\n");
            printf("These operators are used to make decisions in code.\n\n");

            printf("Here are the basic conditional operators:\n");
            printf("    1. ==: Equal to - checks if two values are equal.\n");
            printf("    2. !=: Not equal to - checks if two values are not equal.\n");
            printf("    3. >: Greater than - checks if the left value is greater than the right value.\n");
            printf("    4. <: Less than - checks if the left value is less than the right value.\n");
            printf("    5. >=: Greater than or equal to - checks if the left value is greater than or equal to the right value.\n");
            printf("    6. <=: Less than or equal to - checks if the left value is less than or equal to the right value.\n\n");
            printf("    7. ?: Conditional - returns a value based on a condition being true or false.\n");

            printf("Here are the logical operators:\n");
            printf("    1. &&: Logical AND - returns true if both conditions are true.\n");
            printf("    2. ||: Logical OR - returns true if one of the conditions is true.\n");
            printf("    3. !: Logical NOT - returns true if the condition is false.\n\n");

            printf("Example:\n");
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
            printf("    int main() {\n");
            printf("        int x = 8;\n");
            printf("        int y = 4;\n");
            printf("        if (x > y) {\n");
            printf("            return x;\n");
            printf("        } else {\n");
            printf("            return y;\n");
            printf("        }\n");
            printf("    }\n");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

            printf("The main function returns 8, which is the value of x because x is greater than y.\n");
        } else if (strcmp(learnarg, "binoperators") == 0) {
            printf("Bitwise operators are used to perform operations on individual bits of a number.\n");
            printf("These operators are used to manipulate the bits of variables.\n\n");

            printf("Here are the basic bitwise operators:\n");
            printf("    1. &: Bitwise AND - sets each bit to 1 if both bits are 1.\n");
            printf("    2. |: Bitwise OR - sets each bit to 1 if one of the bits in a group is 1.\n");
            printf("    3. ^: Bitwise XOR - sets each bit to 1 if only one of the bits is 1.\n");
            printf("    4. ~: Bitwise NOT - inverts all the bits.\n");
            printf("    5. <<: Bitwise left shift - shifts the bits to the left by a specified number of positions.\n");
            printf("    6. >>: Bitwise right shift - shifts the bits to the right by a specified number of positions.\n\n");

            printf("Example:\n");
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
            printf("0101 & 0011 = 0001\n");
            printf("0101 | 0011 = 0111\n");
            printf("0101 ^ 0011 = 0110\n");
            printf("~0101 = 1010\n");
            printf("0101 << 1 = 1010; 1 is the number of bits to shift\n");
            printf("0101 >> 1 = 1010; 1 is the number of bits to shift\n");

            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        } else if (strcmp(learnarg, "output") == 0) {
            printf("How does outputting work internally?\n");
            printf("Outputting is the process of displaying data on the screen.\n");

            printf("Here is how outputting is parsed in Assembly:\n");
            printf("    1. The data is stored in a string.\n");
            printf("    2. The data is moved to the output buffer register.\n");
            printf("    3. The data is displayed on the screen.\n");

            printf("Example:\n");
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
            printf("    org 0x7C00\n");
            printf("    start:\n");
            printf("        mov si, msg_hello\n");
            printf("        push si\n");
            printf("        push ax\n");
            printf("        push bx\n\n");

            printf("    halt:\n");
            printf("        cli\n");
            printf("        hlt\n\n");

            printf("    .loop:\n");
            printf("        lodsb\n");
            printf("        or al, al\n");
            printf("        jz .done\n");
            printf("        mov ah, 0x0E\n");
            printf("        mov bh, 0\n");
            printf("        int 0x10\n");
            printf("        jmp .loop\n\n");

            printf("    .done:\n");
            printf("        pop bx\n");
            printf("        pop ax\n");
            printf("        pop si\n");
            printf("        ret\n\n");

            printf("    msg_hello db 'Hello, World!', 0\n");
            printf("    times 510-($-$$) db 0\n");
            printf("    dw 0AA55h\n");

            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        }
    } else if (strcmp(arg1, "whoami") == 0) {
        printUserName();
    } else if (strncmp(arg1, "#", 1) == 0) {
        return; // Ignore comments
    } else if (strcmp(arg1, "gdi") == 0) {
        char *gdiarg = strtok(NULL, "");

        drawTextCenter(gdiarg);

        if (iniVerboseMode) {
            printf("GDI text created\n");
        }
    } else if (strcmp(arg1, "rand") == 0) {
        char *randarg = strtok(NULL, "");
        if (randarg == NULL) {
            printf("Error: Missing argument for random number generator.\n");
            return;
        }
        int mr = atoi(randarg); // Convert the argument to an integer
        srand(time(NULL));
        int r = rand() % mr; // Random number between 0 and 32767
        printf("Random number: %d\n", r);
    } else if (strcmp(arg1, "clip") == 0) {
        char *cliparg = strtok(NULL, " ");
        if (cliparg == NULL) {
            printf("Error: Missing argument for clipboard command.\n");
            return;
        }
        if (set_clipboard_text(cliparg)) {
            printf("Text copied to clipboard. Type \"print (^V)\" to paste it.\n");
        } else {
            printf("Failed to copy text to clipboard. Please try again later.\n");
        }
    } else if (strcmp(arg1, "rgbcol") == 0) {
        char *rgbr = strtok(NULL, " ");
        char *rgbg = strtok(NULL, " ");
        char *rgbb = strtok(NULL, " ");

        if (rgbr == NULL || rgbg == NULL || rgbb == NULL) {
            printf("Error: Missing RGB color values.\n");
            return;
        }
        int r = atoi(rgbr);
        int g = atoi(rgbg);
        int b = atoi(rgbb);

        if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
            printf("Error: RGB values must be between 0 and 255.\n");
            return;
        }
        // Set the console text color using RGB values
        printf("\033[38;2;%d;%d;%dm", r, g, b);
    }
    
    else {
        printf("Error: Command or symbol not recognized. Please see help for a detailed list of commands.\n");
    }
}

void handleOverflow(char *command) {
    if (strlen(command) > MAX_COMMAND_LENGTH) {
        // If command length exceeds the limit, clear the input

        printf("Error: Command too long. (%d)\7\n", sizeof(command));
        strcpy(command, ""); // Reset the command buffer
    }
}
void trimWhitespace(char *str) {
    // Trim leading and trailing whitespace
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == 0) {
        return; // All spaces
    }

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }

    // Write new null terminator
    *(end + 1) = 0;
}

// -- Terminal --

int main() {
    time_t rawtime;
    struct tm *timeinfo;
    char buffert[10];
    char meridiem[3]; // To hold "AM" or "PM"

    // Get the current time
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Check if it's AM or PM
    if (timeinfo->tm_hour >= 12) {
        strcpy(meridiem, "PM");
        if (timeinfo->tm_hour > 12) {
            timeinfo->tm_hour -= 12; // Convert to 12-hour format
        }
    }
    else {
        strcpy(meridiem, "AM");
        if (timeinfo->tm_hour == 0) {
            timeinfo->tm_hour = 12; // Midnight edge case for 12-hour format
        }
    }

    // Format the time as HH:MM:SS
    strftime(buffert, sizeof(buffert), "%I:%M:%S", timeinfo);


    time_t rawtimed;
    struct tm *timeinfod;
    char bufferd[30]; // Buffer to hold the formatted date string

    // Get the current date and time
    time(&rawtimed);
    timeinfod = localtime(&rawtimed);

    // Format the date as "DD-MM-YYYY, Weekday DD"
    strftime(bufferd, sizeof(bufferd), "%d-%m-%Y, %A %d", timeinfod);

    FILE *inifile = fopen("btrm.ini", "rb");
    if (inifile == NULL) {
        iniVerboseMode = false;
        iniFullscreen = false;
        fclose(inifile);

        FILE *mlogfile = fopen("btrm.log", "a");
        fprintf(mlogfile, "[CMDLINE/INFO] %s %s\n", bufferd, buffert);
        fprintf(mlogfile, "Couldn't find file btrm.ini. Continuing with default settings.\n");
    } else {
        iniVerboseMode = true;
    }

    char command[16384];

    enableANSI();
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    SetConsoleTitleA("BTerminal");
    SetConsoleOutputCP(437);

    printf("BTerminal Beta 1.6.9\n");
    printf("Type \"help\" for all commands.\n");

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    splashText();
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    printf("\n");

    while (1) {
        printf("%s", prompt);
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        trimWhitespace(command);
        handleOverflow(command);
        handleCommand(command);

        if (strlen(command) == 0) {
            continue;  // Ignore and keep looping
        }
    }

    return 0;
}