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


void loadValidWords(const char* filename, GameState *game) {
    //printf("Initiating all valid words from: %s\n", filename);
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open the words file");
        return;
    }

    char *currWord = NULL;
    size_t length = 0;
    ssize_t readIt;
    while ((readIt = getline(&currWord, &length, file)) != -1) {
        currWord[strcspn(currWord, "\n")] = 0; 
        for (int i = 0; currWord[i]; i++) currWord[i] = toupper((unsigned char)currWord[i]);
        char **temp = realloc(valid, sizeof(char*) * (validTotal + 1));
        if (temp == NULL) {
            free(currWord);
            perror("Failed to resize valid words array");
            break;
        }
        valid = temp;
        valid[validTotal++] = strdup(currWord);
    }
    free(currWord); 
    fclose(file);
    //printf("Loaded %d valid words.\n", validTotal);
    game->validWordsLoaded = true;
}

int isWordValid(const char* word) {
    //printf("Total words loaded: %d\n", validTotal);
    // if (validTotal > 0) {
    //     //printf("First word: %s\n", valid[0] ? valid[0] : "NULL");
    //     //printf("Last word: %s\n", valid[validTotal - 1] ? valid[validTotal - 1] : "NULL");
    //     // if (validTotal > 123) {
    //     //     //printf("Sample word: %s\n", valid[123] ? valid[123] : "NULL");
    //     // }
    // }
    //printf("Validating word: %s\n", word); // Debug print
    for (int i = 0; i < validTotal; i++) {
        if (valid[i] && strcmp(word, valid[i]) == 0) {
            //printf("Word found: %s\n", word);
            return 1;
        }
    }
    //printf("Word not found: %s\n", word); // Debug print for failure
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
        if (game->validWordsLoaded) {
            freeValidWords(game);
        }
        free(game);
    }

    isGameInitialized = 0;

    //game->isInitialized = false;
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
    game->isInitialized = 0; 
    game->isFirstWordInitiated = false;

    if (!game) {
        perror("Malloc failed");
        fclose(f);
        return NULL;
    }

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

GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed) {
    if (game == NULL || game->grid == NULL) {
        fprintf(stderr, "GameState has not been initialized properly!!\n");
        return NULL;
    }

    if (row < 0 || col < 0 || row >= game->row || col >= game->column) {
        fprintf(stderr, "Initial placement position is out of bounds.\n");
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

    // Check for first word conditions
    if (!game->isFirstWordInitiated && isEmptyBoard) {
        // If it's the first word and the board is empty, check the length of the tiles
        if (strlen(newTiles) < 2) {
            fprintf(stderr, "First word must be minimum two letters long!!\n");
            free(newTiles);
            return game;
        }
        game->isFirstWordInitiated = true; 
    } else if (!game->isFirstWordInitiated) {
        // If it's the first word but the board is not empty (i.e., loaded from a file)
        game->isFirstWordInitiated = true;
  }


    game->prevRow = game->row;
    game->prevColumn = game->column;
    game->prevGrid = copyGrid(game->grid, game->row, game->column);

    //Changes for overlap
    bool isValidMove = false; 

    for (size_t i = 0; i < strlen(newTiles) && !isValidMove; i++) {
        int targetRow = row + (direction == 'V' ? i : 0);
        int targetCol = col + (direction == 'H' ? i : 0);

        if (targetRow >= game->row || targetCol >= game->column) {
            // Increase board size as needed, handled elsewhere
            continue;
        }

        if (game->grid[targetRow][targetCol][0] == '.' || game->grid[targetRow][targetCol][0] != newTiles[i]) {
            // If the spot is empty or the new tile is different, it's a valid move
            isValidMove = true;
        }
    }

    *num_tiles_placed = 0;

    if (!isValidMove) {
        //fprintf(stderr, "Invalid move: Cannot simply cover an existing word with identical tiles.\n");
        free(newTiles); // Clean up before returning
        return game; // Exit the function early
    }

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
        }

        if (currStackHeight >= MAX_STACK_HEIGHT) {
            //fprintf(stderr, "Cannot place '%c' at (%d, %d). Stack height limit reached.\n", newTiles[i], row, col);
            continue;
        }

        //printf("Debug: Placing '%c' at stack height %d\n", newTiles[i], currStackHeight);
        game->grid[row][col][currStackHeight] = newTiles[i];
        //printf("Placed '%c' at (%d, %d). Stack height after placement: %d\n", newTiles[i], row, col, currStackHeight + 1);
        (*num_tiles_placed)++;
        if (direction == 'H') col++;
        else row++;
        //printf("Debug: Tile '%c' placed. New stack height: %d\n", newTiles[i], currStackHeight + 1);
    }

    free(newTiles);
    //printf("Gamestate before saving: \n");
    //displayBoard(game);

    if (!checkBoardWords(game)) {
        //printf("Board state invalid. Undo process in effect...\n");
        undo_place_tiles(game);
        *num_tiles_placed = 0;
    }

    
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

    // Check horizontally
    for (int i = 0; i < game->row; ++i) {
        int wordLength = 0;
        for (int j = 0; j <= game->column; ++j) {
            if (j < game->column && game->grid[i][j][0] != '.') {
                int stackHeight = 0;
                while (game->grid[i][j][stackHeight] != '\0' && stackHeight < MAX_STACK_HEIGHT) stackHeight++;
                wordBuffer[wordLength++] = game->grid[i][j][stackHeight - 1]; // Take the top tile
            } else {
                if (wordLength > 1) {
                    wordBuffer[wordLength] = '\0';
                    if (!isWordValid(wordBuffer)) {
                        //printf("Invalid word has been found: %s\n", wordBuffer);
                        free(wordBuffer);
                        return false;
                    }
                }
                wordLength = 0; // Reset for the next word
            }
        }
    }

    // Check vertically
    for (int j = 0; j < game->column; ++j) {
        int wordLength = 0;
        for (int i = 0; i <= game->row; ++i) {
            if (i < game->row && game->grid[i][j][0] != '.') {
                int stackHeight = 0;
                while (game->grid[i][j][stackHeight] != '\0' && stackHeight < MAX_STACK_HEIGHT) stackHeight++;
                wordBuffer[wordLength++] = game->grid[i][j][stackHeight - 1]; // Take the top tile
            } else {
                if (wordLength > 1) {
                    wordBuffer[wordLength] = '\0';
                    if (!isWordValid(wordBuffer)) {
                        //printf("Invalid word found: %s\n", wordBuffer);
                        free(wordBuffer);
                        return false;
                    }
                }
                wordLength = 0; // Reset for the next word
            }
        }
    }

    free(wordBuffer);
    return true;
}


GameState* undo_place_tiles(GameState *game) {
    if (!game || !game->prevGrid) return game; // Nothing to undo

    // Free current grid
    for (int i = 0; i < game->row; i++) {
        for (int j = 0; j < game->column; j++) {
            free(game->grid[i][j]);
        }
        free(game->grid[i]);
    }
    free(game->grid);

    // Restore previous grid
    game->grid = game->prevGrid;
    game->row = game->prevRow;
    game->column = game->prevColumn;

    // Nullify prevGrid to prevent double-freeing
    game->prevGrid = NULL;

    //printf("Undo successful. Board reverted to previous state.\n");
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
    //fprintf(destination, "\n");

    
    // Print the stack heights
    for (int i = 0; i < game->row; i++) {
        for (int k = 0; k < game->column; k++) {
            int stackHeight = 0;
            // Check if the cell is actually empty
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


