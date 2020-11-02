#include <stdio.h>
#include <stdlib.h>

#include "include/bit_array2d.h"
#include "include/life_runner.h"
#include "include/utils.h"
#include "include/life_drawer.h"
#include "include/key_handlers.h"
#include "include/io_threader.h"


#define PIXELS_PER_MS 8000


int main()
{
    uint32_t game_slowdown = 25;
    
    bool run = true;
    volatile bool pause = true;
    volatile bool lmb_pressed = false;
    volatile bool rmb_pressed = false;

    bool moved_once = false;
    volatile direction movement;
    volatile bool move = false;

    volatile io_threader threader;
    io_threader_init(&threader, 1600, 900, 512, 288, &lmb_pressed, &rmb_pressed, &move);

    SDL_Event event;
    while (run)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                run = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_c)
                {
                    move = false;
                    moved_once = true;
                    lmb_pressed = false;
                    rmb_pressed = false;
                    volatile bit_array2d* new_field = bit_array2d_init(threader.drawer.game.field->x_size, threader.drawer.game.field->y_size);
                    // io_threader_output_lock_drawer(&threader);
                    // io_threader_input_lock_drawer(&threader);
                    bit_array2d_delete(threader.drawer.game.field);
                    threader.drawer.game.field = new_field;
                    // io_threader_input_unlock_drawer(&threader);
                    // io_threader_output_unlock_drawer(&threader);
                    break;
                }
                keydown_handler(event.key.keysym.sym, &pause, &movement, &move);
            }
            if (event.type == SDL_KEYUP)
            {
                keyup_handler(event.key.keysym.sym, &pause, &movement, &move);
            }
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    lmb_pressed = true;
                }
                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    rmb_pressed = true;
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    lmb_pressed = false;
                }
                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    rmb_pressed = false;
                }
            }
        }

        if (move)
        {
            if ((!moved_once && pressed_keys.alt) || !pressed_keys.alt)
            {
                uint8_t distance = 1;
                if (pressed_keys.shift)
                {
                    distance = 4;
                }
                if (!lmb_pressed && !rmb_pressed && !pressed_keys.alt)
                {
                    distance *= 2;
                }
                // io_threader_input_lock_drawer(&threader);
                // io_threader_output_lock_drawer(&threader);
                life_runner_move_game(&threader.drawer.game, movement, distance);
                // io_threader_output_unlock_drawer(&threader);
                // io_threader_input_unlock_drawer(&threader);
            }
            moved_once = true;
        }
        else
        {
            moved_once = false;
        }

        if (!lmb_pressed && !rmb_pressed)
        {
            if (!pause)
            {
                // io_threader_input_lock_drawer(&threader);
                // io_threader_output_lock_drawer(&threader);
                life_runner_make_step(&threader.drawer.game);
                // io_threader_output_unlock_drawer(&threader);
                // io_threader_input_unlock_drawer(&threader);
            }
        }

        if (!move || moved_once)
        {
            sleep_ms(game_slowdown);
        }
    }

    io_threader_delete(&threader);
    return 0;
}
