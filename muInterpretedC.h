/*
muInterpretedC - Muukid
Public domain single-file C library for running C as an interpreted language.
No warranty implied; use at your own risk.

Licensed under MIT License or public domain, whichever you prefer.
More explicit license information at the end of the file.

@TODO test non-8-bit-width values
*/

#ifndef MUIC_H
#define MUIC_H

#ifdef __cplusplus
	extern "C" {
#endif

/* basics */

#define MUIC_VERSION_MAJOR 1
#define MUIC_VERSION_MINOR 0
#define MUIC_VERSION_PATCH 0

#ifndef MUDEF
    #ifdef MU_STATIC
        #define MUDEF static
    #else
        #define MUDEF extern
    #endif
#endif

/* C standard library dependencies */

#if !defined(mu_print) || !defined(mu_printf)
	#ifdef MUCOSA_NO_PRINT
		#ifndef mu_print
			#define mu_print(msg)
		#endif
		#ifndef mu_printf
			#define mu_printf(msg, ...)
		#endif
	#else
		#include <stdio.h>
		#ifndef mu_print
			#define mu_print printf
		#endif
		#ifndef mu_printf
			#define mu_printf printf
		#endif
	#endif
#endif

#ifndef size_m
    #include <stddef.h>
    #define size_m size_t
#endif

#if !defined(uint64_m) || \
	!defined(uint32_m) || \
	!defined(uint16_m) || \
	!defined(uint8_m)

	#include <stdint.h>

	#ifndef uint64_m
		#define uint64_m uint64_t
	#endif

	#ifndef uint32_m
		#define uint32_m uint32_t
	#endif

	#ifndef uint16_m
		#define uint16_m uint16_t
	#endif
	
	#ifndef uint8_m
		#define uint8_m uint8_t
	#endif

#endif

#if !defined(mu_malloc)  || \
    !defined(mu_free)    || \
    !defined(mu_realloc)

    #include <stdlib.h>

    #ifndef mu_malloc
    	#define mu_malloc malloc
    #endif

    #ifndef mu_free
    	#define mu_free free
    #endif

    #ifndef mu_realloc
    	#define mu_realloc realloc
    #endif

#endif

#if !defined(mu_memset) || \
	!defined(mu_memcpy)

	#include <string.h>

	#ifndef mu_memset
		#define mu_memset memset
	#endif

	#ifndef mu_memcpy
		#define mu_memcpy memcpy
	#endif

#endif

/* basic types/defines */

#ifndef MU_BOOL
	#define MU_BOOL
	enum _muBool { MU_FALSE, MU_TRUE };
	typedef enum _muBool _muBool;
	#define muBool int
#endif

#ifndef muResult
	#define muResult  muBool
	#define MU_FAILURE MU_FALSE
	#define MU_SUCCESS MU_TRUE
#endif

#define muByte unsigned char

#ifndef MU_NULL
    #define MU_NULL 0
#endif

#ifndef MU_NULL_PTR
    #define MU_NULL_PTR 0
#endif

/* structs */

struct muContext {
	muByte bitwidth;
	size_m static_memory_len;
	muByte* static_memory;
	// reg0 is memory for virtually anything
	size_m reg0_len;
	muByte* reg0;
	// reg1 is almost exclusively for holding memory addresses
	size_m reg1_len;
	muByte* reg1;
};
typedef struct muContext muContext;
#define muContext muContext

/* binary/bit/byte handling */

#define mu_binary(b) mu_string_to_binary(#b)

// from right to left starting at 0
#define mu_bitcheck(num, bit) (((num) & (1<<(bit)))!=0)

/* header check macros */

#define MU_BEGINNING_HEADER_LENGTH 16
#define MU_END_HEADER_LENGTH 4

/* functions */

// context
MUDEF muContext mu_context_create(muResult* result, muByte* bytecode, size_m bytecode_len);
MUDEF muContext mu_context_destroy(muResult* result, muContext context);

// binary/bit/byte handling
MUDEF muByte mu_string_to_binary(const char* s);

#ifdef __cplusplus
	}
#endif

#endif /* MUIC_H */

#ifdef MUIC_IMPLEMENTATION

#ifdef __cplusplus
	extern "C" {
#endif

uint8_m mu_get_uint8_from_bytecode(muByte* bytecode) {
	return bytecode[0];
}

uint16_m mu_get_uint16_from_bytecode(muByte* bytecode) {
	muByte static_memory_len_bytes[2] = { bytecode[1], bytecode[0] };
	return (*(uint16_m*)&static_memory_len_bytes[0]);
}

uint32_m mu_get_uint32_from_bytecode(muByte* bytecode) {
	muByte static_memory_len_bytes[4] = { bytecode[3], bytecode[2], bytecode[1], bytecode[0] };
	return (*(uint32_m*)&static_memory_len_bytes[0]);
}

uint64_m mu_get_uint64_from_bytecode(muByte* bytecode) {
	muByte static_memory_len_bytes[8] = { bytecode[7], bytecode[6], bytecode[5], bytecode[4], bytecode[3], bytecode[2], bytecode[1], bytecode[0] };
	return (*(uint64_m*)&static_memory_len_bytes[0]);
}

muResult mu_bytecode_check_header_validity(muByte* bytecode, size_m bytecode_len) {
	// length check
	if (bytecode_len < MU_BEGINNING_HEADER_LENGTH + MU_END_HEADER_LENGTH) {
		mu_print("[MUIC] Failed to create context; bytecode is invalid length.\n");
		return MU_FAILURE;
	}

	/* BEGINNING HEADER INFO */

	// file signature check
	if (
		bytecode[0] != mu_binary(01101101) || 
		bytecode[1] != mu_binary(01110101) || 
		bytecode[2] != mu_binary(01101001) || 
		bytecode[3] != mu_binary(01100011)
	) {
		mu_print("[MUIC] Failed to create context; invalid file signature.\n");
		return MU_FAILURE;
	}

	// bitwidth
	if (
		bytecode[4] != 8 && bytecode[4] != 16 &&
		bytecode[4] != 32 && bytecode[4] != 64
	) {
		mu_print("[MUIC] Failed to create context; invalid bit width.\n");
		return MU_FAILURE;
	}

	// version checking
	if (
		bytecode[5] != MUIC_VERSION_MAJOR || 
		bytecode[6] != MUIC_VERSION_MINOR ||
		bytecode[7] != MUIC_VERSION_PATCH
	) {
		mu_print("[MUIC] Failed to create context; muic version specified in bytecode does not match with the version of this interpreter.\n");
		return MU_FAILURE;
	}

	// end header
	if (
		bytecode[bytecode_len-4] != mu_binary(01100101) ||
		bytecode[bytecode_len-3] != mu_binary(01101110) ||
		bytecode[bytecode_len-2] != mu_binary(01100100) ||
		bytecode[bytecode_len-1] != mu_binary(11111111)
	) {
		mu_print("[MUIC] Failed to create context; invalid end header.\n");
		return MU_FAILURE;
	}

	return MU_SUCCESS;
}

// data type
struct muicDataType {
	muBool sign;
	muByte type;
	muByte pointer_count;
	uint16_m byte_size;
};
typedef struct muicDataType muicDataType;

#define MUIC_DATA_TYPE_UNSIGNED 0
#define MUIC_DATA_TYPE_SIGNED 1
#define MUIC_DATA_TYPE_VOID 0
#define MUIC_DATA_TYPE_STRUCT 1
#define MUIC_DATA_TYPE_INTEGER 2
#define MUIC_DATA_TYPE_DECIMAL 3

muicDataType mu_get_data_type_from_bytecode(muByte* bytecode) {
	muicDataType dt = {0};

	dt.sign = mu_bitcheck(bytecode[0], 7);
	dt.type = mu_bitcheck(bytecode[0], 5) + (mu_bitcheck(bytecode[0], 6) * 2);
	dt.pointer_count = mu_bitcheck(bytecode[0], 0) + 
	(mu_bitcheck(bytecode[0], 1) * 2) +
	(mu_bitcheck(bytecode[0], 2) * 4) +
	(mu_bitcheck(bytecode[0], 3) * 8) +
	(mu_bitcheck(bytecode[0], 4) * 16);
	dt.byte_size = mu_get_uint16_from_bytecode(&bytecode[1]);

	return dt;
}

// registers

uint64_m mu_context_get_reg_pointer_value(muByte* reg, size_m reg_len) {
	switch (reg_len) {
	default:
		return 0;
		break;
	case 1:
		return mu_get_uint8_from_bytecode(reg);
		break;
	case 2:
		return mu_get_uint16_from_bytecode(reg);
		break;
	case 4:
		return mu_get_uint32_from_bytecode(reg);
		break;
	case 8:
		return mu_get_uint64_from_bytecode(reg);
		break;
	}
}

muResult mu_context_fill_reg0_with_data_type(muContext* context, muicDataType dt, muByte* bytecode) {
	if ((context->reg0 == MU_NULL_PTR) || (context->reg0_len < dt.byte_size)) {
		if (context->reg0 != MU_NULL_PTR) {
			mu_free(context->reg0);
		}
		context->reg0 = mu_malloc(dt.byte_size);
		context->reg0_len = dt.byte_size;
	}
	// @TODO maybe extra out-of-range checking here? and for rest of func
	if (dt.pointer_count == 0) {
		mu_memcpy(context->reg0, bytecode, dt.byte_size);
	} else {
		if (context->reg0_len < context->bitwidth / 8) {
			context->reg0 = mu_realloc(context->reg0, context->bitwidth / 8);
			context->reg0_len = context->bitwidth / 8;
		}
		mu_memcpy(context->reg0, bytecode, context->bitwidth);
		while (dt.pointer_count > 0) {
			if (dt.pointer_count != 1) {
				mu_memcpy(context->reg0, &context->static_memory[mu_context_get_reg_pointer_value(context->reg0, context->bitwidth / 8)], context->bitwidth / 8);
			} else {
				mu_memcpy(context->reg0, &context->static_memory[mu_context_get_reg_pointer_value(context->reg0, context->bitwidth / 8)], dt.byte_size);
			}
			if (
				dt.pointer_count != 1 && 
				(mu_context_get_reg_pointer_value(context->reg0, context->bitwidth / 8) + context->bitwidth / 8) > context->static_memory_len
			) {
				mu_print("[MUIC] WARNING! Invalid memory address modification attempt!\n");
				return MU_FAILURE;
			}
			dt.pointer_count--;
		}
	}
	return MU_SUCCESS;
}

muResult mu_context_fill_reg1_with_data_type(muContext* context, muicDataType dt, muByte* bytecode) {
	if ((context->reg1 == MU_NULL_PTR) || (context->reg1_len < dt.byte_size)) {
		if (context->reg1 != MU_NULL_PTR) {
			mu_free(context->reg1);
		}
		context->reg1 = mu_malloc(dt.byte_size);
		context->reg1_len = dt.byte_size;
	}
	if (dt.pointer_count == 0) {
		mu_memcpy(context->reg1, bytecode, dt.byte_size);
	} else {
		if (context->reg1_len < context->bitwidth / 8) {
			context->reg1 = mu_realloc(context->reg1, context->bitwidth / 8);
			context->reg1_len = context->bitwidth / 8;
		}
		mu_memcpy(context->reg1, bytecode, context->bitwidth);
		while (dt.pointer_count > 0) {
			if (dt.pointer_count != 1) {
				mu_memcpy(context->reg1, &context->static_memory[mu_context_get_reg_pointer_value(context->reg1, context->bitwidth / 8)], context->bitwidth / 8);
			} else {
				mu_memcpy(context->reg1, &context->static_memory[mu_context_get_reg_pointer_value(context->reg1, context->bitwidth / 8)], dt.byte_size);
			}
			if (
				dt.pointer_count != 1 && 
				(mu_context_get_reg_pointer_value(context->reg1, context->bitwidth / 8) + context->bitwidth / 8) > context->static_memory_len
			) {
				mu_print("[MUIC] WARNING! Invalid memory address modification attempt!\n");
				return MU_FAILURE;
			}
			dt.pointer_count--;
		}
	}
	return MU_SUCCESS;
}

// instructions

muResult mu_instruction_move(muContext* context, muByte* bytecode) {
	size_m offset = 0;

	// @TODO check for reg fill function results
	// @TODO make sure we're in range here
	// obtain value from src_dt into reg0 (pointer=0 means we have actual value)
	muicDataType src_dt = mu_get_data_type_from_bytecode(bytecode);
	mu_context_fill_reg0_with_data_type(context, src_dt, &bytecode[3]);

	// obtain value from dst_dt into reg1 (pointer=0 means we have pointer to set value at)
	offset = 0;
	offset += 3;
	if (src_dt.pointer_count > 0) {
		offset += context->bitwidth / 8;
	} else {
		offset += src_dt.byte_size;
	}
	muicDataType dst_dt = mu_get_data_type_from_bytecode(&bytecode[offset]);
	mu_context_fill_reg1_with_data_type(context, dst_dt, &bytecode[offset+3]);

	// set memory address point stored in reg1 to reg0
	uint64_m reg1_val = mu_context_get_reg_pointer_value(context->reg1, context->bitwidth / 8);
	if ((reg1_val + src_dt.byte_size > context->static_memory_len) || reg1_val == 0) {
		mu_print("[MUIC] WARNING! Invalid memory address modification attempt!\n");
		return MU_FAILURE;
	}

	if (src_dt.sign == dst_dt.sign && src_dt.type == dst_dt.type) {
		size_m min_byte_size = src_dt.byte_size;
		if (dst_dt.byte_size < src_dt.byte_size) {
			min_byte_size = dst_dt.byte_size;
		}
		for (size_m i = 0; i < src_dt.byte_size && i < dst_dt.byte_size; i++) {
			context->static_memory[reg1_val+min_byte_size-i-1] = context->reg0[src_dt.byte_size-i-1];
		}
	}
	
	return MU_SUCCESS;
}

/* functions */

MUDEF muContext mu_context_create(muResult* result, muByte* bytecode, size_m bytecode_len) {
    muContext context = {0};
	
	if (mu_bytecode_check_header_validity(bytecode, bytecode_len) != MU_SUCCESS) {
		if (result != MU_NULL_PTR) {
			*result = MU_FAILURE;
		}
		return context;
	}

	context.bitwidth = bytecode[4];
	context.static_memory_len = mu_get_uint32_from_bytecode(&bytecode[8]);

	if (context.static_memory_len > 0) {
		context.static_memory = mu_malloc(context.static_memory_len * sizeof(muByte));
		mu_memset(context.static_memory, 0, context.static_memory_len * sizeof(muByte));
	} else {
		context.static_memory = MU_NULL_PTR;
	}

	context.reg0 = MU_NULL_PTR;

	if (result != MU_NULL_PTR) {
		*result = MU_SUCCESS;
	}
	return context;
}

MUDEF muContext mu_context_destroy(muResult* result, muContext context) {
	if (context.static_memory != MU_NULL_PTR) {
		mu_free(context.static_memory);
		context.static_memory = MU_NULL_PTR;
	}

	if (context.reg0 != MU_NULL_PTR) {
		mu_free(context.reg0);
		context.reg0 = MU_NULL_PTR;
	}

	if (context.reg1 != MU_NULL_PTR) {
		mu_free(context.reg1);
		context.reg1 = MU_NULL_PTR;
	}

	return context;
}

MUDEF muByte mu_string_to_binary(const char* s) {
	muByte i = 0;
	while (*s) {
		i <<= 1;
		i += *s++ - '0';
	}
	return i;
}

#ifdef __cplusplus
	}
#endif

#endif /* MUIC_IMPLEMENTATION */

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
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
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
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
------------------------------------------------------------------------------
*/
