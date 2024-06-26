#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "hw3.h" 

//#define DEBUG(...) fprintf(stderr, "[] [ DEBUG ] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, " -- %s()\n", __func__)
#define MAX_STACK_HEIGHT 6 

char **valid = NULL;
int validTotal = 0;
int isGameInitialized = 0;

void pushCurrentState(GameState *game) {
    if (game->prevCount == game->prevCapacity) {
        int newCapacity = game->prevCapacity == 0 ? 1 : game->prevCapacity * 2;
        game->prevGrids = realloc(game->prevGrids, newCapacity * sizeof(char ***));
        game->prevRows = realloc(game->prevRows, newCapacity * sizeof(int));
        game->prevColumns = realloc(game->prevColumns, newCapacity * sizeof(int));
        game->prevCapacity = newCapacity;
    }

    game->prevGrids[game->prevCount] = copyGrid(game->grid, game->row, game->column);
    game->prevRows[game->prevCount] = game->row;
    game->prevColumns[game->prevCount] = game->column;
    game->prevCount++;
}

void loadValidWords(const char* filename, GameState *game) {
    //printf("Initiating all valid words from: %s\n", filename);
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open the words file");
        return;
    }
    char *currWord = NULL;
    size_t length = 0;
    ssize_t read;
    int capacity = 100;
    valid = malloc(capacity * sizeof(char*));
    if (!valid) {
        perror("Failed to allocate memory for valid words");
        fclose(file);
        return;
    }
    while ((read = getline(&currWord, &length, file)) != -1) {
        if (validTotal >= capacity) {
            capacity *= 2;
            char **temp = realloc(valid, capacity * sizeof(char*));
            if (!temp) {
                perror("Failed to resize valid words array");
                break;
            }
            valid = temp;
        }
        currWord[strcspn(currWord, "\n")] = 0; 
        for (size_t i = 0; currWord[i]; i++) currWord[i] = toupper((unsigned char)currWord[i]);
        valid[validTotal++] = strdup(currWord);
    }
    free(currWord);
    fclose(file);
    //printf("Loaded %d valid words.\n", validTotal);
    game->validWordsLoaded = true;
}

int isWordValid(const char* word) {
    for (int i = 0; i < validTotal; i++) {
        if (valid[i] && strcmp(word, valid[i]) == 0) {
            //printf("Word found: %s\n", word);
            return 1;
        }
    }
    //printf("Word not found: %s\n", word); 
    return 0;
}

char ***copyGrid(char ***grid, int rows, int columns) {
    char ***newGrid = malloc(rows * sizeof(char**));
    for (int i = 0; i < rows; ++i) {
        newGrid[i] = malloc(columns * sizeof(char*));
        for (int j = 0; j < columns; ++j) {
            newGrid[i][j] = calloc(MAX_STACK_HEIGHT, sizeof(char));
            strcpy(newGrid[i][j], grid[i][j]);
        }
    }
    return newGrid;
}

void freeValidWords(GameState *game) {
    for (int i = 0; i < validTotal; i++) free(valid[i]);
    free(valid);
    valid = NULL;
    validTotal = 0;
    (void)game;
}

void initiatedCheck(GameState *game) {
    if (!isGameInitialized) {
        loadValidWords("./tests/words.txt", game);
        isGameInitialized = 1;
    }
}

void free_game_state(GameState *game) {
    if (game != NULL) {
        for (int i = 0; i < game->row; i++) {
            for (int j = 0; j < game->column; j++) {
                free(game->grid[i][j]);
            }
            free(game->grid[i]);
        }
        free(game->grid);

        for (int i = 0; i < game->prevCount; i++) {
            for (int j = 0; j < game->prevRows[i]; j++) {
                for (int k = 0; k < game->prevColumns[i]; k++) {
                    free(game->prevGrids[i][j][k]);
                }
                free(game->prevGrids[i][j]);
            }
            free(game->prevGrids[i]);
        }
        free(game->prevGrids);
        free(game->prevRows);
        free(game->prevColumns);

        if (game->validWordsLoaded) {
            freeValidWords(game);
        }
        free(game);
    }
    isGameInitialized = 0;
}


GameState* initialize_game_state(const char *filename) {
    //printf("Loading game state from file: %s\n", filename);
    FILE *f;
    f = fopen(filename, "r");
    if (!f) {
        perror("Error opening file");
        return NULL;
    }

    int totalRows = 0, maxColumn = 0, totalColumns = 0;
    char ch;
    while ((ch = fgetc(f)) != EOF) {
        totalColumns++;
        if (ch == '\n') {
            totalRows++;
            if (totalColumns > maxColumn) {
                maxColumn = totalColumns - 1;
            }
            totalColumns = 0;
        }
    }
    if (totalColumns > 0) {
        totalRows++;
        if (totalColumns > maxColumn) maxColumn = totalColumns;
    }

    fseek(f, 0, SEEK_SET);

    GameState *game = malloc(sizeof(GameState));

    if (!game) {
        perror("Malloc failed");
        fclose(f);
        return NULL;
    }

    game->grid = NULL;
    game->row = 0;
    game->column = 0;
    game->isInitialized = 0;
    game->isFirstWordInitiated = false;
    game->validWordsLoaded = false;
    game->prevGrids = NULL;
    game->prevRows = NULL;
    game->prevColumns = NULL;
    game->prevCount = 0;
    game->prevCapacity = 0;

    game->row = totalRows;
    game->column = maxColumn;

    game->grid = malloc(game->row * sizeof(char**));
    for (int i = 0; i < game->row; ++i) {
        game->grid[i] = malloc(game->column * sizeof(char*));
        for (int j = 0; j < game->column; ++j) {
            game->grid[i][j] = calloc(MAX_STACK_HEIGHT, sizeof(char));
            strcpy(game->grid[i][j], ".");
        }
    }

    int rows = 0, columns = 0;
    while ((ch = fgetc(f)) != EOF) {
        if (ch == '\n') {
            rows++;
            columns = 0;
        } else {
            if (columns < maxColumn) {
                game->grid[rows][columns][0] = ch;
                game->grid[rows][columns][1] = '\0';
                columns++;
            }
        }
    }

    fclose(f);
    //printf("Game state initialized.\n");
    return game;
}

void increaseHorizontally(GameState *game, int new_cols) {
    //printf("Expanding horizontally. Current columns: %d, New columns: %d\n", game->column, new_cols);
    for (int i = 0; i < game->row; ++i) {
        char **newRow = realloc(game->grid[i], new_cols * sizeof(char *));
        if (newRow == NULL) {
            perror("Failed expansion horizontally");
            return;
        }
    game->grid[i] = newRow;
    for (int j = game->column; j < new_cols; ++j) {
        game->grid[i][j] = calloc(MAX_STACK_HEIGHT, sizeof(char));
        strcpy(game->grid[i][j], ".");
    }
}
    game->column = new_cols;
    //printf("Expansion complete. New column count: %d\n", game->column);
}

void increaseVertically(GameState *game, int new_rows) {
    //printf("Starting expanding board vertically to %d rows.\n", new_rows);
    char ***newGrid = realloc(game->grid, new_rows * sizeof(char **));
    if (newGrid == NULL) {
        perror("Failed to expand board vertically");
        return;
    }
    game->grid = newGrid;
    for (int i = game->row; i < new_rows; ++i) {
        game->grid[i] = malloc(game->column * sizeof(char *));
        for (int j = 0; j < game->column; ++j) {
            game->grid[i][j] = calloc(MAX_STACK_HEIGHT, sizeof(char));
            strcpy(game->grid[i][j], ".");
        }
    }
    game->row = new_rows;
    //printf("Board has been expanded vertically.\n");
}

char* captureBoardState(GameState *game) {
    int totalSize = game->row * game->column;
    char *boardState = malloc(totalSize + 1); // +1 for the null terminator
    if (!boardState) {
        fprintf(stderr, "Memory allocation failed for capturing board state.\n");
        return NULL;
    }

    int pos = 0;
    for (int i = 0; i < game->row; i++) {
        for (int j = 0; j < game->column; j++) {
            int stackHeight = 0;
            while (game->grid[i][j][stackHeight] != '\0' && stackHeight < MAX_STACK_HEIGHT) {
                stackHeight++;
            }
            boardState[pos++] = stackHeight > 0 ? game->grid[i][j][stackHeight - 1] : '.';
        }
    }
    boardState[pos] = '\0';
    return boardState;
}

bool compareBoardStates(char *stateBefore, char *stateAfter) {
    if (stateBefore == NULL || stateAfter == NULL) return false;
    return strcmp(stateBefore, stateAfter) == 0;
}

GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed) {
    if (game == NULL || game->grid == NULL) {
        fprintf(stderr, "GameState has not been initialized properly!!\n");
        return NULL;
    }

    if (row < 0 || col < 0 || row >= game->row || col >= game->column) {
        fprintf(stderr, "Initial placement position is out of bounds.\n");
        *num_tiles_placed = 0;
        return game; 
    }

    if (!game->isInitialized) {
        initiatedCheck(game); 
        game->isInitialized = 1;
    }

    direction = toupper((unsigned char)direction);
    if (direction != 'H' && direction != 'V') {
        fprintf(stderr, "Invalid direction.\n");
        return game;
    }

    char *newTiles = malloc(strlen(tiles) + 1);
    if (!newTiles) {
        fprintf(stderr, "Memory allocation failed while uppercasing.\n");
        return game;
    }
    for (size_t i = 0; i < strlen(tiles); ++i) {
        newTiles[i] = toupper(tiles[i]);
    }
    newTiles[strlen(tiles)] = '\0';

    // Determine if the board is empty
    bool isEmptyBoard = true;
    for (int i = 0; i < game->row && isEmptyBoard; i++) {
        for (int j = 0; j < game->column; j++) {
            if (game->grid[i][j][0] != '.') {
                isEmptyBoard = false;
                break;
            }
        }
    }

    if (!game->isFirstWordInitiated && isEmptyBoard) {
        if (strlen(newTiles) < 2) {
            fprintf(stderr, "First word must be minimum two letters long!!\n");
            free(newTiles);
            return game;
        }
        game->isFirstWordInitiated = true; 
    } else if (!game->isFirstWordInitiated) {
        game->isFirstWordInitiated = true;
  }
    
    if (game->prevCount == game->prevCapacity) {
    size_t newCapacity = game->prevCapacity == 0 ? 1 : game->prevCapacity * 2;
    game->prevGrids = realloc(game->prevGrids, newCapacity * sizeof(char***));
    game->prevRows = realloc(game->prevRows, newCapacity * sizeof(int));
    game->prevColumns = realloc(game->prevColumns, newCapacity * sizeof(int));

    if (!game->prevGrids || !game->prevRows || !game->prevColumns) {
        return game;
    }

    game->prevCapacity = newCapacity;
}

game->prevRows[game->prevCount] = game->row;
game->prevColumns[game->prevCount] = game->column;
game->prevGrids[game->prevCount] = copyGrid(game->grid, game->row, game->column);
game->prevCount++;

    *num_tiles_placed = 0;
    int coverCount = 0; 
    int newTilesLength = strlen(newTiles);
    for (int i = 0; i < newTilesLength; i++) {
        if (newTiles[i] != ' ') {
            int currentRow = row + (direction == 'V' ? i : 0);
            int currentCol = col + (direction == 'H' ? i : 0);
            if (currentRow >= game->row) {
    increaseVertically(game, currentRow + 1); 
}
if (currentCol >= game->column) {
    increaseHorizontally(game, currentCol + 1); 
}

            if (game->grid[currentRow][currentCol][0] != '.') {

                coverCount++;
            }
        }
    }

    int existingTileCount = 0; 
    if (direction == 'H') {
        for (int j = col; j < game->column && game->grid[row][j][0] != '.'; j++) {
            existingTileCount++;
        }
    } else {
        for (int i = row; i < game->row && game->grid[i][col][0] != '.'; i++) {
            existingTileCount++;
        }
    }

    if (coverCount >= existingTileCount && existingTileCount > 0) {
        //fprintf(stderr, "Cannot cover all tiles of an existing word.\n");
        free(newTiles);
        undo_place_tiles(game);
        return game;
    }

    bool interactsWithExistingTile = false;
    if (!isEmptyBoard) { 
        for (size_t i = 0; i < strlen(tiles); ++i) {
            if (tiles[i] == ' ') continue; 

            int currentRow = row + (direction == 'V' ? i : 0);
            int currentCol = col + (direction == 'H' ? i : 0);
            if (currentRow >= game->row) {
    increaseVertically(game, currentRow + 1); 
}
if (currentCol >= game->column) {
    increaseHorizontally(game, currentCol + 1); 
}

            bool leftOrAboveExists = (direction == 'H' && currentCol > 0 && game->grid[currentRow][currentCol - 1][0] != '.') ||
                                     (direction == 'V' && currentRow > 0 && game->grid[currentRow - 1][currentCol][0] != '.');

            bool rightOrBelowExists = (direction == 'H' && currentCol < game->column - 1 && game->grid[currentRow][currentCol + 1][0] != '.') ||
                                      (direction == 'V' && currentRow < game->row - 1 && game->grid[currentRow + 1][currentCol][0] != '.');

            bool onTopExists = game->grid[currentRow][currentCol][0] != '.';

            bool verticalNeighborExists = direction == 'H' &&
                                          ((currentRow > 0 && game->grid[currentRow - 1][currentCol][0] != '.') ||
                                           (currentRow < game->row - 1 && game->grid[currentRow + 1][currentCol][0] != '.'));

            bool horizontalNeighborExists = direction == 'V' &&
                                            ((currentCol > 0 && game->grid[currentRow][currentCol - 1][0] != '.') ||
                                             (currentCol < game->column - 1 && game->grid[currentRow][currentCol + 1][0] != '.'));

            if (leftOrAboveExists || rightOrBelowExists || onTopExists || verticalNeighborExists || horizontalNeighborExists) {
                interactsWithExistingTile = true;
                break; 
            }
        }


        if (!interactsWithExistingTile) {
            //fprintf(stderr, "Invalid move: A word must interact with an existing tile.\n");
            free(newTiles); 
            *num_tiles_placed = 0;
            undo_place_tiles(game);
            return game;
        }
    }
    char *stateBefore = captureBoardState(game);
    
    bool isValidOverlap = true; 

    for (size_t i = 0; newTiles[i] != '\0'; i++) {
        //printf("Placing tiles: \"%s\" at Row: %d, Col: %d, Direction: %c\n", tiles, row, col, direction);
        if (newTiles[i] == ' ') {
            if (direction == 'H') col++;
            else row++;
            continue;
        }

        if (direction == 'H' && col >= game->column) increaseHorizontally(game, col + 1);
        if (direction == 'V' && row >= game->row) increaseVertically(game, row + 1);

        int currStackHeight = 0;
        while (game->grid[row][col][currStackHeight] != '\0' && currStackHeight < MAX_STACK_HEIGHT) {
            currStackHeight++;
        }

        if (game->grid[row][col][0] == '.') {
            currStackHeight = 0;
        } else {
            if (game->grid[row][col][currStackHeight - 1] == newTiles[i]) {
                isValidOverlap = false; // Mark as invalid overlap
                break; // No need to check further
            }
        }

        if (currStackHeight >= MAX_STACK_HEIGHT - 1) {
            //fprintf(stderr, "Cannot place '%c' at (%d, %d). Stack height limit reached.\n", newTiles[i], row, col);
            free(newTiles);
            free(stateBefore);
            return game;
        }

        //printf("Debug: Placing '%c' at stack height %d\n", newTiles[i], currStackHeight);
        game->grid[row][col][currStackHeight] = newTiles[i];
        //printf("Placed '%c' at (%d, %d). Stack height after placement: %d\n", newTiles[i], row, col, currStackHeight + 1);
        (*num_tiles_placed)++;
        if (direction == 'H') col++;
        else row++;
        //printf("Debug: Tile '%c' placed. New stack height: %d\n", newTiles[i], currStackHeight + 1);
    }

    char *stateAfter = captureBoardState(game);
    free(newTiles);
    // printf("Gamestate before saving: \n");
    // displayBoard(game);
    
    if (!checkBoardWords(game)) {
        //printf("Board state invalid. Undo process in effect...\n");
        undo_place_tiles(game);
        *num_tiles_placed = 0; 
    }

    if (!isValidOverlap) {
        //fprintf(stderr, "Invalid move: Overlapping the same letter.\n");
        undo_place_tiles(game);
        *num_tiles_placed = 0; // Reset the number of tiles placed
        return game;
    }

    if (compareBoardStates(stateBefore, stateAfter)) {
    // The board state hasn't changed, indicating an invalid move
        //fprintf(stderr, "Invalid move: No new word has been created. Undo process in effect...\n");
        undo_place_tiles(game);
        *num_tiles_placed = 0; 
    
    }
    free(stateBefore);
    free(stateAfter);
    //printf("Number of tiles placed: %d\n", *num_tiles_placed);
    return game;
}


void displayBoard(GameState *game) {
    printf("Current board state:\n");
    for (int i = 0; i < game->row; i++) {
        for (int j = 0; j < game->column; j++) {
            int stackHeight = 0;
            while (game->grid[i][j][stackHeight] != '\0' && stackHeight < MAX_STACK_HEIGHT) {
                stackHeight++;
            }
            if (stackHeight > 0) printf("%c", game->grid[i][j][stackHeight - 1]); 
            else printf(".");  
        }
        printf("\n");
    }
}

bool checkBoardWords(GameState *game) {
    char *wordBuffer = malloc((game->row > game->column ? game->row : game->column) + 1);
    if (!wordBuffer) {
        perror("Memory allocation for wordBuffer failed");
        return false;
    }

    memset(wordBuffer, 0, (game->row > game->column ? game->row : game->column) + 1);

    for (int i = 0; i < game->row; ++i) {
        int wordLength = 0;
        for (int j = 0; j <= game->column; ++j) {
            if (j < game->column && game->grid[i][j][0] != '.') {
                int stackHeight = 0;
                while (game->grid[i][j][stackHeight] != '\0' && stackHeight < MAX_STACK_HEIGHT) stackHeight++;
                wordBuffer[wordLength++] = game->grid[i][j][stackHeight - 1]; 
            } else {
                if (wordLength > 1) {
                    wordBuffer[wordLength] = '\0';
                    if (!isWordValid(wordBuffer)) {
                        //printf("Invalid word has been found: %s\n", wordBuffer);
                        free(wordBuffer);
                        return false;
                    }
                }
                wordLength = 0; 
            }
        }
    }

    for (int j = 0; j < game->column; ++j) {
        int wordLength = 0;
        for (int i = 0; i <= game->row; ++i) {
            if (i < game->row && game->grid[i][j][0] != '.') {
                int stackHeight = 0;
                while (game->grid[i][j][stackHeight] != '\0' && stackHeight < MAX_STACK_HEIGHT) stackHeight++;
                wordBuffer[wordLength++] = game->grid[i][j][stackHeight - 1];
            } else {
                if (wordLength > 1) {
                    wordBuffer[wordLength] = '\0';
                    if (!isWordValid(wordBuffer)) {
                        //printf("Invalid word found: %s\n", wordBuffer);
                        free(wordBuffer);
                        return false;
                    }
                }
                wordLength = 0; 
            }
        }
    }

    free(wordBuffer);
    return true;
}

GameState* undo_place_tiles(GameState *game) {
    if (!game || game->prevCount == 0) return game; 
    
    int lastIdx = game->prevCount - 1;
    char ***prevGrid = game->prevGrids[lastIdx];
    int prevRow = game->prevRows[lastIdx];
    int prevColumn = game->prevColumns[lastIdx];
    
    for (int i = 0; i < game->row; i++) {
        for (int j = 0; j < game->column; j++) {
            free(game->grid[i][j]);
        }
        free(game->grid[i]);
    }
    free(game->grid);
    
    game->grid = prevGrid;
    game->row = prevRow;
    game->column = prevColumn;
    
    game->prevGrids[lastIdx] = NULL;
    game->prevRows[lastIdx] = 0;
    game->prevColumns[lastIdx] = 0;
    
    game->prevCount--;
    
    return game;
}

void save_game_state(GameState *game, const char *filename) {
    FILE *destination = fopen(filename, "w");
    if (!destination) {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < game->row; i++) {
        for (int k = 0; k < game->column; k++) {
            int savingStackHeight = 0;
            while (game->grid[i][k][savingStackHeight] != '\0' && savingStackHeight < MAX_STACK_HEIGHT) {
                savingStackHeight++;
            }
            if (savingStackHeight > 0) {
                fprintf(destination, "%c", game->grid[i][k][savingStackHeight - 1]);
                //printf("Debug: Writing '%c' at (%d,%d) with stack height %d\n", game->grid[i][k][savingStackHeight - 1], i, k, savingStackHeight);
            } else {
                fprintf(destination, ".");
                //printf("Debug: Writing '.' at (%d,%d) as it's empty\n", i, k);
            }
        }
        fprintf(destination, "\n");
    }

    printf("\n");

    for (int i = 0; i < game->row; i++) {
        for (int k = 0; k < game->column; k++) {
            int stackHeight = 0;
            if (game->grid[i][k][0] == '.') {
                fprintf(destination, "0");
                //printf("Debug: Stack height at (%d,%d) is %d\n", i, k, stackHeight);
            } else {
                while (game->grid[i][k][stackHeight] != '\0' && stackHeight < MAX_STACK_HEIGHT) {
                    stackHeight++;
                }
                fprintf(destination, "%d", stackHeight);
                //printf("Debug: Stack height at (%d,%d) is %d\n", i, k, stackHeight);
            }
        }
        fprintf(destination, "\n");
    }
    fclose(destination);
}



