#include <stdio.h>
#include <string.h>

void changeColor(const char *color, const char *bg_color) {
    // Foreground colors
    if (strcmp(color, "black") == 0)
        printf("\033[0m");  // Black text
    else if (strcmp(color, "red") == 0)
        printf("\033[31m");  // Red text
    else if (strcmp(color, "green") == 0)
        printf("\033[32m");  // Green text
    else if (strcmp(color, "yellow") == 0)
        printf("\033[93m");  // Yellow text
    else if (strcmp(color, "blue") == 0)
        printf("\033[34m");  // Blue text
    else if (strcmp(color, "magenta") == 0)
        printf("\033[35m");  // Magenta text
    else if (strcmp(color, "gray") == 0)
        printf("\033[36m");
    else if (strcmp(color, "white") == 0)
        printf("\033[97m");  // White text
    else
        printf("Color de texto desconocido: %s\n", color);
    
    // Background colors
    if (bg_color != NULL) {
        if (strcmp(bg_color, "black") == 0)
            printf("\033[0m");  // Black background
        else if (strcmp(bg_color, "red") == 0)
            printf("\033[41m");  // Red background
        else if (strcmp(bg_color, "green") == 0)
            printf("\033[42m");  // Green background
        else if (strcmp(bg_color, "yellow") == 0)
            printf("\033[43m");  // Yellow background
        else if (strcmp(bg_color, "blue") == 0)
            printf("\033[44m");  // Blue background
        else if (strcmp(bg_color, "magenta") == 0)
            printf("\033[45m");  // Magenta background
        else if (strcmp(bg_color, "gray") == 0)
            printf("\033[46m");  // Cyan background
        else if (strcmp(bg_color, "white") == 0)
            printf("\033[97m");  // White background
        else
            return;
    }
}