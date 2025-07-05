#include <stdio.h>
#include <string.h>

#ifndef COLORS_H

// text colors
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[93m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define GRAY "\033[36m"
#define WHITE "\033[97m"

// background colors
#define BGBLACK "\033[40m"
#define BGRED "\033[41m"
#define BGGREEN "\033[42m"
#define BGYELLOW "\033[43m"
#define BGBLUE "\033[44m"
#define BGMAGENTA "\033[45m"
#define BGGRAY "\033[46m"
#define BGWHITE "\033[97m"

// styles
#define BOLD "\033[1m"
#define ITALIC "\033[3m"
#define UNDERLINED "\033[4m"
#define CONCEAL "\033[8m"
#define CROSSOUT "\033[9m"
#define FRAKTUR "\033[20m"
#define RESET "\033[0m"
#define FRAMED "\033[51m"
#define FAINT "\033[2m"

void changeColor(const char *color, const char *bg_color) {
    // Foreground colors
    if (strcmp(color, "black") == 0)
        printf("\033[30m");
    else if (strcmp(color, "red") == 0)
        printf("\033[31m");
    else if (strcmp(color, "green") == 0)
        printf("\033[32m");
    else if (strcmp(color, "yellow") == 0)
        printf("\033[33m");
    else if (strcmp(color, "blue") == 0)
        printf("\033[34m");
    else if (strcmp(color, "pink") == 0)
        printf("\033[35m");
    else if (strcmp(color, "cyan") == 0)
        printf("\033[36m");
    else if (strcmp(color, "white") == 0)
        printf("\033[37m");
    else if (strcmp(color, "bblack") == 0)
        printf("\033[90m");
    else if (strcmp(color, "bred") == 0)
        printf("\033[91m");
    else if (strcmp(color, "bgreen") == 0)
        printf("\033[92m");
    else if (strcmp(color, "byellow") == 0)
        printf("\033[93m");
    else if (strcmp(color, "bblue") == 0)
        printf("\033[94m");
    else if (strcmp(color, "bpink") == 0)
        printf("\033[95m");
    else if (strcmp(color, "bcyan") == 0)
        printf("\033[96m");
    else if (strcmp(color, "bwhite") == 0)
        printf("\033[97m");
    else
        printf("Unknown text color: %s\n", color);
    
    // Background colors
    if (bg_color != NULL) {
        if (strcmp(bg_color, "kblack") == 0)
            printf("\033[40m");
        else if (strcmp(bg_color, "kred") == 0)
            printf("\033[41m");
        else if (strcmp(bg_color, "kgreen") == 0)
            printf("\033[42m");
        else if (strcmp(bg_color, "kyellow") == 0)
            printf("\033[43m");
        else if (strcmp(bg_color, "kblue") == 0)
            printf("\033[44m");
        else if (strcmp(bg_color, "kpink") == 0)
            printf("\033[45m");
        else if (strcmp(bg_color, "kcyan") == 0)
            printf("\033[46m");
        else if (strcmp(bg_color, "kwhite") == 0)
            printf("\033[47m");
        else if (strcmp(bg_color, "kbblack") == 0)
            printf("\033[100m");
        else if (strcmp(bg_color, "kbred") == 0)
            printf("\033[101m");
        else if (strcmp(bg_color, "kbgreen") == 0)
            printf("\033[102m");
        else if (strcmp(bg_color, "kbyellow") == 0)
            printf("\033[103m");
        else if (strcmp(bg_color, "kbblue") == 0)
            printf("\033[104m");
        else if (strcmp(bg_color, "kbpink") == 0)
            printf("\033[105m");
        else if (strcmp(bg_color, "kbcyan") == 0)
            printf("\033[106m");
        else if (strcmp(bg_color, "kbwhite") == 0)
            printf("\033[107m");
        else
            return;
    }
}

#endif