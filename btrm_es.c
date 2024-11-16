#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <direct.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <Psapi.h>
#include "colors_es.h"
#include "errhand_es.h"
#include "resource.h"

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif // Si tu ordenador soporta 'ENABLE_VIRTUAL_TERMINAL_PROCESSING', eliminálo.
// No recomiendo eliminar nada de esto porque puedes romper la función para habilitar ANSI.

#define EXIT_SUCCESS 0
#define MAX_COMMAND_LENGTH 65536
#define PROMPT ">> "


// --Terminal Settings--

void enableANSI() { // Enables ANSI if running on unsupported ANSI program
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

// --Commands--

void shutdownSystem() {
    char input[10];

    // Step 1: Warn the user
    printf("ADVERTENCIA: Esta accion apagara el sistema!\n");
    printf("Quieres proceder a apagar el sistema? (Y/N): ");
    fgets(input, 10, stdin);

    // Remove newline character from input
    input[strcspn(input, "\n")] = 0;

    // Step 2: Check for confirmation
    if (strcmp(input, "Y") == 0) {
        changeColor("red", NULL);
        printf("Apagando el sistema...\n");
        printf("Espera de 15 segundos (si necesitas guardar algo)\n");
        system("shutdown /s /f /t 15");
    } else {
        printf("Apagado cancelado.\n");
    }
}

void showMEMUsage() {
    PROCESS_MEMORY_COUNTERS memInfo;

    // Get memory usage information
    if (GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo))) {
        printf("Memoria usada en el momento: %lu bytes\n", (unsigned long)memInfo.WorkingSetSize);
    } else {
        printf("No se pudo obtener el uso de memoria actual.\n");
        eexit();
        errmsg("No se pudo obtener memInfo.WorkingSetSize data.\nCodigo de error: -1.");
    }
}

void changeDirectory(const char *path) {
    if (_chdir(path) != 0) {
        perror("Error al cambiar directorio\n");
    } else {
        printf("Directorio cambiado a: %s\n", path);
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
    printf("Fecha actual: %s\n", buffer);
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
    } else {
        strcpy(meridiem, "AM");
        if (timeinfo->tm_hour == 0) {
            timeinfo->tm_hour = 12; // Midnight edge case for 12-hour format
        }
    }

    // Format the time as HH:MM:SS
    strftime(buffer, sizeof(buffer), "%I:%M:%S", timeinfo);

    // Print the formatted time with AM/PM
    printf("Hora actual: %s %s\n", buffer, meridiem);
}

void clearScreen() {
    printf("\033[2J\033[H");
}

void readFile(const char *filename) {
    FILE *file = fopen(filename, "rb"); // Open in binary mode to support any file type
    if (file == NULL) {
        perror("Error al abrir archivo");
        return;
    }

    printf("Leyendo archivo: %s\n", filename);

    char buffer[1024];
    size_t bytesRead;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }

    printf("\n");
    fclose(file);
}

void writeFile(const char *filename, const char *content) {
    FILE *pF = fopen(filename, "a"); // Open in append mode
    if (pF == NULL) {
        perror("Error al abrir archivo\n");
        return;
    }

    fprintf(pF, "%s\n", content); // Append the content with a new line
    fflush(pF); // Ensure all data is written to the file
    fclose(pF);
    printf("Contenido escrito a %s\n", filename);
}

void flushinb() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {

    }
}

// --Handlers--

void handleCommand(char *command) {
    char *arg1 = strtok(command, " "); // Get the command
    if (arg1 == NULL) {
        printf("Error: No se escribio ningun comando.\n");
        return;
    }

    if (strcmp(arg1, "help") == 0) {
        printf("Comandos disponibles: help, read, write, gotodir, col, print, error, warning, info, memuse, exit.\n");
    } else if (strcmp(arg1, "read") == 0) {
        char *filename = strtok(NULL, "");  // Capture the entire remaining string as the filename
        if (filename == NULL) {
            printf("Error: No se escribio ningun nombre de archivo.\n");
        } else {
            readFile(filename);  // Pass the filename to the function
    }
    } else if (strcmp(arg1, "write") == 0) {
        char *filename = strtok(NULL, " ");
        char *content = strtok(NULL, "");
        if (filename == NULL || content == NULL) {
            printf("Error: Falta nombre de archivo o contenido.\n");
        } else {
            writeFile(filename, content);
        }
    } else if (strcmp(arg1, "gotodir") == 0) {
        char *path = strtok(NULL, "");
        if (path == NULL) {
            printf("Error: No se escribio ningun directorio.\n");
        } else {
            changeDirectory(path);
        }
    } else if (strcmp(arg1, "col") == 0) {
        char *color = strtok(NULL, " ");
        char *bgColor = strtok(NULL, " ");  // Background color is optional

        if (color) {
            if (bgColor) {
                changeColor(color, bgColor); // If both color and background are provided
            } else {
                changeColor(color, NULL); // Only change the text color if background is not provided
            }
        } else {
            printf("Error: No se escribio  el color del texto.\n");
        }
    } else if (strcmp(arg1, "exit") == 0) {
        exit(0); // Exit the program
    } else if (strcmp(command, "clear") == 0){
        clearScreen();
    } if (strncmp(command, "print ", 6) == 0) {
        char *text = strtok(NULL, "");
        if (text != NULL) {
            printf("%s\n", text); // Output the text after "print"
        }
    } else if (strcmp(command, "time") == 0){
        currentTime();
    } else if (strcmp(command, "date") == 0){
        currentDate();
    } else if (strcmp(command, "memuse") == 0){
        showMEMUsage();
    } else if (strcmp(command, "intoverflow") == 0){
        signed int overflowmax = pow(2, 16);
        printf("exceso de numero = %d\n", overflowmax);
    } else if (strcmp(command, "shutdown") == 0){
        shutdownSystem();
    } else if (strcmp(command, "error") == 0){
        errmsg("Error llamado por usuario\nCodigo de error: 0");
    } else if (strcmp(command, "warning") == 0){
        warnmsg("Advertencia llamada por usuario");
    } else if (strcmp(command, "info") == 0){
        infomsg("Informacion llamada por usuario");
    } else {
        printf(NULL);
    }
}

// --Terminal--

void handleOverflow(char *command) {
    if (strlen(command) > MAX_COMMAND_LENGTH) {
        // If command length exceeds the limit, clear the input

        printf("Error: Comando muy largo. (%d)\n", sizeof(command));
        strcpy(command, ""); // Reset the command buffer
    }
}

void prompt(){
    printf(PROMPT);
}

int main() {
    char command[16384];

    enableANSI();
    SetConsoleTitleA("FishOS BTerminal");

    printf("BTerminal 1.6.8b\n");
    printf("Escriba \"help\" para mostrar todos los comandos.\n");

    while (1) {
        prompt();
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        handleOverflow(command);
        handleCommand(command);
    }

    return 0;
}