/*

============================================================
                        DEMO INFO

DEMO NAME:          conditional.c
DEMO WRITTEN BY:    mukid (hum)
CREATION DATE:      2023-11-14
LAST UPDATED:       2023-11-16

============================================================
                        DEMO PURPOSE

This demo shows how to store conditional statements about
two numbers in the temporary memory and perform if/else 
conditional code based on the results.

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

int main() {
    // two values to compare
    int8_m val_0 = 4;
    int8_m val_1 = 7;

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

        // main function (function ID 0x00)

        0xE0, 0,


            /* move val_0 to address 0x01 */

            // move command  source data type     byte size  value  destination data type  byte size  address
            0x80,            mu_binary(01000000), 0, 1,      val_0, mu_binary(01000000),   0, 1,      0x01,

            /* move val_1 to address 0x02 */

            // move command  source data type     byte size  value  destination data type  byte size  address
            0x80,            mu_binary(01000000), 0, 1,      val_1, mu_binary(01000000),   0, 1,      0x02,

            /* store if val_0 is greater than val_1 in address 0x01 in the temp memory */

            // greater command  source data type     byte size  address 1  address 2  dest data type       byte size  address
            0x94,               mu_binary(01000001), 0, 1,      0x01,      0x02,      mu_binary(01010000), 0, 1,      0x01,
            //                                                                                     ^
            // note that we now have to set the temp bit to 1 to let the interpreter know that we
            // want to store this in the temp memory!

            /* store if val_0 is less than val_1 in address 0x02 in the temp memory */

            // less command  source data type     byte size  address 1  address 2  dest data type       byte size  address
            0x92,            mu_binary(01000001), 0, 1,      0x01,      0x02,      mu_binary(01010000), 0, 1,      0x02,

            /* store if val_0 is equal to val_1 in address 0x03 in the temp memory */

            // equal command  source data type     byte size  address 1  address 2  dest data type       byte size  address
            0x90,             mu_binary(01000001), 0, 1,      0x01,      0x02,      mu_binary(01010000), 0, 1,      0x03,

            /* check if the result of the "greater than" check at temp address 0x01 is true */

            // if command  source data type     byte size  address,
            0xA0,          mu_binary(01010001), 0, 1,      0x01,

                // the code in here will now only execute if the check was true, AKA if val_0 is greater than val_1

                /* print val_0 */

                // print command  data type             byte size  address  print spec (integer)
                0x01,             mu_binary(01000001),  0, 1,      0x01,    mu_binary(00000000),

                /* print " > " */
                // print command  data type             byte size  char  print spec (character)
                0x01,             mu_binary(11000000),  0, 1,      ' ',  mu_binary(10000000),
                0x01,             mu_binary(11000000),  0, 1,      '>',  mu_binary(10000000),
                0x01,             mu_binary(11000000),  0, 1,      ' ',  mu_binary(10000000),

                /* print val_1 */

                // print command  data type             byte size  address  print spec (integer)
                0x01,             mu_binary(01000001),  0, 1,      0x02,    mu_binary(00000000),

                /* print newline */

                // print command  data type             byte size  char  print spec (character)
                0x01,             mu_binary(11000000),  0, 1,      '\n', mu_binary(10000000),

            // end if command
            0xA1,

            /* check if the result of the "less than" check at temp address 0x02 is true */

            // if command  source data type     byte size  address,
            0xA0,          mu_binary(01010001), 0, 1,      0x02,

                // the code in here will now only execute if the check was true, AKA if val_0 is less than val_1

                /* print val_0 */

                // print command  data type             byte size  address  print spec (integer)
                0x01,             mu_binary(01000001),  0, 1,      0x01,    mu_binary(00000000),

                /* print " < " */
                // print command  data type             byte size  char  print spec (character)
                0x01,             mu_binary(11000000),  0, 1,      ' ',  mu_binary(10000000),
                0x01,             mu_binary(11000000),  0, 1,      '<',  mu_binary(10000000),
                0x01,             mu_binary(11000000),  0, 1,      ' ',  mu_binary(10000000),

                /* print val_1 */

                // print command  data type             byte size  address  print spec (integer)
                0x01,             mu_binary(01000001),  0, 1,      0x02,    mu_binary(00000000),

                /* print newline */

                // print command  data type             byte size  char  print spec (character)
                0x01,             mu_binary(11000000),  0, 1,      '\n', mu_binary(10000000),

            // end if command
            0xA1,

            /* check if the result of the "equal to" check at temp address 0x03 is true */

            // if command  source data type     byte size  address,
            0xA0,          mu_binary(01010001), 0, 1,      0x03,

                // the code in here will now only execute if the check was true, AKA if val_0 is equal to val_1

                /* print val_0 */

                // print command  data type             byte size  address  print spec (integer)
                0x01,             mu_binary(01000001),  0, 1,      0x01,    mu_binary(00000000),

                /* print " = " */
                // print command  data type             byte size  char  print spec (character)
                0x01,             mu_binary(11000000),  0, 1,      ' ',  mu_binary(10000000),
                0x01,             mu_binary(11000000),  0, 1,      '=',  mu_binary(10000000),
                0x01,             mu_binary(11000000),  0, 1,      ' ',  mu_binary(10000000),

                /* print val_1 */

                // print command  data type             byte size  address  print spec (integer)
                0x01,             mu_binary(01000001),  0, 1,      0x02,    mu_binary(00000000),

                /* print newline */

                // print command  data type             byte size  char  print spec (character)
                0x01,             mu_binary(11000000),  0, 1,      '\n', mu_binary(10000000),

            // end if command
            0xA1,

            /* check if the previous check failed, AKA if val_0 is not equal to val-1 */
            0xA2,

                // the code in here will now only execute if the previous check was false, AKA if val_0 is NOT equal to val_1

                /* print val_0 */

                // print command  data type             byte size  address  print spec (integer)
                0x01,             mu_binary(01000001),  0, 1,      0x01,    mu_binary(00000000),

                /* print " != " */
                // print command  data type             byte size  char  print spec (character)
                0x01,             mu_binary(11000000),  0, 1,      ' ',  mu_binary(10000000),
                0x01,             mu_binary(11000000),  0, 1,      '!',  mu_binary(10000000),
                0x01,             mu_binary(11000000),  0, 1,      '=',  mu_binary(10000000),
                0x01,             mu_binary(11000000),  0, 1,      ' ',  mu_binary(10000000),

                /* print val_1 */

                // print command  data type             byte size  address  print spec (integer)
                0x01,             mu_binary(01000001),  0, 1,      0x02,    mu_binary(00000000),

                /* print newline */

                // print command  data type             byte size  char  print spec (character)
                0x01,             mu_binary(11000000),  0, 1,      '\n', mu_binary(10000000),

            // end if command
            0xA1,

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
    muContext context = mu_context_create(MU_NULL_PTR, bytecode, sizeof(bytecode), MU_TRUE);

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
