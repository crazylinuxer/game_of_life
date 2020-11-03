#ifndef LIFE_DRAWER_H
#define LIFE_DRAWER_H

#include <SDL2/SDL.h>

#include "life_runner.h"


typedef struct
{
    uint32_t x;
    uint32_t y;
}
coordinates;

typedef struct
{
    life_runner game;
    SDL_Window* window;
    double size_ratio_x;
    double size_ratio_y;
}
life_drawer;


// Init the life drawer and the game; open the window
void life_drawer_init(life_drawer* self, uint32_t pixels_x, uint32_t pixels_y, uint32_t cells_x, uint32_t cells_y);

// Delete the life drawer and the game; close the window
void life_drawer_delete(life_drawer* self);

// Redraw the game
void life_drawer_redraw(const life_drawer* self);

// Resize the game window
void life_drawer_change_window_size(life_drawer* self, uint32_t pixels_x, uint32_t pixels_y);

// Resize the game (number of cells)
void life_drawer_change_game_size(life_drawer* self, uint32_t cells_x, uint32_t cells_y);

// Change cell by coordinates of pixel
void life_drawer_change_cell(life_drawer* self, uint32_t pixel_x, uint32_t pixel_y, bool value);

// Draw line of cells or empty spaces on range of pixels
void life_drawer_draw_line(life_drawer* self, coordinates begin, coordinates end, bool value);

#endif
