#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>

typedef struct GameState {
    char ***grid; 
    int row;      
    int column;
    int isInitialized;
    bool isFirstWordInitiated; 
    bool validWordsLoaded;
    char ***prevGrid; 
    int prevRow;      
    int prevColumn;
} GameState;

void initiatedCheck();
int isWordValid(const char* word);
void freeValidWords();
void loadValidWords(const char* filename, GameState *game);
void increaseHorizontally(GameState *game, int new_cols);
void increaseVertically(GameState *game, int new_rows);
void displayBoard(GameState *game);
bool checkBoardWords(GameState *game);
GameState* initialize_game_state(const char *filename);
GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed);
GameState* undo_place_tiles(GameState *game);
void free_game_state(GameState *game);
void save_game_state(GameState *game, const char *filename);
void freeGameMemory(GameState *game);
char ***copyGrid(char ***grid, int rows, int columns);



