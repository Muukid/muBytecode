#define MUIC_IMPLEMENTATION
#include "muInterpretedC.h"

#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif

int main() {
    muByte bytecode[] = {
        /* BEGINNING HEADER */
        // file signature
        mu_binary(01101101), mu_binary(01110101), mu_binary(01101001), mu_binary(01100011),
        // bit-width         version major        version minor        version patch
        mu_binary(00001000), mu_binary(00000001), mu_binary(00000000), mu_binary(00000000),
        // static memory allocation bytes
        mu_binary(00000000), mu_binary(00000000), mu_binary(00000000), mu_binary(00010000),
        // extra settings data
        mu_binary(10000000), mu_binary(00000000), mu_binary(00000000), mu_binary(00000000),

        

        /* END HEADER */
        // e                 n                    d
        mu_binary(01100101), mu_binary(01101110), mu_binary(01100100), mu_binary(11111111)
    };

    muContext context = mu_context_create(MU_NULL_PTR, bytecode, sizeof(bytecode) / sizeof(muByte));

    muByte test_command[] = {
        mu_binary(01100000), mu_binary(00000000), mu_binary(00000010), mu_binary(10010010), mu_binary(01001001),
        mu_binary(01100000), mu_binary(00000000), mu_binary(00000001), mu_binary(00000100)
    };
    mu_instruction_move(&context, test_command);

    if (context.static_memory != MU_NULL_PTR) {
        printf("memory: \n");
        for (size_m i = 0; i < context.static_memory_len; i++) {
            for (size_m j = 0; j < 8; j++) {
                printf("%d", !!((context.static_memory[i] << j) & 0x80));
            }
            printf(" ");
            if ((i+1) % 4 == 0 && i != 0) {
                printf("\n");
            }
        }
    }

    context = mu_context_destroy(MU_NULL_PTR, context);

    printf("done\n");

    /*printf("bytecode: \n");
    for (size_m i = 0; i < sizeof(bytecode) / sizeof(muByte); i++) {
        for (size_m j = 0; j < 8; j++) {
            printf("%d", !!((bytecode[i] << j) & 0x80));
        }
        printf(" ");
        if ((i+1) % 4 == 0 && i != 0) {
            printf("\n");
        }
    }*/
    
#ifdef WIN32
    Sleep(2000);
#endif
    return 0;
}
