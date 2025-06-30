#define _CRT_SECURE_NO_WARNINGS
#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

Game* games = NULL;
size_t gameCount = 0;

static void loadGamesFromFile(void);
static void saveGamesToFile(void);
static void addGame(void);
static void displayGames(void);
static void suggestByGenre(void);
static void deleteGame(void);
static void updateGame(void);
static void sortGames(void);
static void clearInputBuffer(void);
static int compareGames(const void* a, const void* b);
static void binarySearchGenre(const char* genre, int left, int right);
static int strcasecmp_custom(const char* s1, const char* s2);

void runMenu(void) {
    loadGamesFromFile();
    int choice;
    do {
        printf("\n--- Game Manager ---\n");
        printf("1. Dodaj igru\n");
        printf("2. Prikazi sve igre\n");
        printf("3. Predlozi po zanru\n");
        printf("4. Obrisi igru\n");
        printf("5. Azuriraj zanr igre\n");
        printf("6. Sortiraj igre\n");
        printf("0. Izlaz\nUnesi izbor: ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("Neispravan unos.\n");
            continue;
        }
        clearInputBuffer();

        switch (choice) {
        case MENU_ADD: addGame(); break;
        case MENU_DISPLAY: displayGames(); break;
        case MENU_SUGGEST: suggestByGenre(); break;
        case MENU_DELETE: deleteGame(); break;
        case MENU_UPDATE: updateGame(); break;
        case MENU_SORT: sortGames(); break;
        case MENU_EXIT: break;
        default: printf("Pogresan izbor.\n");
        }
    } while (choice != MENU_EXIT);

    free(games);
    games = NULL;
}

static void loadGamesFromFile(void) {
    FILE* file = fopen(FILE_NAME, "r");
    if (!file) {
        perror("Greska pri otvaranju");
        return;
    }

    Game temp;
    while (fscanf(file, " %99[^|]|%49[^\n]\n", temp.name, temp.genre) == 2) {
        Game* tempPtr = realloc(games, (gameCount + 1) * sizeof(Game));
        if (!tempPtr) {
            perror("Greska realloc");
            fclose(file);
            return;
        }
        games = tempPtr;
        games[gameCount++] = temp;
    }
    fclose(file);
}

static void saveGamesToFile(void) {
    FILE* file = fopen(FILE_NAME, "w");
    if (!file) {
        perror("Ne mogu pisati u datoteku");
        return;
    }
    for (size_t i = 0; i < gameCount; ++i)
        fprintf(file, "%s|%s\n", games[i].name, games[i].genre);
    fclose(file);
}

static void addGame(void) {
    Game newGame;
    printf("Ime igre: ");
    fgets(newGame.name, MAX_NAME_LENGTH, stdin);
    newGame.name[strcspn(newGame.name, "\n")] = 0;

    printf("Zanr: ");
    fgets(newGame.genre, MAX_GENRE_LENGTH, stdin);
    newGame.genre[strcspn(newGame.genre, "\n")] = 0;

    for (size_t i = 0; i < gameCount; ++i) {
        if (strcasecmp_custom(games[i].name, newGame.name) == 0) {
            printf("Igra vec postoji.\n");
            return;
        }
    }

    Game* temp = realloc(games, (gameCount + 1) * sizeof(Game));
    if (!temp) {
        perror("realloc failed");
        return;
    }
    games = temp;
    games[gameCount++] = newGame;
    saveGamesToFile();
}

static void displayGames(void) {
    for (size_t i = 0; i < gameCount; ++i)
        printf("%zu. %s [%s]\n", i + 1, games[i].name, games[i].genre);
}

static void suggestByGenre(void) {
    char genre[MAX_GENRE_LENGTH];
    printf("Unesi zanr: ");
    fgets(genre, MAX_GENRE_LENGTH, stdin);
    genre[strcspn(genre, "\n")] = 0;
    binarySearchGenre(genre, 0, gameCount - 1);
}

static void deleteGame(void) {
    char name[MAX_NAME_LENGTH];
    printf("Unesi ime igre za brisanje: ");
    fgets(name, MAX_NAME_LENGTH, stdin);
    name[strcspn(name, "\n")] = 0;

    for (size_t i = 0; i < gameCount; ++i) {
        if (strcasecmp_custom(games[i].name, name) == 0) {
            for (size_t j = i; j < gameCount - 1; ++j)
                games[j] = games[j + 1];
            --gameCount;
            saveGamesToFile();
            printf("Igra obrisana.\n");
            return;
        }
    }
    printf("Nema te igre.\n");
}

static void updateGame(void) {
    char name[MAX_NAME_LENGTH];
    printf("Unesi ime igre za izmjenu: ");
    fgets(name, MAX_NAME_LENGTH, stdin);
    name[strcspn(name, "\n")] = 0;

    for (size_t i = 0; i < gameCount; ++i) {
        if (strcasecmp_custom(games[i].name, name) == 0) {
            printf("Unesi novi zanr: ");
            fgets(games[i].genre, MAX_GENRE_LENGTH, stdin);
            games[i].genre[strcspn(games[i].genre, "\n")] = 0;
            saveGamesToFile();
            printf("Zanr azuriran.\n");
            return;
        }
    }
    printf("Igra nije nadena.\n");
}

static void sortGames(void) {
    qsort(games, gameCount, sizeof(Game), compareGames);
    saveGamesToFile();
    printf("Sortirano.\n");
}

static int compareGames(const void* a, const void* b) {
    return strcasecmp_custom(((Game*)a)->name, ((Game*)b)->name);
}

static void binarySearchGenre(const char* genre, int left, int right) {
    if (left > right) return;
    int mid = (left + right) / 2;
    if (strcasecmp_custom(games[mid].genre, genre) == 0)
        printf("Preporuka: %s\n", games[mid].name);
    binarySearchGenre(genre, left, mid - 1);
    binarySearchGenre(genre, mid + 1, right);
}

static void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static int strcasecmp_custom(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2))
            return (tolower((unsigned char)*s1) - tolower((unsigned char)*s2));
        s1++; s2++;
    }
    return (tolower((unsigned char)*s1) - tolower((unsigned char)*s2));
}
