#ifndef BTRM_SPLASHES_H

#define BTRM_SPLASHES_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

void splashText() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    srand(time(NULL));
    int randomNum = rand() % 33;

    switch (randomNum) {
        case 0:
            printf("Unicode doesn't work!\n");
            break;
        case 1:
            printf("Hmmmmmmmmmmmm\n");
            break;
        case 2:
            printf("Warning! Big fish team approaching dangerously!\n");
            break;
        case 3:
            printf("Honey, please click the code editor.\n");
            break;
        case 4:
            printf("Absolutely not DOSEm\n");
            break;
        case 5:
            printf("10212024\n");
            break;
        case 6:
            printf("Could not find splash text.\n");
            break;
        case 7:
            printf("Nyeh heh heh heh heh. Random number generator.\n");
            break;
        case 8:
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
            printf("C");
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            printf("o");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            printf("l");
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            printf("o");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            printf("r");
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            printf("f");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            printf("u");
            SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
            printf("l");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            printf("\n");
            break;
        case 9:
            printf("This program is not a virus.\n");
            break;
        case 10:
            printf("This program is not a trojan.\n");
            break;
        case 11:
            printf("Dog Apolo\n");
            break;
        case 12:
            printf("I don't know, maybe FishOS?\n");
            break;
        case 13:
            printf("Exactly! My dog is wonderful.\n");
            break;
        case 14:
            printf("Knock knock! You don't know how many roads I have crossed just to get here.\n");
            break;
        case 15:
            printf("This is getting ");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
            printf("SILLYSILLYSILLYSILLYSILLYSILLYSILLYSILLYSILLYSILLYSILLY\n");
            break;
        case 16:
            printf("This thing is not a joke.\n");
            break;
        case 17:
            printf("This is not a joke.\n");
            break;
        case 18:
            printf("Kids, don't gamble with the splash texts!\n");
            break;
        case 19:
            printf("Written in C- uhh what version?\n");
            break;
        case 20:
            printf("FishOS is now just a buried skeleton. -BTerminal creator, 2025\n");
            break;
        case 21:
            printf("Don't ask me about the splash text.\n");
            break;
        case 22:
            printf("I spent a long time on these splash texts! Now you are making these now!\n");
            break;
        case 23:
            printf("If UTF-8 doesn't work, why can you compile UTF-8 without DUNICODE?\n");
            break;
        case 24:
            printf("If you are reading this, you are a true BTerminal user.\n");
            break;
        case 25:
            printf("Here's your order. $0.2 for the splash texts, and $1 for the program.\n");
            break;
        case 26:
            printf("I may not have a brain, but I have an idea.\n");
            break;
        case 27:
            printf("128-bit sucks. Why would you want so many RAM?\n");
            break;
        case 28:
            printf("printf(\"Splash text\");\n");
            break;
        case 29:
            printf("Yes, but why is printf() an integer function?\n");
            break;
        case 30:
            printf("Clown-to-clown communication.\n");
            break;
        case 31:
            printf("How many lines of code do you want in this program? Yes.\n");
            break;
        case 32:
            printf("Steal a baby's candy. You will save him from diabetes.\n");
            break;
        default:
            printf("Error calculating splash text.\n");
            break;
    }
}

#endif // BTRM_SPLASHES_H