
#ifndef UTILS_H
#define UTILS_H

#include "bit_array2d.h"


// Print error message and exit with code 1
void exit_on_syntax_error(void);


// Return the same pointer or print error message and exit with code 1
void* check_pointer_after_malloc(void* ptr);

// Print bit_array2d
void print_field(bit_array2d* field);

#endif