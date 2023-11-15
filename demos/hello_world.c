/*

============================================================
                        DEMO INFO

DEMO NAME:          hello_world.c
DEMO WRITTEN BY:    mukid (hum)
CREATION DATE:      2023-11-14
LAST UPDATED:       2023-11-14

============================================================
                        DEMO PURPOSE

This demo simply shows how to construct a simple "Hello, 
World!" demo in muBytecode, displaying the basic syntactic 
sugar of the bytecode like beginning/ending headers, the 
main function, basic command structure, and the print
command.

============================================================

============================================================
                        LICENSE INFO

All code is licensed under MIT License or public domain, 
whichever you prefer.
More explicit license information at the end of file.

============================================================

*/

#define MUB_QUICK_COMPILE
#define MUB_IMPLEMENTATION
#include "muBytecode.h"

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
        // extra settings data
        0, 0, 0, 0,

        // main function (function ID 0x00)

        0xE0, 0,

            // print command  data type             data type byte size                       char  print spec (character)
            0x01,             mu_binary(11000000),  mu_binary(00000000), mu_binary(00000001), 'H',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  mu_binary(00000000), mu_binary(00000001), 'e',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  mu_binary(00000000), mu_binary(00000001), 'l',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  mu_binary(00000000), mu_binary(00000001), 'l',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  mu_binary(00000000), mu_binary(00000001), 'o',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  mu_binary(00000000), mu_binary(00000001), ',',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  mu_binary(00000000), mu_binary(00000001), ' ',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  mu_binary(00000000), mu_binary(00000001), 'W',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  mu_binary(00000000), mu_binary(00000001), 'o',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  mu_binary(00000000), mu_binary(00000001), 'r',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  mu_binary(00000000), mu_binary(00000001), 'l',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  mu_binary(00000000), mu_binary(00000001), 'd',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  mu_binary(00000000), mu_binary(00000001), '!',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  mu_binary(00000000), mu_binary(00000001), '\n', mu_binary(10000000),

            // return command  data type             data type byte size                       return value
            0x00,              mu_binary(01000000),  mu_binary(00000000), mu_binary(00000001), 0,

        0xE1,

        /* END HEADER */

        'e', 'n', 'd', mu_binary(11111111),
    };

    // create context with bytecode
    muContext context = mu_context_create(MU_NULL_PTR, bytecode, sizeof(bytecode));

    // execute main function (ignore return value)
    mu_context_execute_main(MU_NULL_PTR, &context);

    // destroy context
    context = mu_context_destroy(MU_NULL_PTR, context);

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
