#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "include/bit_array2d.h"


void exit_on_syntax_error(void)
{
    fprintf(stderr, "%s", "Invalid command line parameters detected!\n");
    exit(1);
}


void* check_pointer_after_malloc(void* ptr)
{
    if (!ptr)
    {
        fprintf(stderr, "%s", "An error occurred while allocating memory! Exiting\n");
        exit(1);
    }
    return ptr;
}


void print_field(bit_array2d* field)
{
    for (uint32_t y = 0; y < field->y_size; y++)
    {
        for (uint32_t x = 0; x < field->x_size; x++)
        {
            printf("%d ", bit_array2d_get_bit(field, x, y));
        }
        printf("\n");
    }
}