/*

============================================================
                        DEMO INFO

DEMO NAME:          dynamic_memory.c
DEMO WRITTEN BY:    mukid (hum)
CREATION DATE:      2023-11-16
LAST UPDATED:       2023-11-16

============================================================
                        DEMO PURPOSE

This demo shows how to adjust the dynamic memory buffer and 
display how certain operations affect it.

============================================================

============================================================
                        LICENSE INFO

All code is licensed under MIT License or public domain, 
whichever you prefer.
More explicit license information at the end of file.

============================================================

*/

#define MUB_IMPLEMENTATION
#include "muBytecode.h"

#include <stdio.h>

// bytecode function to print all the dynamic memory
void bytecode_print_dynamic_mem(muContext* context) {
    printf("-----------------------------------\n");
    if (context->dynamic_memory != MU_NULL_PTR) {
        printf("Dynamic memory: \n");
        for (size_m i = 0; i < context->dynamic_memory_len; i++) {
            for (size_m j = 0; j < 8; j++) {
                printf("%d", !!((context->dynamic_memory[i] << j) & 0x80));
            }
            printf(" ");
            if ((i+1) % 4 == 0 && i != 0) {
                printf("\n");
            }
        }
    } else {
        printf("No dynamic memory.\n");
    }
}

int main() {
    // Bytecode for the program:
    muByte bytecode[] = {
        /* BEGINNING HEADER */

        // file signature
        'm', 'u', 'b', 0,
        // bit-widths        version major version minor version patch
        mu_binary(00000000), 1,            0,            0,
        // static memory allocation bytes
        0, 0, 0, 16,
        // temp memory allocation bytes
        0, 0, 0, 16,

        /* declare 'print_dmem' external function */

        // external function command  id  name
        0x12,                         0,  'p', 'r', 'i', 'n', 't', '_', 'd', 'm', 'e', 'm', '\0',

        // main function (function ID 0x00)

        0xE0, 0,

            /* print dynamic memory, should be none right now. */

            // execute ext. func. command  data type of id      byte size of id  id
            0xE4,                          mu_binary(01000000), 0, 1,            0,

            /* increase dynamic memory to 12 bytes */

            // dynamic mem size set command  data type            byte size  new dynamic memory byte size
            0x02,                            mu_binary(01000000), 0, 1,      12,

            /* print dynamic memory, should be 12 bytes of nothing right now. */

            // execute ext. func. command  data type of id      byte size of id  id
            0xE4,                          mu_binary(01000000), 0, 1,            0,

            /* move 4 bytes of random binary to address #4 */

            // move
            0x80,  
            // src data type     size  random binary  
            mu_binary(00000000), 0, 4, mu_binary(10111111), mu_binary(10010111), mu_binary(11111100), mu_binary(01011010), 
            // dst data type     size  address
            mu_binary(00000000), 0, 4, 16 + 0x04,
            //                         ^^^^^^^^^
            // note that we now have to add the static memory length in order to access dynamic memory!

            /* print dynamic memory, should now have 4 bytes of garbage at address 0x04 */

            // execute ext. func. command  data type of id      byte size of id  id
            0xE4,                          mu_binary(01000000), 0, 1,            0,

            /* move 4 more bytes of random binary to address #8 */

            // move
            0x80,  
            // src data type     size  random binary  
            mu_binary(00000000), 0, 4, mu_binary(11111000), mu_binary(00010101), mu_binary(11100000), mu_binary(00100011), 
            // dst data type     size  address
            mu_binary(00000000), 0, 4, 16 + 0x08,

            /* print dynamic memory, should now have 8 bytes of garbage starting at address 0x04 */

            // execute ext. func. command  data type of id      byte size of id  id
            0xE4,                          mu_binary(01000000), 0, 1,            0,

            /* decrease memory size to only 8 bytes, cutting off memory right before address 0x08 */

            // dynamic mem size set command  data type            byte size  new dynamic memory byte size
            0x02,                            mu_binary(01000000), 0, 1,      8,

            /* print dynamic memory, should now have 4 bytes of garbage at address 0x04, and then cutting off */

            // execute ext. func. command  data type of id      byte size of id  id
            0xE4,                          mu_binary(01000000), 0, 1,            0,

            /* bump up memory back to 12 bytes, but this won't recover the garbage */

            // dynamic mem size set command  data type            byte size  new dynamic memory byte size
            0x02,                            mu_binary(01000000), 0, 1,      12,

            /* print dynamic memory, should now have 4 bytes of garbage at address 0x04, and 4 bytes of nothing */

            // execute ext. func. command  data type of id      byte size of id  id
            0xE4,                          mu_binary(01000000), 0, 1,            0,

            /* set dynamic memory size to 0, freeing all dynamic memory */
            // you don't need to do this, as all dynamic memory will automatically be freed,
            // but it's good practice!

            // dynamic mem size set command  data type            byte size  new dynamic memory byte size
            0x02,                            mu_binary(01000000), 0, 1,      0,

            /* print dynamic memory, should now be nothing */

            // execute ext. func. command  data type of id      byte size of id  id
            0xE4,                          mu_binary(01000000), 0, 1,            0,

            /* return 0 */

            // return command  data type             byte size  return value
            0x00,              mu_binary(01000000),  0, 1,      0,

        0xE1,

        /* END HEADER */

        'e', 'n', 'd', mu_binary(11111111),
    };

    // create context with bytecode
    muContext context = mu_context_create(MU_NULL_PTR, bytecode, sizeof(bytecode), MU_TRUE);

    // set external function named "print_dmem" in bytecode to 'bytecode_print_dynamic_mem'
    mu_context_set_external_function(MU_NULL_PTR, &context, "print_dmem", bytecode_print_dynamic_mem);

    // execute main function (ignore return value)
    mu_context_execute_main(MU_NULL_PTR, &context);

    // destroy context
    context = mu_context_destroy(MU_NULL_PTR, context);

    // print end bar to make look nice
    printf("-----------------------------------\n");

    return 0;
}

/*
This software is available under 2 licenses -- choose whichever you prefer.

## ALTERNATIVE A - MIT License
Copyright (c) 2023 Hum

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
