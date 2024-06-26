#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include "hw3.h"

// typedef struct GameState {
//     char ***grid; 
//     int row;      
//     int column;
//     int isInitialized;
//     bool isFirstWordInitiated; 
//     bool validWordsLoaded;
//     char ***prevGrid; 
//     int prevRow;      
//     int prevColumn;
// } GameState;

// void initiatedCheck();
// int isWordValid(const char* word);
// void freeValidWords();
// void loadValidWords(const char* filename, GameState* game);
// void increaseHorizontally(GameState *game, int new_cols);
// void increaseVertically(GameState *game, int new_rows);
// void displayBoard(GameState *game);
// bool checkBoardWords(GameState *game);
// GameState* initialize_game_state(const char *filename);
// GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed);
// GameState* undo_place_tiles(GameState *game);
// void free_game_state(GameState *game);
// void save_game_state(GameState *game, const char *filename);
// void freeGameMemory(GameState *game);
// char ***copyGrid(char ***grid, int rows, int columns);

// #define MAX_STACK_HEIGHT 5 

// char **valid = NULL;
// int validTotal = 0;
// int isGameInitialized = 0;


// void loadValidWords(const char* filename, GameState *game) {
//     printf("Initiating all valid words from: %s\n", filename);
//     FILE *file = fopen(filename, "r");
//     if (!file) {
//         perror("Failed to open the words file");
//         return;
//     }

//     char *currWord = NULL;
//     size_t length = 0;
//     ssize_t readIt;
//     while ((readIt = getline(&currWord, &length, file)) != -1) {
//         currWord[strcspn(currWord, "\n")] = 0; 
//         for (int i = 0; currWord[i]; i++) currWord[i] = toupper((unsigned char)currWord[i]);
//         char **temp = realloc(valid, sizeof(char*) * (validTotal + 1));
//         if (temp == NULL) {
//             free(currWord);
//             perror("Failed to resize valid words array");
//             break;
//         }
//         valid = temp;
//         valid[validTotal++] = strdup(currWord);
//     }
//     free(currWord); 
//     fclose(file);
//     printf("Loaded %d valid words.\n", validTotal);
//     game->validWordsLoaded = true;
// }

// int isWordValid(const char* word) {
//     //printf("Total words loaded: %d\n", validTotal);
//     // if (validTotal > 0) {
//     //     //printf("First word: %s\n", valid[0] ? valid[0] : "NULL");
//     //     //printf("Last word: %s\n", valid[validTotal - 1] ? valid[validTotal - 1] : "NULL");
//     //     // if (validTotal > 123) {
//     //     //     //printf("Sample word: %s\n", valid[123] ? valid[123] : "NULL");
//     //     // }
//     // }
//     //printf("Validating word: %s\n", word); // Debug print
//     for (int i = 0; i < validTotal; i++) {
//         if (valid[i] && strcmp(word, valid[i]) == 0) {
//             //printf("Word found: %s\n", word);
//             return 1;
//         }
//     }
//     //printf("Word not found: %s\n", word); // Debug print for failure
//     return 0;
// }


// char ***copyGrid(char ***grid, int rows, int columns) {
//     char ***newGrid = malloc(rows * sizeof(char**));
//     for (int i = 0; i < rows; ++i) {
//         newGrid[i] = malloc(columns * sizeof(char*));
//         for (int j = 0; j < columns; ++j) {
//             newGrid[i][j] = calloc(MAX_STACK_HEIGHT, sizeof(char));
//             strcpy(newGrid[i][j], grid[i][j]);
//         }
//     }
//     return newGrid;
// }

// void freeValidWords(GameState *game) {
//     for (int i = 0; i < validTotal; i++) free(valid[i]);
//     free(valid);
//     valid = NULL;
//     validTotal = 0;
//     (void)game;
// }

// void initiatedCheck(GameState *game) {
//     if (!isGameInitialized) {
//         loadValidWords("./tests/words.txt", game);
//         isGameInitialized = 1;
//     }
// }

// void free_game_state(GameState *game) {
//     if (game != NULL) {
//         for (int i = 0; i < game->row; i++) {
//             for (int j = 0; j < game->column; j++) {
//                 free(game->grid[i][j]);
//             }
//             free(game->grid[i]);
//         }
//         free(game->grid);
//         if (game->validWordsLoaded) {
//             freeValidWords(game);
//         }
//         free(game);
//     }

//     isGameInitialized = 0;

//     //game->isInitialized = false;
// }

// GameState* initialize_game_state(const char *filename) {
//     printf("Loading game state from file: %s\n", filename);
//     FILE *f;
//     f = fopen(filename, "r");
//     if (!f) {
//         perror("Error opening file");
//         return NULL;
//     }

//     int totalRows = 0, maxColumn = 0, totalColumns = 0;
//     char ch;
//     while ((ch = fgetc(f)) != EOF) {
//         totalColumns++;
//         if (ch == '\n') {
//             totalRows++;
//             if (totalColumns > maxColumn) {
//                 maxColumn = totalColumns - 1;
//             }
//             totalColumns = 0;
//         }
//     }
//     if (totalColumns > 0) {
//         totalRows++;
//         if (totalColumns > maxColumn) maxColumn = totalColumns;
//     }

//     fseek(f, 0, SEEK_SET);

//     GameState *game = malloc(sizeof(GameState));
//     game->isInitialized = 0; 
//     game->isFirstWordInitiated = false;

//     if (!game) {
//         perror("Malloc failed");
//         fclose(f);
//         return NULL;
//     }

//     game->row = totalRows;
//     game->column = maxColumn;

//     game->grid = malloc(game->row * sizeof(char**));
//     for (int i = 0; i < game->row; ++i) {
//         game->grid[i] = malloc(game->column * sizeof(char*));
//         for (int j = 0; j < game->column; ++j) {
//             game->grid[i][j] = calloc(MAX_STACK_HEIGHT, sizeof(char));
//             strcpy(game->grid[i][j], ".");
//         }
//     }

//     int rows = 0, columns = 0;
//     while ((ch = fgetc(f)) != EOF) {
//         if (ch == '\n') {
//             rows++;
//             columns = 0;
//         } else {
//             if (columns < maxColumn) {
//                 game->grid[rows][columns][0] = ch;
//                 game->grid[rows][columns][1] = '\0';
//                 columns++;
//             }
//         }
//     }

//     fclose(f);
//     printf("Game state initialized.\n");
//     return game;
// }

// void increaseHorizontally(GameState *game, int new_cols) {
//     printf("Expanding horizontally. Current columns: %d, New columns: %d\n", game->column, new_cols);
//     for (int i = 0; i < game->row; ++i) {
//         char **newRow = realloc(game->grid[i], new_cols * sizeof(char *));
//         if (newRow == NULL) {
//             perror("Failed expansion horizontally");
//             return;
//         }
//     game->grid[i] = newRow;
//     for (int j = game->column; j < new_cols; ++j) {
//         game->grid[i][j] = calloc(MAX_STACK_HEIGHT, sizeof(char));
//         strcpy(game->grid[i][j], ".");
//     }
// }
//     game->column = new_cols;
//     printf("Expansion complete. New column count: %d\n", game->column);
// }

// void increaseVertically(GameState *game, int new_rows) {
//     printf("Starting expanding board vertically to %d rows.\n", new_rows);
//     char ***newGrid = realloc(game->grid, new_rows * sizeof(char **));
//     if (newGrid == NULL) {
//         perror("Failed to expand board vertically");
//         return;
//     }
//     game->grid = newGrid;
//     for (int i = game->row; i < new_rows; ++i) {
//         game->grid[i] = malloc(game->column * sizeof(char *));
//         for (int j = 0; j < game->column; ++j) {
//             game->grid[i][j] = calloc(MAX_STACK_HEIGHT, sizeof(char));
//             strcpy(game->grid[i][j], ".");
//         }
//     }
//     game->row = new_rows;
//     printf("Board has been expanded vertically.\n");
// }

// char* captureBoardState(GameState *game) {
//     int totalSize = game->row * game->column;
//     char *boardState = malloc(totalSize + 1); // +1 for the null terminator
//     if (!boardState) {
//         fprintf(stderr, "Memory allocation failed for capturing board state.\n");
//         return NULL;
//     }

//     int pos = 0;
//     for (int i = 0; i < game->row; i++) {
//         for (int j = 0; j < game->column; j++) {
//             int stackHeight = 0;
//             while (game->grid[i][j][stackHeight] != '\0' && stackHeight < MAX_STACK_HEIGHT) {
//                 stackHeight++;
//             }
//             boardState[pos++] = stackHeight > 0 ? game->grid[i][j][stackHeight - 1] : '.';
//         }
//     }
//     boardState[pos] = '\0';
//     return boardState;
// }

// bool compareBoardStates(char *stateBefore, char *stateAfter) {
//     if (stateBefore == NULL || stateAfter == NULL) return false; // Safety check
//     return strcmp(stateBefore, stateAfter) == 0;
// }


// GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed) {
//     if (game == NULL || game->grid == NULL) {
//         fprintf(stderr, "GameState has not been initialized properly!!\n");
//         return NULL;
//     }

//     if (row < 0 || col < 0 || row >= game->row || col >= game->column) {
//         fprintf(stderr, "Initial placement position is out of bounds.\n");
//         return game; 
//     }

//     if (!game->isInitialized) {
//         initiatedCheck(game); 
//         game->isInitialized = 1;
//     }

//     direction = toupper((unsigned char)direction);
//     if (direction != 'H' && direction != 'V') {
//         fprintf(stderr, "Invalid direction.\n");
//         return game;
//     }

//     char *newTiles = malloc(strlen(tiles) + 1);
//     if (!newTiles) {
//         fprintf(stderr, "Memory allocation failed while uppercasing.\n");
//         return game;
//     }
//     for (size_t i = 0; i < strlen(tiles); ++i) {
//         newTiles[i] = toupper(tiles[i]);
//     }
//     newTiles[strlen(tiles)] = '\0';

//     // Determine if the board is empty
//     bool isEmptyBoard = true;
//     for (int i = 0; i < game->row && isEmptyBoard; i++) {
//         for (int j = 0; j < game->column; j++) {
//             if (game->grid[i][j][0] != '.') {
//                 isEmptyBoard = false;
//                 break;
//             }
//         }
//     }

//     // Check for first word conditions
//     if (!game->isFirstWordInitiated && isEmptyBoard) {
//         // If it's the first word and the board is empty, check the length of the tiles
//         if (strlen(newTiles) < 2) {
//             fprintf(stderr, "First word must be minimum two letters long!!\n");
//             free(newTiles);
//             return game;
//         }
//         game->isFirstWordInitiated = true; 
//     } else if (!game->isFirstWordInitiated) {
//         // If it's the first word but the board is not empty (i.e., loaded from a file)
//         game->isFirstWordInitiated = true;
//   }
//     game->prevRow = game->row;
//     game->prevColumn = game->column;
//     game->prevGrid = copyGrid(game->grid, game->row, game->column);

//     *num_tiles_placed = 0;
//     int coverCount = 0; // Count of tiles that will be placed on existing tiles
//     int newTilesLength = strlen(newTiles);
//     for (int i = 0; i < newTilesLength; i++) {
//         if (newTiles[i] != ' ') {
//             int currentRow = row + (direction == 'V' ? i : 0);
//             int currentCol = col + (direction == 'H' ? i : 0);
//             if (currentRow >= game->row) {
//     increaseVertically(game, currentRow + 1); // Ensure the grid has enough rows
// }
// if (currentCol >= game->column) {
//     increaseHorizontally(game, currentCol + 1); // Ensure the grid has enough columns
// }

//             if (game->grid[currentRow][currentCol][0] != '.') {
//                 //changes
                
//                 coverCount++;
//             }
//         }
//     }

//     int existingTileCount = 0; 
//     if (direction == 'H') {
//         for (int j = col; j < game->column && game->grid[row][j][0] != '.'; j++) {
//             existingTileCount++;
//         }
//     } else {
//         for (int i = row; i < game->row && game->grid[i][col][0] != '.'; i++) {
//             existingTileCount++;
//         }
//     }

//     if (coverCount >= existingTileCount && existingTileCount > 0) {
//         fprintf(stderr, "Cannot cover all tiles of an existing word.\n");
//         free(newTiles);
//         game->column = game->prevColumn;
//         game->row = game->prevRow;
//         game->grid = copyGrid(game->prevGrid, game->row, game->column);
//         return game;
//     }

//     bool interactsWithExistingTile = false;
//     if (!isEmptyBoard) { // Skip this check if it's the first placement
//         for (size_t i = 0; i < strlen(tiles); ++i) {
//             if (tiles[i] == ' ') continue; // Skip spaces in the tiles string

//             int currentRow = row + (direction == 'V' ? i : 0);
//             int currentCol = col + (direction == 'H' ? i : 0);
//             if (currentRow >= game->row) {
//     increaseVertically(game, currentRow + 1); // Ensure the grid has enough rows
// }
// if (currentCol >= game->column) {
//     increaseHorizontally(game, currentCol + 1); // Ensure the grid has enough columns
// }

//             // Check interaction in all relevant directions
//             bool leftOrAboveExists = (direction == 'H' && currentCol > 0 && game->grid[currentRow][currentCol - 1][0] != '.') ||
//                                      (direction == 'V' && currentRow > 0 && game->grid[currentRow - 1][currentCol][0] != '.');

//             bool rightOrBelowExists = (direction == 'H' && currentCol < game->column - 1 && game->grid[currentRow][currentCol + 1][0] != '.') ||
//                                       (direction == 'V' && currentRow < game->row - 1 && game->grid[currentRow + 1][currentCol][0] != '.');

//             bool onTopExists = game->grid[currentRow][currentCol][0] != '.';

//             bool verticalNeighborExists = direction == 'H' &&
//                                           ((currentRow > 0 && game->grid[currentRow - 1][currentCol][0] != '.') ||
//                                            (currentRow < game->row - 1 && game->grid[currentRow + 1][currentCol][0] != '.'));

//             bool horizontalNeighborExists = direction == 'V' &&
//                                             ((currentCol > 0 && game->grid[currentRow][currentCol - 1][0] != '.') ||
//                                              (currentCol < game->column - 1 && game->grid[currentRow][currentCol + 1][0] != '.'));

//             if (leftOrAboveExists || rightOrBelowExists || onTopExists || verticalNeighborExists || horizontalNeighborExists) {
//                 interactsWithExistingTile = true;
//                 break; // Found an interaction, no need to check further
//             }
//         }


//         if (!interactsWithExistingTile) {
//             // If no interaction with existing tiles, it's an invalid move (unless it's the first move on an empty board)
//             fprintf(stderr, "Invalid move: A word must interact with an existing tile.\n");
//             free(newTiles); // Remember to free allocated memory before returning
//             game->column = game->prevColumn;
//             game->row = game->prevRow;
//             game->grid = copyGrid(game->prevGrid, game->row, game->column);
//             return game;
//         }
//     }
//     char *stateBefore = captureBoardState(game);
    
//     for (size_t i = 0; newTiles[i] != '\0'; i++) {
//         printf("Placing tiles: \"%s\" at Row: %d, Col: %d, Direction: %c\n", tiles, row, col, direction);
//         if (newTiles[i] == ' ') {
//             if (direction == 'H') col++;
//             else row++;
//             continue;
//         }

//         if (direction == 'H' && col >= game->column) increaseHorizontally(game, col + 1);
//         if (direction == 'V' && row >= game->row) increaseVertically(game, row + 1);

//         int currStackHeight = 0;
//         while (game->grid[row][col][currStackHeight] != '\0' && currStackHeight < MAX_STACK_HEIGHT) {
//             currStackHeight++;
//         }

//         if (game->grid[row][col][0] == '.') {
//             currStackHeight = 0;
//         } 

//         if (currStackHeight >= MAX_STACK_HEIGHT) {
//             fprintf(stderr, "Cannot place '%c' at (%d, %d). Stack height limit reached.\n", newTiles[i], row, col);
//             free(newTiles);
//             return game;
//         }

//         printf("Debug: Placing '%c' at stack height %d\n", newTiles[i], currStackHeight);
//         game->grid[row][col][currStackHeight] = newTiles[i];
//         printf("Placed '%c' at (%d, %d). Stack height after placement: %d\n", newTiles[i], row, col, currStackHeight + 1);
//         (*num_tiles_placed)++;
//         if (direction == 'H') col++;
//         else row++;
//         printf("Debug: Tile '%c' placed. New stack height: %d\n", newTiles[i], currStackHeight + 1);
//     }

//     char *stateAfter = captureBoardState(game);
//     free(newTiles);
//     // printf("Gamestate before saving: \n");
//     // displayBoard(game);
    
//     if (!checkBoardWords(game)) {
//         printf("Board state invalid. Undo process in effect...\n");
//         undo_place_tiles(game);
//         *num_tiles_placed = 0; 
//     }

//     if (compareBoardStates(stateBefore, stateAfter)) {
//     // The board state hasn't changed, indicating an invalid move
//         fprintf(stderr, "Invalid move: No new word has been created. Undo process in effect...\n");
//         undo_place_tiles(game);
//         *num_tiles_placed = 0; 
    
//     }
//     free(stateBefore);
//     free(stateAfter);
//     printf("Number of tiles placed: %d\n", *num_tiles_placed);
//     return game;
// }


// void displayBoard(GameState *game) {
//     printf("Current board state:\n");
//     for (int i = 0; i < game->row; i++) {
//         for (int j = 0; j < game->column; j++) {
//             int stackHeight = 0;
//             while (game->grid[i][j][stackHeight] != '\0' && stackHeight < MAX_STACK_HEIGHT) {
//                 stackHeight++;
//             }
//             if (stackHeight > 0) printf("%c", game->grid[i][j][stackHeight - 1]); 
//             else printf(".");  
//         }
//         printf("\n");
//     }
// }

// bool checkBoardWords(GameState *game) {
//     char *wordBuffer = malloc((game->row > game->column ? game->row : game->column) + 1);
//     if (!wordBuffer) {
//         perror("Memory allocation for wordBuffer failed");
//         return false;
//     }

//     memset(wordBuffer, 0, (game->row > game->column ? game->row : game->column) + 1);

//     // Check horizontally
//     for (int i = 0; i < game->row; ++i) {
//         int wordLength = 0;
//         for (int j = 0; j <= game->column; ++j) {
//             if (j < game->column && game->grid[i][j][0] != '.') {
//                 int stackHeight = 0;
//                 while (game->grid[i][j][stackHeight] != '\0' && stackHeight < MAX_STACK_HEIGHT) stackHeight++;
//                 wordBuffer[wordLength++] = game->grid[i][j][stackHeight - 1]; // Take the top tile
//             } else {
//                 if (wordLength > 1) {
//                     wordBuffer[wordLength] = '\0';
//                     if (!isWordValid(wordBuffer)) {
//                         //printf("Invalid word has been found: %s\n", wordBuffer);
//                         free(wordBuffer);
//                         return false;
//                     }
//                 }
//                 wordLength = 0; // Reset for the next word
//             }
//         }
//     }

//     // Check vertically
//     for (int j = 0; j < game->column; ++j) {
//         int wordLength = 0;
//         for (int i = 0; i <= game->row; ++i) {
//             if (i < game->row && game->grid[i][j][0] != '.') {
//                 int stackHeight = 0;
//                 while (game->grid[i][j][stackHeight] != '\0' && stackHeight < MAX_STACK_HEIGHT) stackHeight++;
//                 wordBuffer[wordLength++] = game->grid[i][j][stackHeight - 1]; // Take the top tile
//             } else {
//                 if (wordLength > 1) {
//                     wordBuffer[wordLength] = '\0';
//                     if (!isWordValid(wordBuffer)) {
//                         //printf("Invalid word found: %s\n", wordBuffer);
//                         free(wordBuffer);
//                         return false;
//                     }
//                 }
//                 wordLength = 0; // Reset for the next word
//             }
//         }
//     }

//     free(wordBuffer);
//     return true;
// }


// GameState* undo_place_tiles(GameState *game) {
//     if (!game || !game->prevGrid) return game; // Nothing to undo

//     // Free current grid
//     for (int i = 0; i < game->row; i++) {
//         for (int j = 0; j < game->column; j++) {
//             free(game->grid[i][j]);
//         }
//         free(game->grid[i]);
//     }
//     free(game->grid);

//     // Restore previous grid
//     game->grid = game->prevGrid;
//     game->row = game->prevRow;
//     game->column = game->prevColumn;

//     // Nullify prevGrid to prevent double-freeing
//     game->prevGrid = NULL;

//     //printf("Undo successful. Board reverted to previous state.\n");
//     return game;
// }

// void save_game_state(GameState *game, const char *filename) {
//     FILE *destination = fopen(filename, "w");
//     if (!destination) {
//         perror("Error opening file");
//         return;
//     }

    
//     for (int i = 0; i < game->row; i++) {
//         for (int k = 0; k < game->column; k++) {
//             int savingStackHeight = 0;
//             while (game->grid[i][k][savingStackHeight] != '\0' && savingStackHeight < MAX_STACK_HEIGHT) {
//                 savingStackHeight++;
//             }
//             if (savingStackHeight > 0) {
//                 fprintf(destination, "%c", game->grid[i][k][savingStackHeight - 1]);
//                 //printf("Debug: Writing '%c' at (%d,%d) with stack height %d\n", game->grid[i][k][savingStackHeight - 1], i, k, savingStackHeight);
//             } else {
//                 fprintf(destination, ".");
//                 //printf("Debug: Writing '.' at (%d,%d) as it's empty\n", i, k);
//             }
//         }
//         fprintf(destination, "\n");
//     }

//     printf("\n");
//     //fprintf(destination, "\n");

    
//     // Print the stack heights
//     for (int i = 0; i < game->row; i++) {
//         for (int k = 0; k < game->column; k++) {
//             int stackHeight = 0;
//             // Check if the cell is actually empty
//             if (game->grid[i][k][0] == '.') {
//                 fprintf(destination, "0");
//                 //printf("Debug: Stack height at (%d,%d) is %d\n", i, k, stackHeight);
//             } else {
//                 while (game->grid[i][k][stackHeight] != '\0' && stackHeight < MAX_STACK_HEIGHT) {
//                     stackHeight++;
//                 }
//                 fprintf(destination, "%d", stackHeight);
//                 //printf("Debug: Stack height at (%d,%d) is %d\n", i, k, stackHeight);
//             }
//         }
//         fprintf(destination, "\n");
//     }

//     fclose(destination);
// }


// void testCase() {


//     static const char *actual_filename = "./tests/actual_outputs/test_output.txt";
//     static int num_tiles_placed;

//     GameState *game = initialize_game_state("./tests/boards/board04.txt");
//     game = place_tiles(game, 8, 18, 'V', "TREAT", &num_tiles_placed);
//     save_game_state(game, actual_filename);
//     free_game_state(game);

//     // GameState *game33 = initialize_game_state("./tests/boards/board02.txt"); 
//     // game33 = place_tiles(game33,0 , 0, 'H', "CANDY", &num_tiles_placed);
//     // game33 = place_tiles(game33,0 , 0, 'H', "R", &num_tiles_placed);
//     // game33 = place_tiles(game33,0 , 0, 'H', "B", &num_tiles_placed);
//     // game33 = place_tiles(game33,0 , 0, 'H', "S", &num_tiles_placed);
//     // game33 = place_tiles(game33,0 , 0, 'H', "H", &num_tiles_placed);
//     // game33 = place_tiles(game33,0 , 0, 'H', "D", &num_tiles_placed);
//     // free_game_state(game33);

//     // printf("----------------------------------------------------------------\n");
// 	// GameState *game = initialize_game_state("./tests/boards/board04.txt");
//     // game = place_tiles(game, 4, 19, 'H', "INSERT", &num_tiles_placed);
//     // save_game_state(game, actual_filename);
//     // free_game_state(game);

//     // printf("----------------------------------------------------------------\n");

//     // GameState *game9 = initialize_game_state("/workspaces/cse220_hw3/tests/boards/board01.txt"); 
//     // game9 = place_tiles(game9, 0, 2, 'V', "UNUSURPING", &num_tiles_placed);
//     // save_game_state(game9, actual_filename);
//     // free_game_state(game9);

//     // printf("----------------------------------------------------------------\n");

//     // GameState *game31 = initialize_game_state("./tests/boards/board05.txt");
//     // game31 = place_tiles(game31, 0, 0, 'H', "CAT", &num_tiles_placed);
//     // save_game_state(game31, actual_filename);
//     // free_game_state(game31);

//     // printf("----------------------------------------------------------------\n");

// 	// GameState *game2 = initialize_game_state("./tests/boards/board01.txt"); 
//     // game2 = place_tiles(game2, 3, 2, 'H', "C", &num_tiles_placed);
//     // save_game_state(game2, actual_filename);
//     // free_game_state(game2);

//     // printf("----------------------------------------------------------------\n");

//     // GameState *game11 = initialize_game_state("./tests/boards/board04.txt");
//     // game11 = place_tiles(game11, 2, 7, 'V', "H B ID", &num_tiles_placed);
//     // printf("Board state after tiles placement:\n");
//     // displayBoard(game11);
//     // save_game_state(game11, actual_filename);
//     // free_game_state(game11);

//     // printf("----------------------------------------------------------------\n");
    
//     // GameState *game22 = initialize_game_state("./tests/boards/board02.txt");
//     // game22 = place_tiles(game22, 0, 0, 'H', "CAT", &num_tiles_placed);
//     // game22 = place_tiles(game22, 0, 0, 'H', "TOT", &num_tiles_placed);
//     // save_game_state(game22, actual_filename);
//     // free_game_state(game22);

//     // GameState *game3 = initialize_game_state("./tests/boards/board08.txt"); 
//     // game3 = place_tiles(game3, 6, 12, 'H', " ROID", &num_tiles_placed);
//     // save_game_state(game3, actual_filename);
//     // free_game_state(game3);

// 	// GameState *game = initialize_game_state("./tests/boards/board04.txt");
//     // game = place_tiles(game, 4, 19, 'H', "INSERT", &num_tiles_placed);
//     // save_game_state(game, actual_filename);
//     // free_game_state(game);

//     // GameState *game2 = initialize_game_state("./tests/boards/board04.txt");
//     // game2 = place_tiles(game2, 6, 11, 'H', "SNEER", &num_tiles_placed);
//     // save_game_state(game2, actual_filename);
//     // free_game_state(game2);

//     // GameState *game4 = initialize_game_state("./tests/boards/board04.txt");
//     // game4 = place_tiles(game4, 4, 12, 'V', "DINGO", &num_tiles_placed);
//     // save_game_state(game4, actual_filename);
//     // free_game_state(game4);

//     // GameState *game5 = initialize_game_state("./tests/boards/board04.txt");
//     // game5 = place_tiles(game5, 2, 7, 'V', "H B ID", &num_tiles_placed);
//     // save_game_state(game5, actual_filename);
//     // free_game_state(game5);
// }

int main(void) {
    //testCase();
    return 0;
}
