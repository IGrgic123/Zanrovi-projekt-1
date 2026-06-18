#define _CRT_SECURE_NO_WARNINGS
#include "game.h"
#include <ctype.h>
#include <errno.h>

Game* games = NULL;
size_t gameCount = 0;

static const char tempFileName[] = TEMP_FILE_NAME;

static void loadGamesFromFile(void);
static bool saveGamesToFile(void);

static void addGame(void);
static void displayGames(void);
static void suggestByGenre(void);
static void deleteGame(void);
static void updateGame(void);
static void insertGame(void);
static void sortGames(void);
static void searchGame(void);

static void clearInputBuffer(void);
static void freeGames(void);

static bool resizeGames(size_t newCount);
static bool readLine(char* text, size_t size);
static bool readMenuChoice(int* choice);
static bool readSize(size_t* value);

static int compareGamesByName(const void* a, const void* b);
static int compareGameKeyByName(const void* key, const void* element);
static int strcasecmpCustom(const char* s1, const char* s2);

static Game* findGameByName(const char* name);
static Game* findGameLinear(const char* name);

static void searchGenreRecursive(
    const char* genre,
    int left,
    int right,
    bool* found
);

static long getFileSize(FILE* file);

void runMenu(void)
{
    static unsigned int menuOpenCount = 0;
    int choice = -1;

    menuOpenCount++;

    loadGamesFromFile();

    do {
        printf("\n--- Game Manager ---\n");
        printf("1. Dodaj igru\n");
        printf("2. Prikazi sve igre\n");
        printf("3. Predlozi po zanru\n");
        printf("4. Obrisi igru\n");
        printf("5. Azuriraj zanr igre\n");
        printf("6. Insert igre na poziciju\n");
        printf("7. Sortiraj igre\n");
        printf("8. Pretrazi igru\n");
        printf("0. Izlaz\n");
        printf("Unesi izbor: ");

        if (!readMenuChoice(&choice)) {
            printf("Neispravan unos.\n");
            continue;
        }

        switch ((MenuOption)choice) {

        case MENU_ADD:
            addGame();
            break;

        case MENU_DISPLAY:
            displayGames();
            break;

        case MENU_SUGGEST:
            suggestByGenre();
            break;

        case MENU_DELETE:
            deleteGame();
            break;

        case MENU_UPDATE:
            updateGame();
            break;

        case MENU_INSERT:
            insertGame();
            break;

        case MENU_SORT:
            sortGames();
            break;

        case MENU_SEARCH:
            searchGame();
            break;

        case MENU_EXIT:
            break;

        default:
            printf("Pogresan izbor.\n");
            break;
        }

    } 
while (choice != MENU_EXIT);

freeGames();
}

static void loadGamesFromFile(void){
    FILE* file = fopen(FILE_NAME, "r");

    if (file == NULL) {
        printf(
            "Datoteka %s nije pronadena: %s\n",
            FILE_NAME,
            strerror(errno)
        );
        return;
    }

    if (getFileSize(file) <= 0) {
        fclose(file);
        return;
    }
rewind(file);
 Game tempGame;
    while (
        fscanf(
            file,
            " %99[^|]|%49[^\n]",
            tempGame.name,
            tempGame.genre
        ) == 2
    ) {
        if (!resizeGames(gameCount + 1)) {
            fclose(file);
            return;
        }

        games[gameCount] = tempGame;
        gameCount++;
    }

    if (ferror(file)) {
        perror("Greska pri citanju datoteke");
    }

    if (feof(file)) {
        clearerr(file);
    }

    fclose(file);
}

static bool saveGamesToFile(void)
{
    FILE* file = fopen(tempFileName, "w");

    if (file == NULL) {
        perror("Ne mogu otvoriti privremenu datoteku");
        return false;
    }

    for (size_t i = 0; i < gameCount; i++) {
        fprintf(
            file,
            "%s|%s\n",
            games[i].name,
            games[i].genre
        );
    }

    if (ferror(file)) {
        perror("Greska pri pisanju u datoteku");
        fclose(file);
        return false;
    }

    fclose(file);

    if (remove(FILE_NAME) != 0) {
        perror("Greska pri brisanju stare datoteke");
        remove(tempFileName);
        return false;
    }

    if (rename(tempFileName, FILE_NAME) != 0) {
        perror("Greska pri preimenovanju datoteke");
        return false;
    }

    return true;
}

static void addGame(void)
{
    Game newGame;

    printf("Ime igre: ");

    if (!readLine(
        newGame.name,
        MAX_NAME_LENGTH
    )) {
        printf("Neispravan unos imena.\n");
        return;
    }

    if (findGameLinear(newGame.name) != NULL) {
        printf("Igra vec postoji.\n");
        return;
    }

    printf("Zanr: ");

    if (!readLine(
        newGame.genre,
        MAX_GENRE_LENGTH
    )) {
        printf("Neispravan unos zanra.\n");
        return;
    }

    if (!resizeGames(gameCount + 1)) {
        return;
    }

    games[gameCount] = newGame;
    gameCount++;

    if (saveGamesToFile()) {
        printf("Igra dodana.\n");
    }
}

static void displayGames(void)
{
    if (IS_EMPTY(gameCount)) {
        printf("Nema spremljenih igara.\n");
        return;
    }

    for (size_t i = 0; i < gameCount; i++) {
        printf(
            "%zu. %s [%s]\n",
            i + 1,
            games[i].name,
            games[i].genre
        );
    }
}

static void suggestByGenre(void)
{
    char genre[MAX_GENRE_LENGTH];
    bool found = false;

    if (IS_EMPTY(gameCount)) {
        printf("Nema spremljenih igara.\n");
        return;
    }

    printf("Unesi zanr: ");

    if (!readLine(
        genre,
        MAX_GENRE_LENGTH
    )) {
        printf("Neispravan unos zanra.\n");
        return;
    }

    printf(
        "Preporuke za zanr %s:\n",
        genre
    );

    searchGenreRecursive(
        genre,
        0,
        (int)gameCount - 1,
        &found
    );

    if (!found) {
        printf("Nema igre za taj zanr.\n");
    }
}
```c
static void deleteGame(void)
{
    char name[MAX_NAME_LENGTH];

    if (IS_EMPTY(gameCount)) {
        printf("Nema igara za brisanje.\n");
        return;
    }

    printf("Unesi ime igre za brisanje: ");

    if (!readLine(
        name,
        MAX_NAME_LENGTH
    )) {
        printf("Neispravan unos imena.\n");
        return;
    }

    Game* foundGame = findGameLinear(name);

    if (foundGame == NULL) {
        printf("Nema te igre.\n");
        return;
    }

    size_t index = (size_t)(foundGame - games);

    for (size_t i = index; i + 1 < gameCount; i++) {
        games[i] = games[i + 1];
    }

    gameCount--;

    if (gameCount == 0) {
        freeGames();
    }
    else if (!resizeGames(gameCount)) {
        return;
    }

    if (saveGamesToFile()) {
        printf("Igra obrisana.\n");
    }
}

static void updateGame(void)
{
    char name[MAX_NAME_LENGTH];

    if (IS_EMPTY(gameCount)) {
        printf("Nema igara za azuriranje.\n");
        return;
    }

    printf("Unesi ime igre za izmjenu: ");

    if (!readLine(
        name,
        MAX_NAME_LENGTH
    )) {
        printf("Neispravan unos imena.\n");
        return;
    }

    Game* foundGame = findGameLinear(name);

    if (foundGame == NULL) {
        printf("Igra nije nadena.\n");
        return;
    }

    printf("Unesi novi zanr: ");

    if (!readLine(
        foundGame->genre,
        MAX_GENRE_LENGTH
    )) {
        printf("Neispravan unos zanra.\n");
        return;
    }

    if (saveGamesToFile()) {
        printf("Zanr azuriran.\n");
    }
}

static void insertGame(void)
{
    Game newGame;
    size_t position;

    printf(
        "Unesi poziciju za insert od 1 do %zu: ",
        gameCount + 1
    );

    if (
        !readSize(&position) ||
        position < 1 ||
        position > gameCount + 1
    ) {
        printf("Neispravna pozicija.\n");
        return;
    }

    printf("Ime igre: ");

    if (!readLine(
        newGame.name,
        MAX_NAME_LENGTH
    )) {
        printf("Neispravan unos imena.\n");
        return;
    }

    if (findGameLinear(newGame.name) != NULL) {
        printf("Igra vec postoji.\n");
        return;
    }

    printf("Zanr: ");

    if (!readLine(
        newGame.genre,
        MAX_GENRE_LENGTH
    )) {
        printf("Neispravan unos zanra.\n");
        return;
    }

    Game* copiedGame = malloc(sizeof(Game));

    if (copiedGame == NULL) {
        perror("Greska malloc");
        return;
    }

    *copiedGame = newGame;

    if (!resizeGames(gameCount + 1)) {
        free(copiedGame);
        copiedGame = NULL;
        return;
    }

    for (
        size_t i = gameCount;
        i > position - 1;
        i--
    ) {
        games[i] = games[i - 1];
    }

    games[position - 1] = *copiedGame;
    gameCount++;

    free(copiedGame);
    copiedGame = NULL;

    if (saveGamesToFile()) {
        printf("Igra umetnuta.\n");
    }
}

static void sortGames(void)
{
    if (IS_EMPTY(gameCount)) {
        printf("Nema igara za sortiranje.\n");
        return;
    }

    qsort(
        games,
        gameCount,
        sizeof(Game),
        compareGamesByName
    );

    if (saveGamesToFile()) {
        printf("Sortirano.\n");
    }
}

static void searchGame(void)
{
    char name[MAX_NAME_LENGTH];

    if (IS_EMPTY(gameCount)) {
        printf("Nema spremljenih igara.\n");
        return;
    }

    printf("Unesi ime igre: ");

    if (!readLine(
        name,
        MAX_NAME_LENGTH
    )) {
        printf("Neispravan unos.\n");
        return;
    }

    Game* foundGame = findGameByName(name);

    if (foundGame == NULL) {
        printf("Igra nije pronadena.\n");
        return;
    }

    printf("Pronadena igra:\n");
    printf(
        "%s [%s]\n",
        foundGame->name,
        foundGame->genre
    );
}

static Game* findGameLinear(
    const char* name
)
{
    if (
        name == NULL ||
        games == NULL
    ) {
        return NULL;
    }

    for (
        size_t i = 0;
        i < gameCount;
        i++
    ) {
        if (
            strcasecmpCustom(
                games[i].name,
                name
            ) == 0
        ) {
            return &games[i];
        }
    }

    return NULL;
}
static Game* findGameByName(
    const char* name
)
{
    if (
        name == NULL ||
        games == NULL ||
        IS_EMPTY(gameCount)
    ) {
        return NULL;
    }

    qsort(
        games,
        gameCount,
        sizeof(Game),
        compareGamesByName
    );

    return bsearch(
        name,
        games,
        gameCount,
        sizeof(Game),
        compareGameKeyByName
    );
}

static void searchGenreRecursive(
    const char* genre,
    int left,
    int right,
    bool* found
)
{
    if (
        genre == NULL ||
        found == NULL ||
        games == NULL ||
        left > right
    ) {
        return;
    }

    int middle =
        left + (right - left) / 2;

    searchGenreRecursive(
        genre,
        left,
        middle - 1,
        found
    );

    if (
        strcasecmpCustom(
            games[middle].genre,
            genre
        ) == 0
    ) {
        printf(
            "- %s\n",
            games[middle].name
        );

        *found = true;
    }

    searchGenreRecursive(
        genre,
        middle + 1,
        right,
        found
    );
}

static bool resizeGames(
    size_t newCount
)
{
    if (newCount == 0) {
        freeGames();
        return true;
    }

    Game* tempGames =
        realloc(
            games,
            newCount * sizeof(Game)
        );

    if (tempGames == NULL) {
        perror("Greska realloc");
        return false;
    }

    games = tempGames;
    return true;
}

static void freeGames(void)
{
    if (games != NULL) {
        free(games);
        games = NULL;
    }

    gameCount = 0;
}

static bool readLine(
    char* text,
    size_t size
)
{
    if (
        text == NULL ||
        size == 0
    ) {
        return false;
    }

    if (
        fgets(
            text,
            (int)size,
            stdin
        ) == NULL
    ) {
        return false;
    }

    removeNewLine(text);

    return strlen(text) > 0;
}

static bool readMenuChoice(
    int* choice
)
{
    if (choice == NULL) {
        return false;
    }

    NumberValue number;

    if (
        scanf(
            "%d",
            &number.intValue
        ) != 1
    ) {
        clearInputBuffer();
        return false;
    }

    clearInputBuffer();

    *choice = number.intValue;

    return true;
}

static bool readSize(
    size_t* value
)
{
    if (value == NULL) {
        return false;
    }

    NumberValue number;

    if (
        scanf(
            "%zu",
            &number.sizeValue
        ) != 1
    ) {
        clearInputBuffer();
        return false;
    }

    clearInputBuffer();

    *value = number.sizeValue;

    return true;
}

static long getFileSize(
    FILE* file
)
{
    if (file == NULL) {
        return -1;
    }

    if (
        fseek(
            file,
            0,
            SEEK_END
        ) != 0
    ) {
        perror("Greska fseek");
        return -1;
    }

    long size = ftell(file);

    if (size == -1L) {
        perror("Greska ftell");
        return -1;
    }

    rewind(file);

    return size;
}

static void clearInputBuffer(void)
{
    int character;

    while (
        (character = getchar()) != '\n' &&
        character != EOF
    ) {
    }
}

static int compareGamesByName(
    const void* a,
    const void* b
)
{
    if (
        a == NULL ||
        b == NULL
    ) {
        return 0;
    }

    const Game* firstGame =
        (const Game*)a;

    const Game* secondGame =
        (const Game*)b;

    return strcasecmpCustom(
        firstGame->name,
        secondGame->name
    );
}

static int compareGameKeyByName(
    const void* key,
    const void* element
)
{
    if (
        key == NULL ||
        element == NULL
    ) {
        return 0;
    }

    const char* name =
        (const char*)key;

    const Game* game =
        (const Game*)element;

    return strcasecmpCustom(
        name,
        game->name
    );
}

static int strcasecmpCustom(
    const char* s1,
    const char* s2
)
{
    if (
        s1 == NULL ||
        s2 == NULL
    ) {
        return 0;
    }

    while (
        *s1 != '\0' &&
        *s2 != '\0'
    ) {
        int firstCharacter =
            tolower(
                (unsigned char)*s1
            );

        int secondCharacter =
            tolower(
                (unsigned char)*s2
            );

        if (
            firstCharacter !=
            secondCharacter
        ) {
            return
                firstCharacter -
                secondCharacter;
        }

        s1++;
        s2++;
    }

    return
        tolower(
            (unsigned char)*s1
        ) -
        tolower(
            (unsigned char)*s2
        );
}
