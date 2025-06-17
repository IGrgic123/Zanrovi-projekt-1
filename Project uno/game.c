#define _CRT_SECURE_NO_WARNINGS

#include "game.h" // uključuje vlastitu zaglavlje datoteku
#include <stdio.h>   // za rad sa ulaz/izlazom 
#include <stdlib.h>  // za izlaz u slučaju pogreške 
#include <string.h>  // za rad sa stringovima 
#include <errno.h>   // za ispis grešaka
#include <ctype.h>   // za funkcije poput tolower

// Deklaracije statičkih funkcija za enkapsulaciju funkcionalnosti (skrivanje implementacije) static bool fileExists(const char* filename); // provjera postoji li datoteka static void createEmptyFile(const char* filename); // kreira praznu datoteku ako ne postoji static bool gameExists(const char* name); // provjera postoji li igra već u datoteci static void addGame(const Game* game); // dodavanje nove igre u datoteku static void displayAllGames(void); // ispis svih igara static void suggestByGenre(const char* genre); // prijedlog igara po žanru static void clearInputBuffer(void); // čišćenje input buffera static int strcasecmp_custom(const char* s1, const char* s2); // vlastita verzija case-insensitive usporedbe

// Glavna funkcija menija koja poziva ostale ovisno o unosu korisnika void runMenu(void) { if (!fileExists(FILE_NAME)) { // ako datoteka ne postoji, kreiraj je createEmptyFile(FILE_NAME); }

int choice;
do {
    // Jasna i strukturirana izbornik interakcija s korisnikom
    printf("\n--- Game Manager ---\n");
    printf("1. Dodaj novu igricu\n");
    printf("2. Ispisi sve igrice\n");
    printf("3. Predlozi igrice po zanru\n");
    printf("0. Exit\n");
    printf("Upisi svoj odabir: ");

    if (scanf("%d", &choice) != 1) { // zaštita unosa
        clearInputBuffer();
        printf("Netocan upis, probaj ponovno.\n");
        continue;
    }
    clearInputBuffer();

    switch (choice) {
    case MENU_ADD_GAME: {
        Game newGame;

        // Unos imena igre
        printf("Upisi ime igrice: ");
        if (!fgets(newGame.name, sizeof(newGame.name), stdin)) {
            printf("Error citanja imena igrice.\n");
            break;
        }
        newGame.name[strcspn(newGame.name, "\n")] = 0; // uklanjanje newline znaka

        if (strlen(newGame.name) == 0) { // provjera praznog unosa
            printf("Polje za ime igrice ne moze biti prazno.\n");
            break;
        }

        if (gameExists(newGame.name)) { // provjera duplikata
            printf("Igrica vec postoji.\n");
            break;
        }

        // Unos žanra igre
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

        addGame(&newGame); // dodavanje igre u datoteku
        printf("Igrica dodana uspjesno.\n");
        break;
    }
    case MENU_DISPLAY_ALL:
        displayAllGames(); // ispis svih igara
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

        suggestByGenre(genre); // predlaganje igara po žanru
        break;
    }
    case MENU_EXIT:
        printf("Zavrsavam program.\n"); // izlaz iz programa
        break;
    default:
        printf("Netocan odabir, pokusaj ponovno.\n");
        break;
    }
} while (choice != MENU_EXIT); // glavni kontrolni loop

}

static bool fileExists(const char* filename) { FILE* file = fopen(filename, "r"); if (file) { fclose(file); return true; } return false; }

static void createEmptyFile(const char* filename) { FILE* file = fopen(filename, "w"); if (!file) { perror("Neuspjesno kreiranje datoteke"); exit(EXIT_FAILURE); // program se gasi na grešku (obrada greške) } fclose(file); }

static bool gameExists(const char* name) { FILE* file = fopen(FILE_NAME, "r"); if (!file) { perror("Error otvaranja datoteke "); return false; }

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

static void addGame(const Game* game) { FILE* file = fopen(FILE_NAME, "a"); if (!file) { perror("Error otvaranja datoteke za dodavanje igre"); return; } fprintf(file, "%s|%s\n", game->name, game->genre); fclose(file); }

static void displayAllGames(void) { FILE* file = fopen(FILE_NAME, "r"); if (!file) { perror("Error otvaranja datoteke za ispis igre"); return; }

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

static int strcasecmp_custom(const char* s1, const char* s2) { while (*s1 && *s2) { if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2)) return (tolower((unsigned char)*s1) - tolower((unsigned char)*s2)); s1++; s2++; } return (tolower((unsigned char)*s1) - tolower((unsigned char)*s2)); }

// Ovdje se koristi rekurzija - funkcija rekurzivno traži žanr u datoteci static void suggestByGenre(const char* genre) { static long position = 0; FILE* file = fopen(FILE_NAME, "r"); if (!file) { perror("Error otvaranja datoteke za ispisivanje zanra"); return; } fseek(file, position, SEEK_SET);

char line[256];
bool found = false;
while (fgets(line, sizeof(line), file)) {
    position = ftell(file);
    char* sep = strchr(line, '|');
    if (!sep) continue;
    *sep = '\0';
    char* gameName = line;
    char* gameGenre = sep + 1;
    gameGenre[strcspn(gameGenre, "\n")] = 0;

    if (strcasecmp_custom(gameGenre, genre) == 0) {
        printf("Name: %s\n", gameName);
        found = true;
        fclose(file);
        suggestByGenre(genre); // REKURZIJA
        return;
    }
}
if (!found) {
    printf("Nema igrica sa ovim zanrom, dodajte ih!.\n");
}
position = 0;
fclose(file);

}

static void clearInputBuffer(void) { int c; while ((c = getchar()) != '\n' && c != EOF); // čisti buffer da se izbjegne višestruki unos }

