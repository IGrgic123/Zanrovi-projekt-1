#define _CRT_SECURE_NO_WARNINGS

#include "game.h"
#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>     
#include <errno.h>      
#include <ctype.h>      


static bool fileExists(const char* filename);
static void createEmptyFile(const char* filename);
static bool gameExists(const char* name);
static void addGame(const Game* game);
static void displayAllGames(void);
static void suggestByGenre(const char* genre);
static void clearInputBuffer(void);
static int strcasecmp_custom(const char* s1, const char* s2);
static bool deleteGame(const char* name);


void runMenu(void) {
    if (!fileExists(FILE_NAME)) {
        createEmptyFile(FILE_NAME);
    }

    int choice;
    do {
        printf("\n--- Game Manager ---\n");
        printf("1. Dodaj novu igricu\n");
        printf("2. Ispisi sve igrice\n");
        printf("3. Predlozi igrice po zanru\n");
        printf("4. Obrisi igru\n");
        printf("0. Exit\n");
        printf("Upisi svoj odabir: ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer(); // čišćenje buffera ako unos nije broj
            printf("Netocan upis, probaj ponovno.\n");
            continue;
        }
        clearInputBuffer();

        switch (choice) {
        case MENU_ADD_GAME: {
            Game newGame;

            printf("Upisi ime igrice: ");
            if (!fgets(newGame.name, sizeof(newGame.name), stdin)) {
                printf("Error citanja imena igrice.\n");
                break;
            }
            newGame.name[strcspn(newGame.name, "\n")] = 0;

            if (strlen(newGame.name) == 0) {
                printf("Polje za ime igrice ne moze biti prazno.\n");
                break;
            }

            if (gameExists(newGame.name)) {
                printf("Igrica vec postoji.\n");
                break;
            }

            printf("Unesi zanr igrice: ");
            if (!fgets(newGame.genre, sizeof(newGame.genre), stdin)) {
                printf("Error citanja zanra igrice.\n");
                break;
            }
            newGame.genre[strcspn(newGame.genre, "\n")] = 0;

            if (strlen(newGame.genre) == 0) {
                printf("Polje zanra ne moze biti prazno.\n");
                break;
            }

            addGame(&newGame);
            printf("Igrica dodana uspjesno.\n");
            break;
        }
        case MENU_DISPLAY_ALL:
            displayAllGames();
            break;
        case MENU_SUGGEST_BY_GENRE: {
            char genre[MAX_GENRE_LENGTH];
            printf("Upisite zanr: ");
            if (!fgets(genre, sizeof(genre), stdin)) {
                printf("Error citanja zanra.\n");
                break;
            }
            genre[strcspn(genre, "\n")] = 0;

            if (strlen(genre) == 0) {
                printf("Polje zanra ne moze biti prazno.\n");
                break;
            }

            suggestByGenre(genre);
            break;
        }
        case MENU_DELETE_GAME: {
            char nameToDelete[MAX_NAME_LENGTH];
            printf("Unesi ime igre za brisanje: ");
            if (!fgets(nameToDelete, sizeof(nameToDelete), stdin)) {
                printf("Greska pri citanju imena.\n");
                break;
            }
            nameToDelete[strcspn(nameToDelete, "\n")] = 0;

            if (strlen(nameToDelete) == 0) {
                printf("Ime ne moze biti prazno.\n");
                break;
            }

            if (deleteGame(nameToDelete)) {
                printf("Igra '%s' uspjesno obrisana.\n", nameToDelete);
            }
            else {
                printf("Igra '%s' nije pronadena.\n", nameToDelete);
            }
            break;
        }
        case MENU_EXIT:
            printf("Zavrsavam program.\n");
            break;
        default:
            printf("Netocan odabir, pokusaj ponovno.\n");
            break;
        }
    } while (choice != MENU_EXIT);
}

// Provjera postoji li datoteka
static bool fileExists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

// Kreira praznu datoteku
static void createEmptyFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Neuspjesno kreiranje datoteke");
        exit(EXIT_FAILURE);
    }
    fclose(file);
}

// Provjerava postoji li već igrica s tim imenom
static bool gameExists(const char* name) {
    FILE* file = fopen(FILE_NAME, "r");
    if (!file) {
        perror("Error otvaranja datoteke ");
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char* sep = strchr(line, '|');
        if (!sep) continue;
        *sep = '\0';
        if (strcmp(line, name) == 0) {
            fclose(file);
            return true;
        }
    }
    fclose(file);
    return false;
}

// Dodaje novu igru
static void addGame(const Game* game) {
    FILE* file = fopen(FILE_NAME, "a");
    if (!file) {
        perror("Error otvaranja datoteke za dodavanje igre");
        return;
    }
    fprintf(file, "%s|%s\n", game->name, game->genre);
    fclose(file);
}

// Ispisuje sve igrice
static void displayAllGames(void) {
    FILE* file = fopen(FILE_NAME, "r");
    if (!file) {
        perror("Error otvaranja datoteke za ispis igre");
        return;
    }

    char line[256];
    printf("\n--- Sve igre ---\n");
    while (fgets(line, sizeof(line), file)) {
        char* sep = strchr(line, '|');
        if (!sep) continue;
        *sep = '\0';
        char* gameName = line;
        char* genre = sep + 1;
        genre[strcspn(genre, "\n")] = 0;

        printf("Ime: %s, Zanr: %s\n", gameName, genre);
    }
    fclose(file);
}

static int strcasecmp_custom(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2))
            return (tolower((unsigned char)*s1) - tolower((unsigned char)*s2));
        s1++;
        s2++;
    }
    return (tolower((unsigned char)*s1) - tolower((unsigned char)*s2));
}

static void suggestByGenre(const char* genre) {
    FILE* file = fopen(FILE_NAME, "r");
    if (!file) {
        perror("Error otvaranja datoteke za ispisivanje zanra");
        return;
    }

    char line[256];
    bool found = false;
    printf("\n--- Igrice s tim zanrom '%s' ---\n", genre);
    while (fgets(line, sizeof(line), file)) {
        char* sep = strchr(line, '|');
        if (!sep) continue;
        *sep = '\0';
        char* gameName = line;
        char* gameGenre = sep + 1;
        gameGenre[strcspn(gameGenre, "\n")] = 0;

        if (strcasecmp_custom(gameGenre, genre) == 0) {
            printf("Name: %s\n", gameName);
            found = true;
        }
    }
    if (!found) {
        printf("Nema igrica sa ovim zanrom, dodajte ih!.\n");
    }
    fclose(file);
}

static void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Brise igru iz datoteke po imenu
static bool deleteGame(const char* name) {
    FILE* file = fopen(FILE_NAME, "r");
    if (!file) {
        perror("Greska pri otvaranju datoteke");
        return false;
    }

    FILE* tempFile = fopen("temp.txt", "w");
    if (!tempFile) {
        perror("Greska pri kreiranju privremene datoteke");
        fclose(file);
        return false;
    }

    char line[256];
    bool found = false;
    while (fgets(line, sizeof(line), file)) {
        char* sep = strchr(line, '|');
        if (!sep) {
            fputs(line, tempFile);
            continue;
        }
        *sep = '\0';
        char* gameName = line;

        if (strcmp(gameName, name) == 0) {
            found = true; // preskoci ovu liniju (brisemo)
        }
        else {
            *sep = '|';
            fputs(line, tempFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        if (remove(FILE_NAME) != 0) {
            perror("Greska pri brisanju originalne datoteke");
            return false;
        }
        if (rename("temp.txt", FILE_NAME) != 0) {
            perror("Greska pri preimenovanju privremene datoteke");
            return false;
        }
    }
    else {
        remove("temp.txt"); // brisemo temp ako igra nije nadjena
    }

    return found;
}
