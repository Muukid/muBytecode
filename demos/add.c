/*

============================================================
                        DEMO INFO

DEMO NAME:          add.c
DEMO WRITTEN BY:    mukid (hum)
CREATION DATE:      2023-11-14
LAST UPDATED:       2023-11-14

============================================================
                        DEMO PURPOSE

This demo shows how to add numbers by storing them in 
memory, dereferencing their addresses, and using the add 
command to store the result in a separate address and 
printing the result.

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
    // two values to add
    int8_m val_0 = 9;
    int8_m val_1 = 10;

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


            /* move val_0 to address 0x01 */

            // move command  source data type     byte size  value  destination data type  byte size  address
            0x80,            mu_binary(01000000), 0, 1,      val_0, mu_binary(01000000),   0, 1,      0x01,
            //                                ^
            // note that the destination data type's dereference count is 0 but is still
            // interpreted as an address due to it being the destination

            /* move val_1 to address 0x02 */
            // note that both addresses can be one byte away from each other because each value is only 1 byte

            // move command  source data type     byte size  value  destination data type  byte size  address
            0x80,            mu_binary(01000000), 0, 1,      val_1, mu_binary(01000000),   0, 1,      0x02,

            /* add values at address 0x01 (val_0) and 0x02 (val_1) and store the result at address 0x03 */

            // add command  source 0 data type   byte size  address  source 1 data type   byte size  address  dest data type       byte size  address
            0x81,           mu_binary(01000001), 0, 1,      0x01,    mu_binary(01000001), 0, 1,      0x02,    mu_binary(01000000), 0, 1,      0x03,
            //                               ^              ^                         ^              ^
            // note that now source 0 and source 1 are addresses to the values we want to use,
            // so we set our dereference count in both data types to 1, so that the interpreter
            // uses the values at the addresses in the calculation, not the addresses themselves!

            /* print the value at address 0x03, where we stored the result of the add command */

            // print command  data type             byte size  address  print spec (integer)
            0x01,             mu_binary(01000001),  0, 1,      0x03,    mu_binary(00000000),

            /* print newline character to be nice :) */

            // print command  data type             byte size  char  print spec (character)
            0x01,             mu_binary(11000000),  0, 1,      '\n', mu_binary(10000000),

            /* return 0 */

            // return command  data type             byte size  return value
            0x00,              mu_binary(01000000),  0, 1,      0,

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
