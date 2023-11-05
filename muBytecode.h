/*
muBytecode - Muukid
Public domain single-file C library for storing and executing bytecode.
No warranty implied; use at your own risk.

Licensed under MIT License or public domain, whichever you prefer.
More explicit license information at the end of the file.

@TODO test non-8-bit-width values
*/

#ifndef MUB_H
#define MUB_H

#ifdef __cplusplus
	extern "C" {
#endif

/* basics */

#define MUB_VERSION_MAJOR 1
#define MUB_VERSION_MINOR 0
#define MUB_VERSION_PATCH 0

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
	!defined(uint8_m)  || \
	!defined(int64_m)  || \
	!defined(int32_m)  || \
	!defined(int16_m)  || \
	!defined(int8_m)

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

	#ifndef int64_m
		#define int64_m int64_t
	#endif

	#ifndef int32_m
		#define int32_m int32_t
	#endif

	#ifndef int16_m
		#define int16_m int16_t
	#endif
	
	#ifndef int8_m
		#define int8_m int8_t
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

#if !defined(PRIu8_m)  || \
	!defined(PRId8_m)  || \
	!defined(PRIu16_m) || \
	!defined(PRId16_m) || \
	!defined(PRIu32_m) || \
	!defined(PRId32_m) || \
	!defined(PRIu64_m) || \
	!defined(PRId64_m)

	#include <inttypes.h>

	#ifndef PRIu8_m
		#define PRIu8_m PRIu8
	#endif
	#ifndef PRId8_m
		#define PRId8_m PRId8
	#endif

	#ifndef PRIu16_m
		#define PRIu16_m PRIu16
	#endif
	#ifndef PRId16_m
		#define PRId16_m PRId16
	#endif

	#ifndef PRIu32_m
		#define PRIu32_m PRIu32
	#endif
	#ifndef PRId32_m
		#define PRId32_m PRId32
	#endif

	#ifndef PRIu64_m
		#define PRIu64_m PRIu64
	#endif
	#ifndef PRId64_m
		#define PRId64_m PRId64
	#endif

#endif

#if !defined(setlocale_m) || \
	!defined(MU_LC_ALL)

	#include <locale.h>
	#ifndef setlocale_m
		#define setlocale_m setlocale
	#endif

	#ifndef MU_LC_ALL
		#define MU_LC_ALL LC_ALL
	#endif
#endif

#ifndef wchar_m
	#include <wchar.h>
	#define wchar_m wchar_t
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
	muByte* bytecode;
	size_m bytecode_len;
	muByte* bytecode_main;
	int32_m main_return;

	muByte bitwidth;
	size_m static_memory_len;
	muByte* static_memory;
	// reg0 is memory for virtually anything
	size_m reg0_len;
	muByte* reg0;
	// reg1 is almost exclusively for holding memory addresses
	size_m reg1_len;
	muByte* reg1;
	// reg2 also
	size_m reg2_len;
	muByte* reg2;
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

MUDEF int mu_context_execute_main(muResult* result, muContext* context);

// binary/bit/byte handling
MUDEF muByte mu_string_to_binary(const char* s);

#ifdef __cplusplus
	}
#endif

#endif /* MUB_H */

#ifdef MUB_IMPLEMENTATION

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

int8_m mu_get_int8_from_bytecode(muByte* bytecode) {
	return bytecode[0];
}

int16_m mu_get_int16_from_bytecode(muByte* bytecode) {
	muByte static_memory_len_bytes[2] = { bytecode[1], bytecode[0] };
	return (*(int16_m*)&static_memory_len_bytes[0]);
}

int32_m mu_get_int32_from_bytecode(muByte* bytecode) {
	muByte static_memory_len_bytes[4] = { bytecode[3], bytecode[2], bytecode[1], bytecode[0] };
	return (*(int32_m*)&static_memory_len_bytes[0]);
}

int64_m mu_get_int64_from_bytecode(muByte* bytecode) {
	muByte static_memory_len_bytes[8] = { bytecode[7], bytecode[6], bytecode[5], bytecode[4], bytecode[3], bytecode[2], bytecode[1], bytecode[0] };
	return (*(int64_m*)&static_memory_len_bytes[0]);
}

muResult mu_bytecode_check_header_validity(muByte* bytecode, size_m bytecode_len) {
	// length check
	if (bytecode_len < MU_BEGINNING_HEADER_LENGTH + MU_END_HEADER_LENGTH) {
		mu_print("[MUB] Failed to create context; bytecode is invalid length.\n");
		return MU_FAILURE;
	}

	/* BEGINNING HEADER INFO */

	// file signature check
	if (
		bytecode[0] != mu_binary(01101101) || 
		bytecode[1] != mu_binary(01110101) || 
		bytecode[2] != mu_binary(01100010) || 
		bytecode[3] != mu_binary(00000000)
	) {
		mu_print("[MUB] Failed to create context; invalid file signature.\n");
		return MU_FAILURE;
	}

	// bitwidth
	if (
		bytecode[4] != 8 && bytecode[4] != 16 &&
		bytecode[4] != 32 && bytecode[4] != 64
	) {
		mu_print("[MUB] Failed to create context; invalid bit width.\n");
		return MU_FAILURE;
	}

	// version checking
	if (
		bytecode[5] != MUB_VERSION_MAJOR || 
		bytecode[6] != MUB_VERSION_MINOR ||
		bytecode[7] != MUB_VERSION_PATCH
	) {
		mu_print("[MUB] Failed to create context; mub version specified in bytecode does not match with the version of this interpreter.\n");
		return MU_FAILURE;
	}

	// end header
	if (
		bytecode[bytecode_len-4] != mu_binary(01100101) ||
		bytecode[bytecode_len-3] != mu_binary(01101110) ||
		bytecode[bytecode_len-2] != mu_binary(01100100) ||
		bytecode[bytecode_len-1] != mu_binary(11111111)
	) {
		mu_print("[MUB] Failed to create context; invalid end header.\n");
		return MU_FAILURE;
	}

	return MU_SUCCESS;
}

// data type
struct mubDataType {
	muBool sign;
	muByte type;
	muByte temp;
	muByte pointer_count;
	uint16_m byte_size;
};
typedef struct mubDataType mubDataType;

#define MUB_DATA_TYPE_UNSIGNED 0
#define MUB_DATA_TYPE_SIGNED 1
#define MUB_DATA_TYPE_VOID 0
#define MUB_DATA_TYPE_STRUCT 1
#define MUB_DATA_TYPE_INTEGER 2
#define MUB_DATA_TYPE_DECIMAL 3

muResult mub_perform_operation(muContext* context, mubDataType src0_dt, mubDataType src1_dt, mubDataType dst_dt, int operation, int64_m reg1_val, int64_m reg2_val);

mubDataType mu_get_data_type_from_bytecode(muByte* bytecode) {
	mubDataType dt = {0};

	dt.sign = mu_bitcheck(bytecode[0], 7);
	dt.type = mu_bitcheck(bytecode[0], 5) + (mu_bitcheck(bytecode[0], 6) * 2);
	dt.temp = mu_bitcheck(bytecode[0], 4);
	dt.pointer_count = mu_bitcheck(bytecode[0], 0) + 
	(mu_bitcheck(bytecode[0], 1) * 2) +
	(mu_bitcheck(bytecode[0], 2) * 4) +
	(mu_bitcheck(bytecode[0], 3) * 8);
	dt.byte_size = mu_get_uint16_from_bytecode(&bytecode[1]);

	return dt;
}

#define MUB_OPERATION_MOVE 0
#define MUB_OPERATION_ADD 1
#define MUB_OPERATION_SUBTRACT 2
#define MUB_OPERATION_MULTIPLY 3
#define MUB_OPERATION_DIVIDE 4

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

int64_m mu_context_get_reg_pointer_signed_value(muByte* reg, size_m reg_len) {
	switch (reg_len) {
	default:
		return 0;
		break;
	case 1:
		return mu_get_int8_from_bytecode(reg);
		break;
	case 2:
		return mu_get_int16_from_bytecode(reg);
		break;
	case 4:
		return mu_get_int32_from_bytecode(reg);
		break;
	case 8:
		return mu_get_int64_from_bytecode(reg);
		break;
	}
}

muResult mu_context_fill_reg0_with_data_type(muContext* context, mubDataType dt, muByte* bytecode) {
	if ((context->reg0 == MU_NULL_PTR) || (context->reg0_len < dt.byte_size)) {
		if (context->reg0 != MU_NULL_PTR) {
			mu_free(context->reg0);
		}
		context->reg0 = mu_malloc(dt.byte_size);
		context->reg0_len = dt.byte_size;
	}
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
				mu_print("[MUB] WARNING! Invalid memory address modification attempt!\n");
				return MU_FAILURE;
			}
			dt.pointer_count--;
		}
	}
	return MU_SUCCESS;
}

muResult mu_context_fill_reg1_with_data_type(muContext* context, mubDataType dt, muByte* bytecode) {
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
				mu_print("[MUB] WARNING! Invalid memory address modification attempt!\n");
				return MU_FAILURE;
			}
			dt.pointer_count--;
		}
	}
	return MU_SUCCESS;
}

muResult mu_context_fill_reg2_with_data_type(muContext* context, mubDataType dt, muByte* bytecode) {
	if ((context->reg2 == MU_NULL_PTR) || (context->reg2_len < dt.byte_size)) {
		if (context->reg2 != MU_NULL_PTR) {
			mu_free(context->reg2);
		}
		context->reg2 = mu_malloc(dt.byte_size);
		context->reg2_len = dt.byte_size;
	}
	if (dt.pointer_count == 0) {
		mu_memcpy(context->reg2, bytecode, dt.byte_size);
	} else {
		if (context->reg2_len < context->bitwidth / 8) {
			context->reg2 = mu_realloc(context->reg2, context->bitwidth / 8);
			context->reg2_len = context->bitwidth / 8;
		}
		mu_memcpy(context->reg2, bytecode, context->bitwidth);
		while (dt.pointer_count > 0) {
			if (dt.pointer_count != 1) {
				mu_memcpy(context->reg2, &context->static_memory[mu_context_get_reg_pointer_value(context->reg2, context->bitwidth / 8)], context->bitwidth / 8);
			} else {
				mu_memcpy(context->reg2, &context->static_memory[mu_context_get_reg_pointer_value(context->reg2, context->bitwidth / 8)], dt.byte_size);
			}
			if (
				dt.pointer_count != 1 && 
				(mu_context_get_reg_pointer_value(context->reg2, context->bitwidth / 8) + context->bitwidth / 8) > context->static_memory_len
			) {
				mu_print("[MUB] WARNING! Invalid memory address modification attempt!\n");
				return MU_FAILURE;
			}
			dt.pointer_count--;
		}
	}
	return MU_SUCCESS;
}

// instructions

muResult mu_instruction_return_main(muContext* context, muByte* bytecode) {
	mubDataType src_dt = mu_get_data_type_from_bytecode(bytecode);
	if (mu_context_fill_reg0_with_data_type(context, src_dt, &bytecode[3]) != MU_SUCCESS) {
		return MU_FAILURE;
	}
	
	int32_m reg0_val = mu_context_get_reg_pointer_signed_value(context->reg0, src_dt.byte_size);
	context->main_return = reg0_val;
	
	return MU_SUCCESS;
}

// @TODO implement spec
muResult mu_instruction_print(muContext* context, muByte* bytecode) {
	size_m offset = 0;
	mubDataType src_dt = mu_get_data_type_from_bytecode(bytecode);
	if (mu_context_fill_reg0_with_data_type(context, src_dt, &bytecode[3]) != MU_SUCCESS) {
		return MU_FAILURE;
	}

	offset = 0;
	offset += 3;
	if (src_dt.pointer_count > 0) {
		offset += context->bitwidth / 8;
	} else {
		offset += src_dt.byte_size;
	}
	muByte spec_byte = bytecode[offset];
	muByte type = (mu_bitcheck(spec_byte, 7) * 2) + mu_bitcheck(spec_byte, 6);

	switch (type) { default: break;
		// print integer / decimal
		case 0: { case 1: {
			switch (src_dt.byte_size) { default: break;
				case 1: {
					switch (src_dt.type) { default: break;
						case MUB_DATA_TYPE_INTEGER: {
							switch (src_dt.sign) { default: break;
								case MUB_DATA_TYPE_UNSIGNED: { printf("%"PRIu8_m"", *(uint8_m*)context->reg0); return MU_SUCCESS; } break;
								case MUB_DATA_TYPE_SIGNED:   { printf("%"PRId8_m"", *(int8_m*) context->reg0); return MU_SUCCESS; } break;
							}
						} break;
					}
				} break;
				case 2: {
					switch (src_dt.type) { default: break;
						case MUB_DATA_TYPE_INTEGER: {
							switch (src_dt.sign) { default: break;
								case MUB_DATA_TYPE_UNSIGNED: { printf("%"PRIu16_m"", *(uint16_m*)context->reg0); return MU_SUCCESS; } break;
								case MUB_DATA_TYPE_SIGNED:   { printf("%"PRId16_m"", *(int16_m*) context->reg0); return MU_SUCCESS; } break;
							}
						} break;
					}
				} break;
				case 4: {
					switch (src_dt.type) { default: break;
						case MUB_DATA_TYPE_INTEGER: {
							switch (src_dt.sign) { default: break;
								case MUB_DATA_TYPE_UNSIGNED: { printf("%"PRIu32_m"", *(uint32_m*)context->reg0); return MU_SUCCESS; } break;
								case MUB_DATA_TYPE_SIGNED:   { printf("%"PRId32_m"", *(int32_m*) context->reg0); return MU_SUCCESS; } break;
							}
						} break;
						case MUB_DATA_TYPE_DECIMAL: {
							switch (src_dt.sign) { default: break;
								case MUB_DATA_TYPE_SIGNED:   { printf("%f", *(float*) context->reg0); return MU_SUCCESS; } break;
							}
						} break;
					}
				} break;
				case 8: {
					switch (src_dt.type) { default: break;
						case MUB_DATA_TYPE_INTEGER: {
							switch (src_dt.sign) { default: break;
								// @TODO kill Microsoft
								#if !defined(_WIN32) && !defined(WIN32)
									case MUB_DATA_TYPE_UNSIGNED: { printf("%"PRIu64_m"", *(uint64_m*)context->reg0); return MU_SUCCESS; } break;
									case MUB_DATA_TYPE_SIGNED:   { printf("%"PRId64_m"", *(int64_m*) context->reg0); return MU_SUCCESS; } break;
								#endif
							}
						} break;
						case MUB_DATA_TYPE_DECIMAL: {
							switch (src_dt.sign) { default: break;
								case MUB_DATA_TYPE_SIGNED:   { printf("%f", *(double*) context->reg0); return MU_SUCCESS; } break;
							}
						} break;
					}
				} break;
			}
		} break; } break;
		// print character
		case 2: {
			if (src_dt.byte_size == 1) {
				char arr[2] = { context->reg0[0], '\0' };
				printf(arr);
			} else {
				// @TODO test if this works
				uint64_m reg0_val = mu_context_get_reg_pointer_value(context->reg0, src_dt.byte_size);
				wchar_m arr[2] = { reg0_val, '\0' };
				setlocale_m(MU_LC_ALL, "");
				printf("%ls", arr);
			}
		} break;
	}

	return MU_SUCCESS;
}

muResult mu_instruction_move(muContext* context, muByte* bytecode) {
	size_m offset = 0;

	// obtain value from src_dt into reg0 (pointer=0 means we have actual value)
	mubDataType src_dt = mu_get_data_type_from_bytecode(bytecode);
	if (mu_context_fill_reg0_with_data_type(context, src_dt, &bytecode[3]) != MU_SUCCESS) {
		return MU_FAILURE;
	}

	// obtain value from dst_dt into reg1 (pointer=0 means we have pointer to set value at)
	offset = 0;
	offset += 3;
	if (src_dt.pointer_count > 0) {
		offset += context->bitwidth / 8;
	} else {
		offset += src_dt.byte_size;
	}
	mubDataType dst_dt = mu_get_data_type_from_bytecode(&bytecode[offset]);
	if (mu_context_fill_reg1_with_data_type(context, dst_dt, &bytecode[offset+3]) != MU_SUCCESS) {
		return MU_FAILURE;
	}

	// set memory address point stored in reg1 to reg0

	uint64_m reg1_val = mu_context_get_reg_pointer_value(context->reg1, context->bitwidth / 8);
	if ((reg1_val + dst_dt.byte_size > context->static_memory_len) || reg1_val == 0) {
		mu_print("[MUB] WARNING! Invalid memory address modification attempt!\n");
		return MU_FAILURE;
	}
	
	if (
		(src_dt.type == MUB_DATA_TYPE_VOID || src_dt.type == MUB_DATA_TYPE_STRUCT) ||
		(dst_dt.type == MUB_DATA_TYPE_VOID || dst_dt.type == MUB_DATA_TYPE_STRUCT)
	) {
		for (size_m i = 0; i < src_dt.byte_size && i < dst_dt.byte_size; i++) {
			context->static_memory[reg1_val+i] = context->reg0[i];
		}
		return MU_SUCCESS;
	}
	
	return mub_perform_operation(context, src_dt, src_dt, dst_dt, MUB_OPERATION_MOVE, reg1_val, reg1_val);
}

muResult mu_instruction_add(muContext* context, muByte* bytecode) {
	size_m offset = 0;

	// obtain value from src0_dt into reg0 (pointer=0 means we have actual value)
	mubDataType src0_dt = mu_get_data_type_from_bytecode(bytecode);
	if (mu_context_fill_reg0_with_data_type(context, src0_dt, &bytecode[3]) != MU_SUCCESS) {
		return MU_FAILURE;
	}

	// obtain value from src1_dt into reg1 (pointer=0 means we have pointer to set value at)
	offset = 0;
	offset += 3;
	if (src0_dt.pointer_count > 0) {
		offset += context->bitwidth / 8;
	} else {
		offset += src0_dt.byte_size;
	}
	mubDataType src1_dt = mu_get_data_type_from_bytecode(&bytecode[offset]);
	if (mu_context_fill_reg1_with_data_type(context, src1_dt, &bytecode[offset+3]) != MU_SUCCESS) {
		return MU_FAILURE;
	}

	// obtain value from dst_dt into reg2
	offset += 3;
	if (src1_dt.pointer_count > 0) {
		offset += context->bitwidth / 8;
	} else {
		offset += src1_dt.byte_size;
	}
	mubDataType dst_dt = mu_get_data_type_from_bytecode(&bytecode[offset]);
	if (mu_context_fill_reg2_with_data_type(context, dst_dt, &bytecode[offset+3]) != MU_SUCCESS) {
		return MU_FAILURE;
	}

	// set memory address point stored in reg2 to reg0 + reg1
	
	uint64_m reg2_val = mu_context_get_reg_pointer_value(context->reg2, context->bitwidth / 8);
	if ((reg2_val + dst_dt.byte_size > context->static_memory_len) || reg2_val == 0) {
		mu_print("[MUB] WARNING! Invalid memory address modification attempt!\n");
		return MU_FAILURE;
	}
	
	if (
		(src0_dt.type == MUB_DATA_TYPE_VOID || src0_dt.type == MUB_DATA_TYPE_STRUCT) ||
		(src1_dt.type == MUB_DATA_TYPE_VOID || src1_dt.type == MUB_DATA_TYPE_STRUCT) ||
		(dst_dt.type == MUB_DATA_TYPE_VOID || dst_dt.type == MUB_DATA_TYPE_STRUCT)
	) {
		mu_print("[MIUC] Invalid mathematical operation.\n");
		return MU_FAILURE;
	}
	
	return mub_perform_operation(context, src0_dt, src1_dt, dst_dt, MUB_OPERATION_ADD, reg2_val, reg2_val);
}

size_m mub_get_step_from_data_type(muContext* context, muByte* bytecode) {
	size_m step = 3;
	mubDataType dt0 = mu_get_data_type_from_bytecode(bytecode);
	if (dt0.pointer_count > 0) {
		step += context->bitwidth / 8;
	} else {
		step += dt0.byte_size;
	}
	return step;
}

muByte* mub_advance_header(muContext* context, muByte* bytecode, muByte* bytecode_beginning, size_m bytecode_len) {
	switch (bytecode[0]) {
		default: return bytecode + 1; break;
		case 0x00: bytecode += 1 + mub_get_step_from_data_type(context, bytecode+1); return bytecode; break;
		case 0x01: bytecode += 1 + mub_get_step_from_data_type(context, bytecode+1) + 1; return bytecode; break;
		// beginning stuff
		case 0x6D: if (bytecode == bytecode_beginning) { return bytecode + 16; } break;
		// end stuff
		case 0x65: if (bytecode + 4 == bytecode_beginning + bytecode_len) { return bytecode + 4; } break;
		// commands
		case 0x80: bytecode += 1 + mub_get_step_from_data_type(context, bytecode+1) + 3 + (context->bitwidth / 8); return bytecode; break;
		case 0x81: bytecode += 1 + mub_get_step_from_data_type(context, bytecode+1);  bytecode += mub_get_step_from_data_type(context, bytecode) + 3 + (context->bitwidth / 8); return bytecode; break;
		// jump markers
		case 0xF0: {
			bytecode++;
			uint64_m val = 1;
			switch (context->bitwidth) {
				case 8:  val = *((uint8_m*) bytecode); bytecode ++;   break;
				case 16: val = *((uint16_m*)bytecode); bytecode += 2; break;
				case 32: val = *((uint32_m*)bytecode); bytecode += 4; break;
				case 64: val = *((uint64_m*)bytecode); bytecode += 8; break;
			}
			if (val == 0) {
				context->bytecode_main = bytecode;
			}
			return bytecode;
		} break;
		case 0xF1: {
			bytecode++;
			return bytecode;
		} break;
	}
	return bytecode + 1;
}

muResult mub_execute_command(muContext* context, muByte* bytecode) {
	switch (bytecode[0]) { default: break;
	case 0x00: return mu_instruction_return_main(context, bytecode+1); break;
	case 0x01: return mu_instruction_print(context, bytecode+1); break;
	case 0x80: return mu_instruction_move(context, bytecode+1); break;
	case 0x81: return mu_instruction_add(context, bytecode+1); break;
	}
	mu_print("[MUB] Unrecognized command when executing.\n");
	return MU_FAILURE;
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

	context.bytecode_main = MU_NULL_PTR;
	context.main_return = 0;

	if (context.static_memory_len > 0) {
		context.static_memory = mu_malloc(context.static_memory_len * sizeof(muByte));
		mu_memset(context.static_memory, 0, context.static_memory_len * sizeof(muByte));
	} else {
		context.static_memory = MU_NULL_PTR;
	}

	context.reg0 = MU_NULL_PTR;
	context.reg1 = MU_NULL_PTR;
	context.reg2 = MU_NULL_PTR;

	context.bytecode = mu_malloc(bytecode_len * sizeof(muByte));
	mu_memcpy(context.bytecode, bytecode, bytecode_len * sizeof(muByte));
	context.bytecode_len = bytecode_len;

	muByte* step = context.bytecode;
	while (step < context.bytecode + context.bytecode_len) {
		step = mub_advance_header(&context, step, context.bytecode, context.bytecode_len);
	}

	if (result != MU_NULL_PTR) {
		*result = MU_SUCCESS;
	}
	return context;
}

MUDEF muContext mu_context_destroy(muResult* result, muContext context) {
	if (context.bytecode != MU_NULL_PTR) {
		mu_free(context.bytecode);
		context.bytecode = MU_NULL_PTR;
	}

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

	if (context.reg2 != MU_NULL_PTR) {
		mu_free(context.reg2);
		context.reg2 = MU_NULL_PTR;
	}

	if (result != MU_NULL_PTR) {
		*result = MU_SUCCESS;
	}

	return context;
}

MUDEF int mu_context_execute_main(muResult* result, muContext* context) {
	if (context->bytecode_main == MU_NULL_PTR) {
		mu_print("[MUB] Failed to execute main; no main specified in the bytecode.\n");
		if (result != MU_NULL_PTR) {
			*result = MU_FAILURE;
		}
		return 0;
	}

	muByte* step = context->bytecode_main;
	while (step < context->bytecode + context->bytecode_len) {
		if (step[0] == 0xF1) {
			break;
		}
		if (mub_execute_command(context, step) != MU_SUCCESS) {
			if (result != MU_NULL_PTR) {
				*result = MU_FAILURE;
			}
			return 0;
		}
		if (step[0] == 0x00) {
			return context->main_return;
		}
		step = mub_advance_header(context, step, context->bytecode, context->bytecode_len);
	}

	if (result != MU_NULL_PTR) {
		*result = MU_SUCCESS;
	}

	return 0;
}

MUDEF muByte mu_string_to_binary(const char* s) {
	muByte i = 0;
	while (*s) {
		i <<= 1;
		i += *s++ - '0';
	}
	return i;
}

muResult mub_perform_operation(muContext* context, mubDataType src0_dt, mubDataType src1_dt, mubDataType dst_dt, int operation, int64_m reg1_val, int64_m reg2_val) {
	switch (operation) {
		default: {
			mu_print("[MUB] Failed to perform operation; unrecognized operation.\n");
			return MU_FAILURE;
			break;
		}
		case MUB_OPERATION_MOVE: {
			switch (src0_dt.byte_size) {default: break;
				case 1: {
					switch (dst_dt.byte_size) { default: break;
						case 1: {
							switch (src0_dt.type) { default: break;
								case MUB_DATA_TYPE_INTEGER: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg1_val]) = (uint8_m)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint8_m*)&context->static_memory[reg1_val]) = (uint8_m)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((int8_m*)&context->static_memory[reg1_val]) = (int8_m)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg1_val]) = (int8_m)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
							}
						} break;
						case 2: {
							switch (src0_dt.type) { default: break;
								case MUB_DATA_TYPE_INTEGER: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg1_val]) = (uint8_m)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint8_m*)&context->static_memory[reg1_val]) = (uint8_m)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((int8_m*)&context->static_memory[reg1_val]) = (int8_m)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg1_val]) = (int8_m)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
							}
						} break;
						case 4: {
							switch (src0_dt.type) { default: break;
								case MUB_DATA_TYPE_INTEGER: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg1_val]) = (uint8_m)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint8_m*)&context->static_memory[reg1_val]) = (uint8_m)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((int8_m*)&context->static_memory[reg1_val]) = (int8_m)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg1_val]) = (int8_m)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint8_m*)&context->static_memory[reg1_val]) = (uint8_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg1_val]) = (int8_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
							}
						} break;
						case 8: {
							switch (src0_dt.type) { default: break;
								case MUB_DATA_TYPE_INTEGER: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg1_val]) = (uint8_m)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint8_m*)&context->static_memory[reg1_val]) = (uint8_m)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((int8_m*)&context->static_memory[reg1_val]) = (int8_m)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg1_val]) = (int8_m)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint8_m*)&context->static_memory[reg1_val]) = (uint8_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg1_val]) = (int8_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
							}
						} break;
					}
				} break;
				case 2: {
					switch (dst_dt.byte_size) { default: break;
						case 1: {
							switch (src0_dt.type) { default: break;
								case MUB_DATA_TYPE_INTEGER: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg1_val]) = (uint16_m)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint16_m*)&context->static_memory[reg1_val]) = (uint16_m)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((int16_m*)&context->static_memory[reg1_val]) = (int16_m)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg1_val]) = (int16_m)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
							}
						} break;
						case 2: {
							switch (src0_dt.type) { default: break;
								case MUB_DATA_TYPE_INTEGER: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg1_val]) = (uint16_m)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint16_m*)&context->static_memory[reg1_val]) = (uint16_m)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((int16_m*)&context->static_memory[reg1_val]) = (int16_m)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg1_val]) = (int16_m)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
							}
						} break;
						case 4: {
							switch (src0_dt.type) { default: break;
								case MUB_DATA_TYPE_INTEGER: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg1_val]) = (uint16_m)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint16_m*)&context->static_memory[reg1_val]) = (uint16_m)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((int16_m*)&context->static_memory[reg1_val]) = (int16_m)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg1_val]) = (int16_m)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint16_m*)&context->static_memory[reg1_val]) = (uint16_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg1_val]) = (int16_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
							}
						} break;
						case 8: {
							switch (src0_dt.type) { default: break;
								case MUB_DATA_TYPE_INTEGER: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg1_val]) = (uint16_m)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint16_m*)&context->static_memory[reg1_val]) = (uint16_m)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((int16_m*)&context->static_memory[reg1_val]) = (int16_m)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg1_val]) = (int16_m)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint16_m*)&context->static_memory[reg1_val]) = (uint16_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg1_val]) = (int16_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
							}
						} break;
					}
				} break;
				case 4: {
					switch (dst_dt.byte_size) { default: break;
						case 1: {
							switch (src0_dt.type) { default: break;
								case MUB_DATA_TYPE_INTEGER: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg1_val]) = (uint32_m)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint32_m*)&context->static_memory[reg1_val]) = (uint32_m)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((int32_m*)&context->static_memory[reg1_val]) = (int32_m)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg1_val]) = (int32_m)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
								case MUB_DATA_TYPE_DECIMAL: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg1_val]) = (float)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg1_val]) = (float)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
							}
						} break;
						case 2: {
							switch (src0_dt.type) { default: break;
								case MUB_DATA_TYPE_INTEGER: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg1_val]) = (uint32_m)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint32_m*)&context->static_memory[reg1_val]) = (uint32_m)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((int32_m*)&context->static_memory[reg1_val]) = (int32_m)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg1_val]) = (int32_m)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
								case MUB_DATA_TYPE_DECIMAL: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg1_val]) = (float)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg1_val]) = (float)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
							}
						} break;
						case 4: {
							switch (src0_dt.type) { default: break;
								case MUB_DATA_TYPE_INTEGER: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg1_val]) = (uint32_m)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint32_m*)&context->static_memory[reg1_val]) = (uint32_m)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((int32_m*)&context->static_memory[reg1_val]) = (int32_m)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg1_val]) = (int32_m)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint32_m*)&context->static_memory[reg1_val]) = (uint32_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg1_val]) = (int32_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
								case MUB_DATA_TYPE_DECIMAL: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg1_val]) = (float)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg1_val]) = (float)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg1_val]) = (float)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
							}
						} break;
						case 8: {
							switch (src0_dt.type) { default: break;
								case MUB_DATA_TYPE_INTEGER: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg1_val]) = (uint32_m)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint32_m*)&context->static_memory[reg1_val]) = (uint32_m)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((int32_m*)&context->static_memory[reg1_val]) = (int32_m)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg1_val]) = (int32_m)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint32_m*)&context->static_memory[reg1_val]) = (uint32_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg1_val]) = (int32_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
								case MUB_DATA_TYPE_DECIMAL: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg1_val]) = (float)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg1_val]) = (float)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg1_val]) = (float)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
							}
						} break;
					}
				} break;
				case 8: {
					switch (dst_dt.byte_size) { default: break;
						case 1: {
							switch (src0_dt.type) { default: break;
								case MUB_DATA_TYPE_INTEGER: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg1_val]) = (uint64_m)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint64_m*)&context->static_memory[reg1_val]) = (uint64_m)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((int64_m*)&context->static_memory[reg1_val]) = (int64_m)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg1_val]) = (int64_m)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
								case MUB_DATA_TYPE_DECIMAL: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg1_val]) = (double)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg1_val]) = (double)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
							}
						} break;
						case 2: {
							switch (src0_dt.type) { default: break;
								case MUB_DATA_TYPE_INTEGER: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg1_val]) = (uint64_m)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint64_m*)&context->static_memory[reg1_val]) = (uint64_m)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((int64_m*)&context->static_memory[reg1_val]) = (int64_m)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg1_val]) = (int64_m)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
								case MUB_DATA_TYPE_DECIMAL: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg1_val]) = (double)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg1_val]) = (double)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
							}
						} break;
						case 4: {
							switch (src0_dt.type) { default: break;
								case MUB_DATA_TYPE_INTEGER: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg1_val]) = (uint64_m)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint64_m*)&context->static_memory[reg1_val]) = (uint64_m)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((int64_m*)&context->static_memory[reg1_val]) = (int64_m)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg1_val]) = (int64_m)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint64_m*)&context->static_memory[reg1_val]) = (uint64_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg1_val]) = (int64_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
								case MUB_DATA_TYPE_DECIMAL: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg1_val]) = (double)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg1_val]) = (double)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg1_val]) = (double)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
							}
						} break;
						case 8: {
							switch (src0_dt.type) { default: break;
								case MUB_DATA_TYPE_INTEGER: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg1_val]) = (uint64_m)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint64_m*)&context->static_memory[reg1_val]) = (uint64_m)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((int64_m*)&context->static_memory[reg1_val]) = (int64_m)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg1_val]) = (int64_m)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((uint64_m*)&context->static_memory[reg1_val]) = (uint64_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg1_val]) = (int64_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
								case MUB_DATA_TYPE_DECIMAL: {
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg1_val]) = (double)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg1_val]) = (double)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg1_val]) = (double)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
									} break;
								} break;
							}
						} break;
					}
				} break;
			}
		} break;
		case MUB_OPERATION_ADD: {
			switch (src0_dt.byte_size) { default: break;
				case 1: {
					switch (src1_dt.byte_size) { default: break;
						case 1: {
							switch (dst_dt.byte_size) { default: break;
								case 1: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 2: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 4: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 8: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
							}
						} break;
						case 2: {
							switch (dst_dt.byte_size) { default: break;
								case 1: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 2: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 4: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 8: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
							}
						} break;
						case 4: {
							switch (dst_dt.byte_size) { default: break;
								case 1: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 2: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 4: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 8: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
							}
						} break;
						case 8: {
							switch (dst_dt.byte_size) { default: break;
								case 1: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 2: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 4: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 8: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int8_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
							}
						} break;
					}
				} break;
				case 2: {
					switch (src1_dt.byte_size) { default: break;
						case 1: {
							switch (dst_dt.byte_size) { default: break;
								case 1: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 2: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 4: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 8: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
							}
						} break;
						case 2: {
							switch (dst_dt.byte_size) { default: break;
								case 1: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 2: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 4: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 8: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
							}
						} break;
						case 4: {
							switch (dst_dt.byte_size) { default: break;
								case 1: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 2: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 4: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint16_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint16_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int16_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int16_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 8: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint16_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint16_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int16_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int16_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
							}
						} break;
						case 8: {
							switch (dst_dt.byte_size) { default: break;
								case 1: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 2: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 4: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 8: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int16_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
							}
						} break;
					}
				} break;
				case 4: {
					switch (src1_dt.byte_size) { default: break;
						case 1: {
							switch (dst_dt.byte_size) { default: break;
								case 1: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((float*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((float*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((float*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((float*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 2: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((float*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((float*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((float*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((float*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 4: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((float*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((float*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((float*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((float*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 8: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((float*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((float*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((float*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((float*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
							}
						} break;
						case 2: {
							switch (dst_dt.byte_size) { default: break;
								case 1: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((float*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((float*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((float*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((float*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 2: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((float*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((float*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((float*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((float*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 4: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((float*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((float*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((float*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((float*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 8: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((float*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((float*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((float*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((float*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
							}
						} break;
						case 4: {
							switch (dst_dt.byte_size) { default: break;
								case 1: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((float*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((float*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((float*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((float*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 2: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((float*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((float*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((float*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((float*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 4: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((float*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((float*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((float*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((float*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 8: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((float*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((float*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((float*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((float*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
							}
						} break;
						case 8: {
							switch (dst_dt.byte_size) { default: break;
								case 1: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((float*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((float*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((float*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((float*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((float*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((float*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 2: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((float*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((float*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((float*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((float*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((float*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((float*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 4: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((float*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((float*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((float*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((float*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((float*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((float*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((float*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 8: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int32_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((float*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((float*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((float*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((float*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((float*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((float*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((float*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
							}
						} break;
					}
				} break;
				case 8: {
					switch (src1_dt.byte_size) { default: break;
						case 1: {
							switch (dst_dt.byte_size) { default: break;
								case 1: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((double*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((double*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((double*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((double*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 2: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((double*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((double*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((double*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((double*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 4: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((double*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((double*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((double*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((double*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 8: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((double*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((double*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((double*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((double*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
							}
						} break;
						case 2: {
							switch (dst_dt.byte_size) { default: break;
								case 1: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((double*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((double*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((double*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((double*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 2: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((double*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((double*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((double*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((double*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 4: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((double*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((double*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((double*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((double*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 8: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((double*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((double*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((double*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((double*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
							}
						} break;
						case 4: {
							switch (dst_dt.byte_size) { default: break;
								case 1: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((double*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((double*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((double*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((double*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((double*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((double*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 2: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((double*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((double*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((double*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((double*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((double*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((double*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 4: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((double*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((double*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((double*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((double*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((double*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((double*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 8: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((double*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((double*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((double*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((double*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((double*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((double*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
							}
						} break;
						case 8: {
							switch (dst_dt.byte_size) { default: break;
								case 1: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((uint64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((uint64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((int64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((int64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((double*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((double*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((double*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((double*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)&context->static_memory[reg2_val]) = (uint8_m)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)&context->static_memory[reg2_val]) = (int8_m)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 2: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((uint64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((uint64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((int64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((int64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((double*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((double*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((double*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((double*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)&context->static_memory[reg2_val]) = (uint16_m)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)&context->static_memory[reg2_val]) = (int16_m)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 4: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((uint64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((uint64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((int64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((int64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((uint64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((int64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((double*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((double*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((double*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((double*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)&context->static_memory[reg2_val]) = (uint32_m)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)&context->static_memory[reg2_val]) = (int32_m)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((float*)&context->static_memory[reg2_val]) = (float)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
								case 8: {
									switch (src0_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((uint64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((uint64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((int64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((int64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_UNSIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((uint64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((int64_m*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src1_dt.type) { default: break;
												case MUB_DATA_TYPE_INTEGER: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((double*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((double*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((double*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((double*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_UNSIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
												case MUB_DATA_TYPE_DECIMAL: {
													switch (dst_dt.type) { default: break;
														case MUB_DATA_TYPE_INTEGER: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)&context->static_memory[reg2_val]) = (uint64_m)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)&context->static_memory[reg2_val]) = (int64_m)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
														case MUB_DATA_TYPE_DECIMAL: {
															switch (src0_dt.sign) { default: break;
																case MUB_DATA_TYPE_SIGNED: {
																	switch (src1_dt.sign) { default: break;
																		case MUB_DATA_TYPE_SIGNED: {
																			switch (dst_dt.sign) { default: break;
																				case MUB_DATA_TYPE_UNSIGNED: { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																				case MUB_DATA_TYPE_SIGNED:   { *((double*)&context->static_memory[reg2_val]) = (double)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
																			}
																		} break;
																	}
																} break;
															}
														} break;
													}
												} break;
											}
										} break;
									}
								} break;
							}
						} break;
					}
				} break;
			}
		} break;
	}

	mu_print("[MUB] Failed to perform operation; invalid conversion request.\n");
	return MU_FAILURE;
}

#ifdef __cplusplus
	}
#endif

#endif /* MUB_IMPLEMENTATION */

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
