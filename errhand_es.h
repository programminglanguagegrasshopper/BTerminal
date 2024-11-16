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
        "Error",         // Title of the message box
        MB_ICONERROR | MB_OK // Error icon and an OK button
    );
}

void warnmsg(const char* warnMessage) {
    // Create a message box that shows the error message
    MessageBox(
        NULL,            // Handle to the owner window (NULL for no owner)
        warnMessage,    // The error message to display
        "Advertencia",         // Title of the message box
        MB_ICONWARNING | MB_OK // Error icon and an OK button
    );
}

void infomsg(const char* infoMessage) {
    // Create a message box that shows the error message
    MessageBox(
        NULL,            // Handle to the owner window (NULL for no owner)
        infoMessage,    // The error message to display
        "Informacion",         // Title of the message box
        MB_ICONINFORMATION | MB_OK // Error icon and an OK button
    );
}

void eexit(){
    system("exit");
}