#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

#define FILE_NAME "games.txt"
#define TEMP_FILE_NAME "games_temp.txt"

#define MAX_NAME_LENGTH 100
#define MAX_GENRE_LENGTH 50

#define IS_EMPTY(count) ((count) == 0)

typedef union {
    int intValue;
    size_t sizeValue;
} NumberValue;

typedef struct {
    char name[MAX_NAME_LENGTH];
    char genre[MAX_GENRE_LENGTH];
} Game;

extern Game* games;
extern size_t gameCount;

enum MenuOption {
    MENU_EXIT,
    MENU_ADD,
    MENU_DISPLAY,
    MENU_SUGGEST,
    MENU_DELETE,
    MENU_UPDATE,
    MENU_INSERT,
    MENU_SORT,
    MENU_SEARCH
};

static inline void removeNewLine(char* text)
{
    if (text != NULL) {
        while (*text) {
            if (*text == '\n') {
                *text = '\0';
                break;
            }
            text++;
        }
    }
}

void runMenu(void);

#endif
