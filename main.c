#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "include/utils.h"
#include "include/main.h"

#define SAVEGAME_FILENAME "saved_game.life"


uint32_t step_delay;  // minimum is 20
bool run;
bool pause;
bool lmb_pressed;
bool rmb_pressed;
uint8_t speed;

bool moved_once;
direction movement;
bool move;
key_handler keyhandler;
io_threader threader;


void apply_movement(bool lock_drawer)
{
    if (move)
    {
        if ((!moved_once && keyhandler.pressed_keys.alt) || !keyhandler.pressed_keys.alt)
        {
            uint8_t distance = 1;
            if (keyhandler.pressed_keys.shift)
            {
                distance = 4;
            }
            if (!lmb_pressed && !rmb_pressed && !keyhandler.pressed_keys.alt)
            {
                distance *= 2;
            }
            if (lock_drawer)
            {
                io_threader_lock_drawer(&threader);
            }
            life_runner_move_game(&threader.drawer.game, movement, distance);
            if (lock_drawer)
            {
                io_threader_unlock_drawer(&threader);
            }
        }
        moved_once = true;
    }
    else
    {
        moved_once = false;
    }
}


int main()
{
    step_delay = 36;
    run = true;
    pause = true;
    lmb_pressed = false;
    rmb_pressed = false;
    speed = 1;
    moved_once = false;
    move = false;
    struct timeval  start_time, end_time;

    key_handler_init(&keyhandler, &pause, &movement, &move, &speed);
    io_threader_init(&threader, 1600, 900, 512, 288, &lmb_pressed, &rmb_pressed, &move, &speed);

    SDL_Event event;
    while (run)
    {
        gettimeofday(&start_time, NULL);
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                run = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_c:
                    {
                        move = false;
                        moved_once = true;
                        lmb_pressed = false;
                        rmb_pressed = false;
                        bit_array2d* new_field = bit_array2d_init(threader.drawer.game.field->x_size, threader.drawer.game.field->y_size);
                        io_threader_lock_drawer(&threader);
                        bit_array2d_delete(threader.drawer.game.field);
                        threader.drawer.game.field = new_field;
                        io_threader_unlock_drawer(&threader);
                        break;
                    }
                    case SDLK_s:
                    {
                        io_threader_lock_drawer(&threader);
                        life_runner_snapshot snapshot = life_runner_to_snapshot(&threader.drawer.game);
                        io_threader_unlock_drawer(&threader);

                        save_runner_snapshot_to_file(&snapshot, SAVEGAME_FILENAME);
                        life_runner_snapshot_delete(&snapshot);
                        break;
                    }
                    case SDLK_l:
                    {
                        life_runner_snapshot file_snapshot;
                        if (load_runner_snapshot_from_file(&file_snapshot, SAVEGAME_FILENAME, false))
                        {
                            io_threader_lock_drawer(&threader);
                            life_runner_from_snapshot(&threader.drawer.game, file_snapshot, true);
                            life_drawer_field_fit(&threader.drawer);
                            io_threader_unlock_drawer(&threader);
                            life_runner_snapshot_delete(&file_snapshot);
                        }
                        break;
                    }
                    case SDLK_ESCAPE:
                    {
                        run = false;
                    }
                    
                    default:
                    {
                        break;
                    }
                }
                key_handler_down(&keyhandler, event.key.keysym.sym);
            }
            if (event.type == SDL_KEYUP)
            {
                key_handler_up(&keyhandler, event.key.keysym.sym);
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

        apply_movement(true);
        if (!lmb_pressed && !rmb_pressed)
        {
            if (!pause)
            {
                io_threader_lock_drawer(&threader);
                for (uint8_t step = 0; step < round(pow(1.435, speed)); step++)
                {
                    life_runner_make_step(&threader.drawer.game);
                    if ((step % 7) == 1)
                    {
                        life_drawer_redraw(&threader.drawer);
                        apply_movement(false);
                    }
                    if (!run)
                    {
                        break;
                    }
                }
                io_threader_unlock_drawer(&threader);
            }
        }

        if (!move || moved_once)
        {
            gettimeofday(&end_time, NULL);
            int64_t msec_total =
                ((double)(end_time.tv_usec - start_time.tv_usec) / 1000) +
                ((double)(end_time.tv_sec - start_time.tv_sec) * 1000);
            if (((int64_t)step_delay - msec_total) > 3)
            {
                sleep_ms((int64_t)step_delay - msec_total);
            }
            else
            {
                sleep_ms(3);
            }
            
        }
    }

    io_threader_delete(&threader);
    return 0;
}
