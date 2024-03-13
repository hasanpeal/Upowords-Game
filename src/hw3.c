#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "hw3.h" 

#define DEBUG(...) fprintf(stderr, "[          ] [ DEBUG ] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, " -- %s()\n", __func__)

GameState* initialize_game_state(const char *filename) {
    FILE *txtFile = fopen(filename, "r");
    if (!txtFile) 
    {
        perror("Error opening file");
        return NULL;
    }

    int totalRows = 0, maxColumn = 0, totalColumns = 0;
    char ch;
    while ((ch = fgetc(txtFile)) != EOF) 
    {
        totalColumns++;
        if (ch == '\n') 
        {
            totalRows++;
            if (totalColumns > maxColumn) 
            {
                maxColumn = totalColumns - 1; 
            }
            totalColumns = 0; 
        }
    }
    if (totalColumns > 0) 
    { 
        totalRows++; 
        if (totalColumns > maxColumn) 
        {
            maxColumn = totalColumns; 
        }
    }

    fseek(txtFile, 0, SEEK_SET);

    GameState *game = malloc(sizeof(GameState));
    if (!game) 
    {
        perror("Malloc failed");
        fclose(txtFile);
        return NULL;
    }

    game->row = totalRows;
    game->column = maxColumn;

    game->grid = malloc(game->row * sizeof(char**));
    for (int i = 0; i < game->row; ++i) 
    {
        game->grid[i] = malloc(game->column * sizeof(char*));
        for (int j = 0; j < game->column; ++j) 
        {
            game->grid[i][j] = calloc(MAX_STACK_HEIGHT, sizeof(char));
            strcpy(game->grid[i][j], ".");
        }
    }

    int rows = 0, columns = 0;
    while ((ch = fgetc(txtFile)) != EOF) 
    {
        if (ch == '\n') 
        {
            rows++;
            columns = 0;
        } else {
            if (columns < maxColumn) 
            {
                game->grid[rows][columns][0] = ch;
                game->grid[rows][columns][1] = '\0';
                columns++;
            }
        }
    }

    fclose(txtFile);
    return game;
}

GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed) {
    (void)game;
    (void)row;
    (void)col;
    (void)direction;
    (void)tiles;
    (void)num_tiles_placed;
    return NULL;
}

GameState* undo_place_tiles(GameState *game) {
    (void)game;
    return NULL;
}

void free_game_state(GameState *game) {
    (void)game;
}

void save_game_state(GameState *game, const char *filename) {
    (void)game;
    (void)filename;
}
