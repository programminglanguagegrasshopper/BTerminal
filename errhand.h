#include <windows.h>
#include <Psapi.h>

#ifndef ERRHAND_H

#define ERRHAND_H

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

// Helper function to register the window class
void RegisterMyWindowClass(HINSTANCE hInstance, HICON icon, const wchar_t* className) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hIcon = icon; // Set custom icon

    RegisterClassW(&wc);
}

// Helper function to create the window
HWND CreateMyWindow(const wchar_t* title, const wchar_t* className, DWORD style,
                    int xpos, int ypos, int xsize, int ysize, HWND pwin, HINSTANCE hInstance) {
    HWND hwnd = CreateWindowExW(
        0,                              // Optional window styles (dwExStyle)
        className,                      // Window class
        title,                          // Window title
        style,                          // Window style
        xpos, ypos,                     // Position (xpos, ypos)
        xsize, ysize,                   // Size (xsize, ysize)
        pwin,                           // Parent window handle
        NULL,                           // Menu handle (optional)
        hInstance,                      // Instance handle
        NULL                            // Additional application data
    );

    return hwnd; // Return the handle of the created window
}

// Function to get the icon based on the specified type
HICON GetIcon(int iconType) {
    switch (iconType) {
        case 0: return NULL;                              // No icon
        case 16: return LoadIcon(NULL, IDI_ERROR);       // Error icon
        case 32: return LoadIcon(NULL, IDI_WARNING);     // Warning icon
        case 48: return LoadIcon(NULL, IDI_INFORMATION); // Information icon
        default: return NULL;                             // Default case, no icon
    }
}

// Function to display a message box with the appropriate title and message
void ShowMessageBox(HWND hwnd, const wchar_t* title, const wchar_t* msg, int iconType) {
    // Don't show the window at all
    ShowWindow(hwnd, SW_HIDE);

    UINT iconStyle;
    switch (iconType) {
        case 0: iconStyle = MB_OK; break;                              // No icon
        case 16: iconStyle = MB_OK | MB_ICONERROR; break;            // Error icon
        case 32: iconStyle = MB_OK | MB_ICONWARNING; break;          // Warning icon
        case 48: iconStyle = MB_OK | MB_ICONINFORMATION; break;      // Information icon
        default: iconStyle = MB_OK;                                   // Default case, no icon
    }

    // Show the message box
    MessageBoxW(NULL, msg, title, iconStyle);

    // After closing the message box, prevent window from showing again
}

void errmsg(const char* errorMessage) {
    // Create a message box that shows the error message
    MessageBox(
        NULL,            // Handle to the owner window (NULL for no owner)
        errorMessage,    // The error message to display
        "Error",        // Title of the message box
        MB_ICONERROR | MB_OK // Error icon and an OK button
    );
}

void warnmsg(const char* warnMessage) {
    // Create a message box that shows the error message
    MessageBox(
        NULL,            // Handle to the owner window (NULL for no owner)
        warnMessage,    // The error message to display
        "Warning",         // Title of the message box
        MB_ICONWARNING | MB_OK // Warning icon and an OK button
    );
}

void infomsg(const char* infoMessage) {
    // Create a message box that shows the error message
    MessageBox(
        NULL,            // Handle to the owner window (NULL for no owner)
        infoMessage,    // The error message to display
        "Information",         // Title of the message box
        MB_ICONINFORMATION | MB_OK // Info icon and an OK button
    );
}

void nmsg(const char* Message, const char* title) {
    // Create a message box that shows the error message
    MessageBox(
        NULL,            // Handle to the owner window (NULL for no owner)
        Message,    // The error message to display
        title,         // Title of the message box
        MB_OK // No icon and an OK button
    );
}

void qstmsg(const char* Message, const char* title) {
    // Create a message box that shows the error message
    MessageBox(
        NULL,            // Handle to the owner window (NULL for no owner)
        Message,    // The error message to display
        title,         // Title of the message box
        MB_ICONQUESTION | MB_OK // No icon and an OK button
    );
}

void eexit(){
    SetConsoleTitleA("Clearing buffer...");
    fflush(stdout);

    SetConsoleTitleA("Obtaining exit code...");
    int exitCode = GetLastError();
    printf("Exiting program with code %d\n", exitCode);

    SetConsoleTitleA("Exiting process...");
    SetConsoleTitleA("");
    exit(0);
}

void displayError(DWORD errorCode) {
    unsigned char *errorBytes = (unsigned char *)&errorCode;

    // Print each byte of the error code as a character from code page 437
    for (int i = 0; i < 65535; i++) {
        printf("%c", errorBytes[i]);
    }

    putchar('\n');
    Beep(1024, 10000);
}

void throwError() {
    // Get the last error code
    DWORD lastError = GetLastError();
    
    if (lastError == 0) {
        // If there is no last error, output an error message
        printf("Error: No error to initiate.\n");
        return;
    }
    fflush(stdout);

    // Change text color and background
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE);

    // Output the program handle
    HANDLE hProgramHandle = GetCurrentProcess();
    
    printf("HANDLE hProgramHandle = %p\n", hProgramHandle);

    // Output the error code
    printf("DWORD lastError = %lu\n\n", lastError);

    char errorMessage[] = "ERR_HEX_LASTERROR";

    printf("An error has occurred and BTerminal needs to shut down.\n\n");

    printf("STOP - 0x%X, 0x%X\n", lastError, hProgramHandle);
    printf("If this is the first time you see this error, please be aware that something \n");
    printf("critical has failed. This may be due to hardware issues or disk failures.\n\n");

    printf("If you need to see the error shown, please locate the string below the stdout dump \n");
    printf("section ('stdout -> hex'). If the error says UNKNOWN ERROR or NULL BUFFER then \n");
    printf("it's safe to exit. If you see other error like DISK IS FULL, SYSTEM OVERFLOW, etc. then \n");
    printf("the device has hardware issues.\n\n");

    printf("Initializing dump of memory... ");
    printf("0");

    long int GLEC = (lastError * sizeof(stdout)) / 2;
    Sleep(500);
    printf("\b33");
    Sleep(500);
    printf("\b\b66");
    SetConsoleTitleA("ERROR!");
    Beep(32767, 1000);
    printf("\b\b99");
    int ERRLEVEL = sizeof(lastError);
    printf("\b\b100\n\n");

    printf("\n");
    printf("GLE %x HPH %x ", GetLastError(), hProgramHandle);
    printf("EM %d EM %x GLE %lu ", errorMessage, errorMessage, GetLastError());
    printf("EM %lld GLE %lld HPH %llu GLEClean %ld", sizeof(errorMessage), sizeof(GetLastError()), sizeof(hProgramHandle), GLEC);
    printf("stdout -> %x\n", stdout);

    switch (lastError) {
        case 0x0008:
            printf("MEMORY OVERFLOW");
        case 0x0015:
            printf("THE DEVICE IS NOT READY");
        case 0x05AA:
            printf("SYSTEM OVERFLOW");
        case 0x05B4:
            printf("OVERLOAD");
        case 0x01F4:
            printf("UNKNOWN ACCESS");
        case 0x0051:
            Beep(1024, 4000);
            exit(81);
        case 0x0048:
            printf("END OF FILE ERROR");
            exit(72);
        case 0x004A:
            printf("DISK IS FULL");
        case 0x004F:
            printf("HARDWARE FAILURE");
        case 0x045B:
            printf("BAD DISK");
        case 0x001A:
            printf("IMCOMPLETE DISK");
        case 0x0041:
            printf("DEVICE NOT FOUND");
        case 0x00BB:
            printf("NULL BUFFER");
        default:
            printf("UNKNOWN ERROR");
    }
    FILE *logFile = fopen("btrm.log", "a");
    if (logFile == NULL) {
        return;
    } else {
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

        fprintf(logFile, "[CMDLINE/ERROR] %s %s\n", bufferd, buffert);
        fprintf(logFile, "Cause: Unknown (cause was defined by handler or user)\n");
        fprintf(logFile, "Error code: %lu (%d bytes)\n", lastError, ERRLEVEL);
        fprintf(logFile, "Error message: %s\n", errorMessage);
        fprintf(logFile, "Program handle: %p\n", hProgramHandle);
        fclose(logFile);
    }

    displayError(lastError);

    printf("\n");

    // Exit the program after error output
    exit(1);
}

#endif // ERRHAND_H;