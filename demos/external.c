/*

============================================================
                        DEMO INFO

DEMO NAME:          external.c
DEMO WRITTEN BY:    mukid (hum)
CREATION DATE:      2023-11-15
LAST UPDATED:       2023-11-15

============================================================
                        DEMO PURPOSE

This demo shows how to use external functions and variables 
to interact with bytecode outside of the context and how to 
create basic loops with jump markers.

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

// include OS libraries necessary for getting time

#ifdef WIN32
    #include <windows.h>
#endif

#ifdef linux
    #include <time.h>
#endif

// timer var
double timer = 0.0f;

// cross-platform "get global time" function
double get_global_time() {
    // get time on windows
    #ifdef WIN32
        FILETIME file_time;
        GetSystemTimeAsFileTime(&file_time);
        LONGLONG ll_now = (LONGLONG)file_time.dwLowDateTime + ((LONGLONG)(file_time.dwHighDateTime) << 32LL);
        return ((double)ll_now / (double)1.0e7);
    #endif
    // get time on linux
    #ifdef linux
        struct timespec spec;
        clock_gettime(CLOCK_REALTIME, &spec);
        return (double)spec.tv_sec + ((double)spec.tv_nsec / (double)1.0e9);
    #endif
    return 0.f;
}

// start the timer
void start_time() {
    timer = get_global_time();
}

// get time since timer has started
float get_time() {
    return (float)(get_global_time() - timer);
}

// bytecode 'time' variable
muVariable bc_time = { 0 };

// bytecode function to assign the result of 'get_time' to bytecode 'time' variable
void bytecode_get_time(muContext* context) {
    // set value of variable to result of 'get_time'
    float time = get_time();
    mu_context_set_variable_data(MU_NULL_PTR, context, bc_time, &time);
}

int main() {
    // amount of seconds the program should wait
    float seconds_to_wait = 5.f;
    // the bytes of it so we can more easily store it in the bytecode
    muByte sec_bytes[4] = { 
        *(((muByte*)&seconds_to_wait) + 0),
        *(((muByte*)&seconds_to_wait) + 1),
        *(((muByte*)&seconds_to_wait) + 2), 
        *(((muByte*)&seconds_to_wait) + 3)
    };

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

        /* declare time variable */

        // variable command  address  data type            byte size  name
        0x11,                0x01,    mu_binary(11100000), 0, 4,      't', 'i', 'm', 'e', '\0',
        //                                      ^             ^
        // note that the data type is signed, its type is now decimal, and byte size is now 4
        // which all represents a 32-bit signed decimal, AKA a float

        /* declare 'get_time' external function */

        // external function command  id  name
        0x12,                         0,  'g', 'e', 't', '_', 't', 'i', 'm', 'e', '\0',

        // main function (function ID 0x00)

        0xE0, 0,

            /* move seconds_to_wait into address 0x05 */

            // move command  source data type     byte size  value                                                   destination data type  byte size  address
            0x80,            mu_binary(11100000), 0, 4,      sec_bytes[0], sec_bytes[1], sec_bytes[2], sec_bytes[3], mu_binary(11100000),   0, 4,      0x05,

            /* print "waiting..." */

            // print command  data type             byte size  char  print spec (character)
            0x01,             mu_binary(11000000),  0, 1,      'w',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  0, 1,      'a',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  0, 1,      'i',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  0, 1,      't',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  0, 1,      'i',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  0, 1,      'n',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  0, 1,      'g',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  0, 1,      '.',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  0, 1,      '.',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  0, 1,      '.',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  0, 1,      '\n', mu_binary(10000000),

            /* set up jump marker #0 here */

            // jump marker declaration command  id
            0xF0,                               0,

            /* execute external function id #0 (AKA 'get_time' function) */

            // execute ext. func. command  data type of id      byte size of id  id
            0xE4,                          mu_binary(01000000), 0, 1,            0,

            /* store if the seconds to wait (at address 0x05) are greater than the value at address 0x01 (time variable) at temp address 0x01 */

            // greater command  source data type     byte size  address 1  address 2  dest data type       byte size  dest address
            0x94,               mu_binary(11100001), 0, 4,      0x05,      0x01,      mu_binary(01010000), 0, 1,      0x01,

            /* if value at temp address 0x01 (aka if 5 is over the time variable) is true, go back to jump marker #0 */

            // if command  source data type     byte size  address,
            0xA0,          mu_binary(01010001), 0, 1,      0x01,
                // go to jump marker command  id
                0xF1,                         0x00,
            0xA1,

            // if we've reached this point, that means we didn't go back to jump marker #0
            // which means that the time variable is over 5 seconds

            /* print "done!" */

            // print command  data type             byte size  char  print spec (character)
            0x01,             mu_binary(11000000),  0, 1,      'd',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  0, 1,      'o',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  0, 1,      'n',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  0, 1,      'e',  mu_binary(10000000),
            0x01,             mu_binary(11000000),  0, 1,      '!',  mu_binary(10000000),
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

    // find variable named "time" in bytecode
    bc_time = mu_context_get_variable(MU_NULL_PTR, &context, "time");

    // set external function named "get_time" in bytecode to 'bytecode_get_time'
    mu_context_set_external_function(MU_NULL_PTR, &context, "get_time", bytecode_get_time);

    // start timer right before we execute main
    start_time();

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
