#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#define FILE_NAME "games.txt"
#define MAX_NAME_LENGTH 100
#define MAX_GENRE_LENGTH 50

typedef struct {
    char name[MAX_NAME_LENGTH];
    char genre[MAX_GENRE_LENGTH];
} Game;

enum {
    MENU_EXIT = 0,
    MENU_ADD_GAME = 1,
    MENU_DISPLAY_ALL = 2,
    MENU_SUGGEST_BY_GENRE = 3,
    MENU_DELETE_GAME = 4
};

void runMenu(void);

#endif

