#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdio.h>

#define FILE_NAME "games.txt"
#define MAX_NAME_LENGTH 100
#define MAX_GENRE_LENGTH 50

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
    MENU_SORT
};

void runMenu(void);

#endif
