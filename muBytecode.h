/*
muBytecode - Muukid
Public domain single-file C library for storing and executing bytecode.
No warranty implied; use at your own risk.

Licensed under MIT License or public domain, whichever you prefer.
More explicit license information at the end of the file.

@TODO implement print spec decimal print accuracy
@TODO do more thorough testing on wide character printing
@TODO "kill" option for multithreaded contexts
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
	#ifdef MUB_NO_PRINT
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
	!defined(mu_memcpy) || \
	!defined(mu_strcmp)

	#include <string.h>

	#ifndef mu_memset
		#define mu_memset memset
	#endif

	#ifndef mu_memcpy
		#define mu_memcpy memcpy
	#endif

	#ifndef mu_strcmp
		#define mu_strcmp strcmp
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

struct muJumpMarker {
	size_m bytecode_index;
	size_m index;
};
typedef struct muJumpMarker muJumpMarker;

struct muFunction {
	size_m bytecode_index;
	size_m index;
	char* name;
};
typedef struct muFunction muFunction;

struct muExternalFunction {
	void (*function)(void* context);
	size_m index;
	char* name;
};
typedef struct muExternalFunction muExternalFunction;

struct mubDataType {
	muBool sign;
	muByte type;
	muByte temp;
	muByte pointer_count;
	uint16_m byte_size;
};
typedef struct mubDataType mubDataType;

struct muVariable {
	size_m address;
	mubDataType datatype;
	char* name;
};
typedef struct muVariable muVariable;

struct muContext {
	muBool alive;
	muByte* bytecode;
	size_m bytecode_len;
	int32_m main_return;

	muResult last_if;

	muFunction* functions;
	size_m function_len;
	size_m function_id_byte_len;

	muExternalFunction* external_functions;
	size_m external_function_len;
	size_m external_function_id_byte_len;

	muJumpMarker* jump_markers;
	size_m jump_marker_len;
	size_m jump_marker_id_byte_len;

	muVariable* variables;
	size_m variable_len;

	muByte bytewidth;

	size_m static_memory_len;
	muByte* static_memory;

	size_m temp_memory_len;
	muByte* temp_memory;

	size_m dynamic_memory_len;
	muByte* dynamic_memory;

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
MUDEF muContext mu_context_create(muResult* result, muByte* bytecode, size_m bytecode_len, muBool fail_if_invalid_bytecode);
MUDEF muContext mu_context_destroy(muResult* result, muContext context);

MUDEF int mu_context_execute_main(muResult* result, muContext* context);
MUDEF void mu_context_execute_function(muResult* result, muContext* context, size_m id);

MUDEF size_m mu_context_get_function_id(muResult* result, muContext* context, const char* name);

MUDEF muVariable mu_context_get_variable(muResult* result, muContext* context, const char* name);
MUDEF void mu_context_get_variable_data(muResult* result, muContext* context, muVariable variable, void* data);
MUDEF void mu_context_set_variable_data(muResult* result, muContext* context, muVariable variable, void* data);

MUDEF void mu_context_set_external_function(muResult* result, muContext* context, const char* name, void(*function)(muContext* context));

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

#ifdef MUB_QUICK_COMPILE
	#ifndef MUB_SHORT_OPERATION_LIST
		#define MUB_SHORT_OPERATION_LIST
	#endif
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

	// version checking
	if (
		bytecode[5] != MUB_VERSION_MAJOR || 
		bytecode[6] != MUB_VERSION_MINOR ||
		bytecode[7] != MUB_VERSION_PATCH
	) {
		mu_print("[MUB] WARNING! mub version specified in bytecode does not match with the version of this interpreter. Bytecode may not execute properly.\n");
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

#define MUB_OPERATION_MOVE                0

#define MUB_OPERATION_ADD                 1
#define MUB_OPERATION_SUBTRACT            2
#define MUB_OPERATION_MULTIPLY            3
#define MUB_OPERATION_DIVIDE              4
#define MUB_OPERATION_MODULO              5

#define MUB_OPERATION_BW_NOT              6
#define MUB_OPERATION_BW_AND              7
#define MUB_OPERATION_BW_OR               8
#define MUB_OPERATION_BW_XOR              9
#define MUB_OPERATION_BW_LSHIFT          10
#define MUB_OPERATION_BW_RSHIFT          11

#define MUB_OPERATION_COND_EQUAL         12
#define MUB_OPERATION_COND_NOT_EQUAL     13
#define MUB_OPERATION_COND_LESS          14
#define MUB_OPERATION_COND_LESS_EQUAL    15
#define MUB_OPERATION_COND_GREATER       16
#define MUB_OPERATION_COND_GREATER_EQUAL 17

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

muByte* mu_context_get_memory_location(muContext* context, size_m index, int buffer) {
	if (buffer == 0) {
		if (index < context->static_memory_len) {
			return &context->static_memory[index];
		} else {
			index -= context->static_memory_len;
			if (index < context->dynamic_memory_len) {
				return &context->dynamic_memory[index];
			}
			return MU_NULL_PTR;
		}
	}
	if (index < context->temp_memory_len) {
		return &context->temp_memory[index];
	}
	return MU_NULL_PTR;
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
		if (context->reg0_len < context->bytewidth) {
			context->reg0 = mu_realloc(context->reg0, context->bytewidth);
			context->reg0_len = context->bytewidth;
		}
		mu_memcpy(context->reg0, bytecode, context->bytewidth);
		while (dt.pointer_count > 0) {
			if (dt.pointer_count != 1) {
				muByte* mem_loc = mu_context_get_memory_location(context, mu_context_get_reg_pointer_value(context->reg0, context->bytewidth), dt.temp);
				if (mem_loc != MU_NULL_PTR) {
					mu_memcpy(context->reg0, mem_loc, context->bytewidth);
				} else {
					mu_print("[MUB] WARNING! Invalid memory address modification attempt!\n");
					return MU_FAILURE;
				}
			} else {
				muByte* mem_loc = mu_context_get_memory_location(context, mu_context_get_reg_pointer_value(context->reg0, context->bytewidth), dt.temp);
				if (mem_loc != MU_NULL_PTR) {
					mu_memcpy(context->reg0, mem_loc, dt.byte_size);
				} else {
					mu_print("[MUB] WARNING! Invalid memory address modification attempt!\n");
					return MU_FAILURE;
				}
			}
			if (
				dt.pointer_count != 1 && 
				(mu_context_get_reg_pointer_value(context->reg0, context->bytewidth) + context->bytewidth) > context->static_memory_len + context->dynamic_memory_len
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
		if (context->reg1_len < context->bytewidth) {
			context->reg1 = mu_realloc(context->reg1, context->bytewidth);
			context->reg1_len = context->bytewidth;
		}
		mu_memcpy(context->reg1, bytecode, context->bytewidth);
		while (dt.pointer_count > 0) {
			if (dt.pointer_count != 1) {
				muByte* mem_loc = mu_context_get_memory_location(context, mu_context_get_reg_pointer_value(context->reg1, context->bytewidth), dt.temp);
				if (mem_loc != MU_NULL_PTR) {
					mu_memcpy(context->reg1, mem_loc, context->bytewidth);
				} else {
					mu_print("[MUB] WARNING! Invalid memory address modification attempt!\n");
					return MU_FAILURE;
				}
			} else {
				muByte* mem_loc = mu_context_get_memory_location(context, mu_context_get_reg_pointer_value(context->reg1, context->bytewidth), dt.temp);
				if (mem_loc != MU_NULL_PTR) {
					mu_memcpy(context->reg1, mem_loc, dt.byte_size);
				} else {
					mu_print("[MUB] WARNING! Invalid memory address modification attempt!\n");
					return MU_FAILURE;
				}
			}
			if (
				dt.pointer_count != 1 && 
				(mu_context_get_reg_pointer_value(context->reg1, context->bytewidth) + context->bytewidth) > context->static_memory_len + context->dynamic_memory_len
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
		if (context->reg2_len < context->bytewidth) {
			context->reg2 = mu_realloc(context->reg2, context->bytewidth);
			context->reg2_len = context->bytewidth;
		}
		mu_memcpy(context->reg2, bytecode, context->bytewidth);
		while (dt.pointer_count > 0) {
			if (dt.pointer_count != 1) {
				muByte* mem_loc = mu_context_get_memory_location(context, mu_context_get_reg_pointer_value(context->reg2, context->bytewidth), dt.temp);
				if (mem_loc != MU_NULL_PTR) {
					mu_memcpy(context->reg2, mem_loc, context->bytewidth);
				} else {
					mu_print("[MUB] WARNING! Invalid memory address modification attempt!\n");
					return MU_FAILURE;
				}
			} else {
				muByte* mem_loc = mu_context_get_memory_location(context, mu_context_get_reg_pointer_value(context->reg2, context->bytewidth), dt.temp);
				if (mem_loc != MU_NULL_PTR) {
					mu_memcpy(context->reg2, mem_loc, dt.byte_size);
				} else {
					mu_print("[MUB] WARNING! Invalid memory address modification attempt!\n");
					return MU_FAILURE;
				}
			}
			if (
				dt.pointer_count != 1 && 
				(mu_context_get_reg_pointer_value(context->reg2, context->bytewidth) + context->bytewidth) > context->static_memory_len + context->dynamic_memory_len
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

muResult mu_instruction_print(muContext* context, muByte* bytecode) {
	size_m offset = 0;
	mubDataType src_dt = mu_get_data_type_from_bytecode(bytecode);
	if (mu_context_fill_reg0_with_data_type(context, src_dt, &bytecode[3]) != MU_SUCCESS) {
		return MU_FAILURE;
	}

	offset = 0;
	offset += 3;
	if (src_dt.pointer_count > 0) {
		offset += context->bytewidth;
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
								case MUB_DATA_TYPE_UNSIGNED: { mu_printf("%"PRIu8_m"", *(uint8_m*)context->reg0); return MU_SUCCESS; } break;
								case MUB_DATA_TYPE_SIGNED:   { mu_printf("%"PRId8_m"", *(int8_m*) context->reg0); return MU_SUCCESS; } break;
							}
						} break;
					}
				} break;
				case 2: {
					switch (src_dt.type) { default: break;
						case MUB_DATA_TYPE_INTEGER: {
							switch (src_dt.sign) { default: break;
								case MUB_DATA_TYPE_UNSIGNED: { mu_printf("%"PRIu16_m"", *(uint16_m*)context->reg0); return MU_SUCCESS; } break;
								case MUB_DATA_TYPE_SIGNED:   { mu_printf("%"PRId16_m"", *(int16_m*) context->reg0); return MU_SUCCESS; } break;
							}
						} break;
					}
				} break;
				case 4: {
					switch (src_dt.type) { default: break;
						case MUB_DATA_TYPE_INTEGER: {
							switch (src_dt.sign) { default: break;
								case MUB_DATA_TYPE_UNSIGNED: { mu_printf("%"PRIu32_m"", *(uint32_m*)context->reg0); return MU_SUCCESS; } break;
								case MUB_DATA_TYPE_SIGNED:   { mu_printf("%"PRId32_m"", *(int32_m*) context->reg0); return MU_SUCCESS; } break;
							}
						} break;
						case MUB_DATA_TYPE_DECIMAL: {
							switch (src_dt.sign) { default: break;
								case MUB_DATA_TYPE_SIGNED:   { mu_printf("%f", *(float*) context->reg0); return MU_SUCCESS; } break;
							}
						} break;
					}
				} break;
				case 8: {
					switch (src_dt.type) { default: break;
						case MUB_DATA_TYPE_INTEGER: {
							switch (src_dt.sign) { default: break;
								case MUB_DATA_TYPE_UNSIGNED: { mu_printf("%"PRIu64_m"", *(uint64_m*)context->reg0); return MU_SUCCESS; } break;
								case MUB_DATA_TYPE_SIGNED:   { mu_printf("%"PRId64_m"", *(int64_m*) context->reg0); return MU_SUCCESS; } break;
							}
						} break;
						case MUB_DATA_TYPE_DECIMAL: {
							switch (src_dt.sign) { default: break;
								case MUB_DATA_TYPE_SIGNED:   { mu_printf("%f", *(double*) context->reg0); return MU_SUCCESS; } break;
							}
						} break;
					}
				} break;
			}
		} break; } break;
		// print character
		case 2: {
			if (src_dt.byte_size == 1) {
				if (context->reg0[0] != '%') {
					char arr[2] = { context->reg0[0], '\0' };
					mu_printf("%s", arr);
				} else {
					char arr[3] = { context->reg0[0], context->reg0[0], '\0' };
					mu_printf("%s", arr);
				}
			} else {
				uint64_m reg0_val = mu_context_get_reg_pointer_value(context->reg0, src_dt.byte_size);
				wchar_m arr[2] = { reg0_val, '\0' };
				setlocale_m(MU_LC_ALL, "");
				mu_printf("%ls", arr);
			}
			return MU_SUCCESS;
		} break;
	}

	mu_print("[MUB] Failed to execute print command; unrecognized type.\n");

	return MU_FAILURE;
}

muResult mu_instruction_set_dynamic_buffer_size(muContext* context, muByte* bytecode) {
	mubDataType src_dt = mu_get_data_type_from_bytecode(bytecode);
	if (mu_context_fill_reg0_with_data_type(context, src_dt, &bytecode[3]) != MU_SUCCESS) {
		return MU_FAILURE;
	}

	if (src_dt.type != MUB_DATA_TYPE_INTEGER) {
		mu_print("[MUB] Failed to set dynamic buffer size; type passed in for new size was non-integer.\n");
		return MU_FAILURE;
	}
	if (src_dt.sign == MUB_DATA_TYPE_SIGNED) {
		mu_print("[MUB] Failed to set dynamic buffer size; sign of data type for new size is signed.\n");
		return MU_FAILURE;
	}

	uint64_m reg0_val = mu_context_get_reg_pointer_value(context->reg0, src_dt.byte_size);

	if (reg0_val == 0) {
		if (context->dynamic_memory != MU_NULL_PTR) {
			mu_free(context->dynamic_memory);
			context->dynamic_memory = MU_NULL_PTR;
			context->dynamic_memory_len = 0;
		}
	} else {
		if (context->dynamic_memory == MU_NULL_PTR) {
			context->dynamic_memory = mu_malloc(reg0_val);
			if (context->dynamic_memory == MU_NULL_PTR) {
				mu_print("[MUB] Failed to set dynamic buffer size; malloc call failed.\n");
				context->dynamic_memory_len = 0;
				return MU_FAILURE;
			}
			mu_memset(context->dynamic_memory, 0, reg0_val);
			context->dynamic_memory_len = reg0_val;
		} else {
			context->dynamic_memory = mu_realloc(context->dynamic_memory, reg0_val);
			if (reg0_val > context->dynamic_memory_len) {
				mu_memset(context->dynamic_memory + context->dynamic_memory_len, 0, reg0_val - context->dynamic_memory_len);
			}
			context->dynamic_memory_len = reg0_val;
		}
	}

	return MU_SUCCESS;
}

muResult mu_two_operand_instruction(muContext* context, muByte* bytecode, int operator) {
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
		offset += context->bytewidth;
	} else {
		offset += src_dt.byte_size;
	}
	mubDataType dst_dt = mu_get_data_type_from_bytecode(&bytecode[offset]);
	size_m orig_byte_size = dst_dt.byte_size;
	dst_dt.byte_size = context->bytewidth;
	if (mu_context_fill_reg1_with_data_type(context, dst_dt, &bytecode[offset+3]) != MU_SUCCESS) {
		return MU_FAILURE;
	}
	dst_dt.byte_size = orig_byte_size;

	// set memory address point stored in reg1 to reg0

	uint64_m reg1_val = mu_context_get_reg_pointer_value(context->reg1, context->bytewidth);
	if ((reg1_val + dst_dt.byte_size > context->static_memory_len + context->dynamic_memory_len) || reg1_val == 0) {
		mu_print("[MUB] WARNING! Invalid memory address modification attempt!\n");
		return MU_FAILURE;
	}
	
	if (
		(src_dt.type == MUB_DATA_TYPE_VOID || src_dt.type == MUB_DATA_TYPE_STRUCT) ||
		(dst_dt.type == MUB_DATA_TYPE_VOID || dst_dt.type == MUB_DATA_TYPE_STRUCT)
	) {
		for (size_m i = 0; i < src_dt.byte_size && i < dst_dt.byte_size; i++) {
			muByte* mem_loc = mu_context_get_memory_location(context, reg1_val+i, dst_dt.temp);
			if (mem_loc != MU_NULL_PTR) {
				*mem_loc = context->reg0[i];
			} else {
				mu_print("[MUB] WARNING! Invalid memory address modification attempt!\n");
				return MU_FAILURE;
			}
		}
		return MU_SUCCESS;
	}
	
	return mub_perform_operation(context, src_dt, src_dt, dst_dt, operator, reg1_val, reg1_val);
}

muResult mu_three_operand_instruction(muContext* context, muByte* bytecode, int operator) {
	size_m offset = 3;

	// obtain value from src0_dt into reg0 (pointer=0 means we have actual value)
	mubDataType src0_dt = mu_get_data_type_from_bytecode(bytecode);
	if (mu_context_fill_reg0_with_data_type(context, src0_dt, &bytecode[offset]) != MU_SUCCESS) {
		return MU_FAILURE;
	}

	// obtain value from src1_dt into reg1 (pointer=0 means we have pointer to set value at)
	if (src0_dt.pointer_count > 0) {
		offset += context->bytewidth;
	} else {
		offset += src0_dt.byte_size;
	}
	mubDataType src1_dt = src0_dt;
	if (mu_context_fill_reg1_with_data_type(context, src1_dt, &bytecode[offset]) != MU_SUCCESS) {
		return MU_FAILURE;
	}

	// obtain value from dst_dt into reg2
	if (src1_dt.pointer_count > 0) {
		offset += context->bytewidth;
	} else {
		offset += src1_dt.byte_size;
	}
	mubDataType dst_dt = mu_get_data_type_from_bytecode(&bytecode[offset]);
	offset += 3;
	size_m orig_byte_size = dst_dt.byte_size;
	dst_dt.byte_size = context->bytewidth;
	if (mu_context_fill_reg2_with_data_type(context, dst_dt, &bytecode[offset]) != MU_SUCCESS) {
		return MU_FAILURE;
	}
	dst_dt.byte_size = orig_byte_size;

	// set memory address point stored in reg2 to reg0 + reg1
	
	uint64_m reg2_val = mu_context_get_reg_pointer_value(context->reg2, context->bytewidth);
	if ((reg2_val + dst_dt.byte_size > context->static_memory_len + context->dynamic_memory_len) || reg2_val == 0) {
		mu_print("[MUB] WARNING! Invalid memory address modification attempt!\n");
		return MU_FAILURE;
	}
	
	if (
		(src0_dt.type == MUB_DATA_TYPE_VOID || src0_dt.type == MUB_DATA_TYPE_STRUCT) ||
		(src1_dt.type == MUB_DATA_TYPE_VOID || src1_dt.type == MUB_DATA_TYPE_STRUCT) ||
		(dst_dt.type == MUB_DATA_TYPE_VOID || dst_dt.type == MUB_DATA_TYPE_STRUCT)
	) {
		mu_print("[MUB] Invalid mathematical operation.\n");
		return MU_FAILURE;
	}
	
	return mub_perform_operation(context, src0_dt, src1_dt, dst_dt, operator, reg2_val, reg2_val);
}

muResult mu_instruction_move(muContext* context, muByte* bytecode) {
	return mu_two_operand_instruction(context, bytecode, MUB_OPERATION_MOVE);
}

muResult mu_instruction_add(muContext* context, muByte* bytecode) {
	return mu_three_operand_instruction(context, bytecode, MUB_OPERATION_ADD);
}

muResult mu_instruction_subtract(muContext* context, muByte* bytecode) {
	return mu_three_operand_instruction(context, bytecode, MUB_OPERATION_SUBTRACT);
}

muResult mu_instruction_multiply(muContext* context, muByte* bytecode) {
	return mu_three_operand_instruction(context, bytecode, MUB_OPERATION_MULTIPLY);
}

muResult mu_instruction_divide(muContext* context, muByte* bytecode) {
	return mu_three_operand_instruction(context, bytecode, MUB_OPERATION_DIVIDE);
}

muResult mu_instruction_modulo(muContext* context, muByte* bytecode) {
	return mu_three_operand_instruction(context, bytecode, MUB_OPERATION_MODULO);
}

muResult mu_instruction_bw_not(muContext* context, muByte* bytecode) {
	return mu_two_operand_instruction(context, bytecode, MUB_OPERATION_BW_NOT);
}

muResult mu_instruction_bw_and(muContext* context, muByte* bytecode) {
	return mu_three_operand_instruction(context, bytecode, MUB_OPERATION_BW_AND);
}

muResult mu_instruction_bw_or(muContext* context, muByte* bytecode) {
	return mu_three_operand_instruction(context, bytecode, MUB_OPERATION_BW_OR);
}

muResult mu_instruction_bw_xor(muContext* context, muByte* bytecode) {
	return mu_three_operand_instruction(context, bytecode, MUB_OPERATION_BW_XOR);
}

muResult mu_instruction_bw_lshift(muContext* context, muByte* bytecode) {
	return mu_three_operand_instruction(context, bytecode, MUB_OPERATION_BW_LSHIFT);
}

muResult mu_instruction_bw_rshift(muContext* context, muByte* bytecode) {
	return mu_three_operand_instruction(context, bytecode, MUB_OPERATION_BW_RSHIFT);
}

muResult mu_instruction_cond_equal(muContext* context, muByte* bytecode) {
	return mu_three_operand_instruction(context, bytecode, MUB_OPERATION_COND_EQUAL);
}

muResult mu_instruction_cond_not_equal(muContext* context, muByte* bytecode) {
	return mu_three_operand_instruction(context, bytecode, MUB_OPERATION_COND_NOT_EQUAL);
}

muResult mu_instruction_cond_less(muContext* context, muByte* bytecode) {
	return mu_three_operand_instruction(context, bytecode, MUB_OPERATION_COND_LESS);
}

muResult mu_instruction_cond_less_equal(muContext* context, muByte* bytecode) {
	return mu_three_operand_instruction(context, bytecode, MUB_OPERATION_COND_LESS_EQUAL);
}

muResult mu_instruction_cond_greater(muContext* context, muByte* bytecode) {
	return mu_three_operand_instruction(context, bytecode, MUB_OPERATION_COND_GREATER);
}

muResult mu_instruction_cond_greater_equal(muContext* context, muByte* bytecode) {
	return mu_three_operand_instruction(context, bytecode, MUB_OPERATION_COND_GREATER_EQUAL);
}

muResult mu_instruction_if(muContext* context, muByte* bytecode) {
	mubDataType src_dt = mu_get_data_type_from_bytecode(bytecode);
	if (mu_context_fill_reg0_with_data_type(context, src_dt, &bytecode[3]) != MU_SUCCESS) {
		return MU_FAILURE;
	}

	context->last_if = MU_FALSE;
	if (src_dt.pointer_count == 0) {
		for (size_m i = 0; i < src_dt.byte_size; i++) {
			if (context->reg0[i] != 0) {
				context->last_if = MU_TRUE;
			}
		}
	} else {
		for (size_m i = 0; i < context->bytewidth; i++) {
			if (context->reg0[i] != 0) {
				context->last_if = MU_TRUE;
			}
		}
	}

	return MU_SUCCESS;
}

muResult mub_execute_function(muContext* context, muByte* bytecode);

muResult mu_instruction_call_function(muContext* context, muByte* bytecode) {
	mubDataType src_dt = mu_get_data_type_from_bytecode(bytecode);
	if (mu_context_fill_reg0_with_data_type(context, src_dt, &bytecode[3]) != MU_SUCCESS) {
		return MU_FAILURE;
	}

	uint64_m reg0_val = mu_context_get_reg_pointer_value(context->reg0, src_dt.byte_size);
	if (reg0_val < context->function_len && context->functions[reg0_val-1].index == reg0_val) {
		return mub_execute_function(context, &context->bytecode[context->functions[reg0_val-1].bytecode_index]);
	}
	for (size_m i = 0; i < context->function_len; i++) {
		if (context->functions[i].index == reg0_val) {
			return mub_execute_function(context, &context->bytecode[context->functions[i].bytecode_index]);
		}
	}
	mu_print("[MUB] WARNING! Unrecognized function ID when executing bytecode. Bytecode may not execute properly.\n");
	return MU_FAILURE;
}

muResult mu_instruction_call_external_function(muContext* context, muByte* bytecode) {
	mubDataType src_dt = mu_get_data_type_from_bytecode(bytecode);
	if (mu_context_fill_reg0_with_data_type(context, src_dt, &bytecode[3]) != MU_SUCCESS) {
		return MU_FAILURE;
	}

	uint64_m reg0_val = mu_context_get_reg_pointer_value(context->reg0, src_dt.byte_size);
	size_m index = 0;
	muBool found_index = MU_FALSE;
	if (reg0_val < context->external_function_len && context->external_functions[reg0_val].index == reg0_val) {
		index = reg0_val;
		found_index = MU_TRUE;
	} else {
		for (size_m i = 0; i < context->external_function_len; i++) {
			if (context->external_functions[i].index == reg0_val) {
				index = i;
				found_index = MU_TRUE;
				break;
			}
		}
	}
	if (found_index == MU_FALSE) {
		mu_print("[MUB] Failed to call external function; could not find external function with specified index.\n");
		return MU_FAILURE;
	}

	if (context->external_functions[index].function == MU_NULL_PTR) {
		mu_print("[MUB] Failed to call external function; specified external function has no function associated with it.\n");
		return MU_FAILURE;
	}
	context->external_functions[index].function(context);
	return MU_SUCCESS;
}

size_m mub_get_step_from_data_type(muContext* context, muByte* bytecode) {
	size_m step = 3;
	mubDataType dt0 = mu_get_data_type_from_bytecode(bytecode);
	if (dt0.pointer_count > 0) {
		step += context->bytewidth;
	} else {
		step += dt0.byte_size;
	}
	return step;
}

muByte* mub_advance_header(muResult* res, muContext* context, muByte* bytecode, muByte* bytecode_beginning, size_m bytecode_len, muBool executing) {
	if (res != MU_NULL_PTR) {
		*res = MU_SUCCESS;
	}
	switch (bytecode[0]) {
		case 0x00: bytecode += 1 + mub_get_step_from_data_type(context, bytecode+1); return bytecode; break;
		case 0x01: bytecode += 1 + mub_get_step_from_data_type(context, bytecode+1) + 1; return bytecode; break;
		case 0x02: bytecode += 1 + mub_get_step_from_data_type(context, bytecode+1); return bytecode; break;
		// name setters
		case 0x10: bytecode++; bytecode += context->function_id_byte_len; while (bytecode[0] != '\0' && bytecode < bytecode_beginning + bytecode_len) { bytecode++; } bytecode++; return bytecode; break;
		case 0x11: bytecode++; bytecode += context->bytewidth; bytecode += 3; while (bytecode[0] != '\0' && bytecode < bytecode_beginning + bytecode_len) { bytecode++; } bytecode++; return bytecode; break;
		case 0x12: bytecode++; bytecode += context->external_function_id_byte_len; while (bytecode[0] != '\0' && bytecode < bytecode_beginning + bytecode_len) { bytecode++; } bytecode++; return bytecode; break;
		// beginning stuff
		case 0x6D: if (bytecode == bytecode_beginning) { return bytecode + 16; } break;
		// end stuff
		case 0x65: if (bytecode + 4 == bytecode_beginning + bytecode_len) { return bytecode + 4; } break;
		// commands
		case 0x80: case 0x86: bytecode += 1 + mub_get_step_from_data_type(context, bytecode+1) + 3 + (context->bytewidth); return bytecode; break;
		case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: case 0x88: case 0x89: case 0x8A: case 0x8B:
		case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: {
			bytecode += 1 + 3 + ((mub_get_step_from_data_type(context, bytecode+1) - 3) * 2);
			bytecode += 3 + context->bytewidth; 
			return bytecode;
		} break;
		// functions
		case 0xE0: bytecode++; bytecode += context->function_id_byte_len; return bytecode; break;
		case 0xE1: bytecode++; return bytecode; break;
		case 0xE2: bytecode++; return bytecode; break;
		case 0xE3: case 0xE4: bytecode += 1 + mub_get_step_from_data_type(context, bytecode+1); return bytecode; break;
		// jump markers
		case 0xF0: bytecode++; bytecode += context->jump_marker_id_byte_len; return bytecode; break;
		case 0xF1: {
			bytecode++;
			uint64_m val = 0;
			switch (context->jump_marker_id_byte_len) { default: break;
				case 1: val = mu_get_uint8_from_bytecode(bytecode); bytecode ++;   break;
				case 2: val = mu_get_uint16_from_bytecode(bytecode); bytecode += 2; break;
				case 4: val = mu_get_uint32_from_bytecode(bytecode); bytecode += 4; break;
				case 8: val = mu_get_uint64_from_bytecode(bytecode); bytecode += 8; break;
			}
			if (!executing) {
				return bytecode;
			}
			if (val < context->jump_marker_len && context->jump_markers[val].index == val) {
				return &context->bytecode[context->jump_markers[val].bytecode_index];
			}
			for (size_m i = 0; i < context->jump_marker_len; i++) {
				if (context->jump_markers[i].index == val) {
					return &context->bytecode[context->jump_markers[i].bytecode_index];
				}
			}
			return bytecode;
		}
		// other stuff
		case 0xA0: bytecode += 1 + mub_get_step_from_data_type(context, bytecode+1); return bytecode; break;
		case 0xA1: bytecode++; return bytecode; break;
		case 0xA2: bytecode++; return bytecode; break;
	}
	if (res != MU_NULL_PTR) {
		*res = MU_FAILURE;
	}
	mu_print("[MUB] WARNING! Unrecognized command found when scanning bytecode. Bytecode may not execute properly.\n");
	return bytecode + 1;
}

muResult mub_execute_command(muContext* context, muByte* bytecode);

muResult mub_execute_function(muContext* context, muByte* bytecode) {
	muByte* step = bytecode;
	while (step < context->bytecode + context->bytecode_len) {
		if (step[0] == 0xE1 || step[0] == 0xE2) {
			return MU_SUCCESS;
		}
		muBool ifCheck = MU_FALSE;
		if (step[0] == 0xA0) {
			ifCheck = MU_TRUE;
		}
		if (mub_execute_command(context, step) != MU_SUCCESS) {
			return MU_FAILURE;
		}
		if ((ifCheck && context->last_if == MU_FALSE) || step[0] == 0xA2) {
			size_m if_count = 1;
			while (if_count != 0 && step < context->bytecode + context->bytecode_len) {
				step = mub_advance_header(MU_NULL_PTR, context, step, context->bytecode, context->bytecode_len, MU_FALSE);
				if (step[0] == 0xA0 || step[0] == 0xA2) {
					if_count++;
				} else if (step[0] == 0xA1) {
					if_count--;
				}
			}
			if (step[1] == 0xA2) {
				step += 2;
			}
		} else {
			step = mub_advance_header(MU_NULL_PTR, context, step, context->bytecode, context->bytecode_len, MU_TRUE);
		}
	}
	mu_print("[MUB] Failed to execute function; no end to the function was found.\n");
	return MU_FAILURE;
}

muResult mub_execute_command(muContext* context, muByte* bytecode) {
	switch (bytecode[0]) { default: break;
	case 0x00: return mu_instruction_return_main(context, bytecode+1); break;
	case 0x01: return mu_instruction_print(context, bytecode+1); break;
	case 0x02: return mu_instruction_set_dynamic_buffer_size(context, bytecode+1); break;
	case 0x10: return MU_SUCCESS; break;
	case 0x11: return MU_SUCCESS; break;
	case 0x12: return MU_SUCCESS; break;
	case 0x80: return mu_instruction_move(context, bytecode+1); break;
	case 0x81: return mu_instruction_add(context, bytecode+1); break;
	case 0x82: return mu_instruction_subtract(context, bytecode+1); break;
	case 0x83: return mu_instruction_multiply(context, bytecode+1); break;
	case 0x84: return mu_instruction_divide(context, bytecode+1); break;
	case 0x85: return mu_instruction_modulo(context, bytecode+1); break;
	case 0x86: return mu_instruction_bw_not(context, bytecode+1); break;
	case 0x87: return mu_instruction_bw_and(context, bytecode+1); break;
	case 0x88: return mu_instruction_bw_or(context, bytecode+1); break;
	case 0x89: return mu_instruction_bw_xor(context, bytecode+1); break;
	case 0x8A: return mu_instruction_bw_lshift(context, bytecode+1); break;
	case 0x8B: return mu_instruction_bw_rshift(context, bytecode+1); break;
	case 0x90: return mu_instruction_cond_equal(context, bytecode+1); break;
	case 0x91: return mu_instruction_cond_not_equal(context, bytecode+1); break;
	case 0x92: return mu_instruction_cond_less(context, bytecode+1); break;
	case 0x93: return mu_instruction_cond_less_equal(context, bytecode+1); break;
	case 0x94: return mu_instruction_cond_greater(context, bytecode+1); break;
	case 0x95: return mu_instruction_cond_greater_equal(context, bytecode+1); break;
	case 0xA0: return mu_instruction_if(context, bytecode+1); break;
	case 0xA1: return MU_SUCCESS; break;
	case 0xA2: return MU_SUCCESS; break;
	case 0xF0: return MU_SUCCESS; break;
	case 0xF1: return MU_SUCCESS; break;
	case 0xE0: return MU_SUCCESS; break;
	case 0xE1: return MU_SUCCESS; break;
	case 0xE2: return MU_SUCCESS; break;
	case 0xE3: return mu_instruction_call_function(context, bytecode+1); break;
	case 0xE4: return mu_instruction_call_external_function(context, bytecode+1); break;
	}
	mu_print("[MUB] WARNING! Unrecognized command when executing. Bytecode may not execute properly.\n");
	return MU_FAILURE;
}

/* functions */

MUDEF muContext mu_context_create(muResult* result, muByte* bytecode, size_m bytecode_len, muBool fail_if_invalid_bytecode) {
    muContext context = {0};
    context.alive = MU_FALSE;
	
	if (mu_bytecode_check_header_validity(bytecode, bytecode_len) != MU_SUCCESS) {
		if (result != MU_NULL_PTR) {
			*result = MU_FAILURE;
		}
		return context;
	}

	unsigned char quick_bit_len = 
	(mu_bitcheck(bytecode[4], 6) * 1) + 
	(mu_bitcheck(bytecode[4], 7) * 2);
	context.bytewidth = 1;
	while (quick_bit_len > 0) {
		context.bytewidth *= 2;
		quick_bit_len--;
	}

	unsigned char quick_id_len = 
	(mu_bitcheck(bytecode[4], 4) * 1) + 
	(mu_bitcheck(bytecode[4], 5) * 2);
	context.jump_marker_id_byte_len = 1;
	while (quick_id_len > 0) {
		context.jump_marker_id_byte_len *= 2;
		quick_id_len--;
	}

	quick_id_len = 
	(mu_bitcheck(bytecode[4], 2) * 1) + 
	(mu_bitcheck(bytecode[4], 3) * 2);
	context.function_id_byte_len = 1;
	while (quick_id_len > 0) {
		context.function_id_byte_len *= 2;
		quick_id_len--;
	}

	quick_id_len = 
	(mu_bitcheck(bytecode[4], 0) * 1) + 
	(mu_bitcheck(bytecode[4], 1) * 2);
	context.external_function_id_byte_len = 1;
	while (quick_id_len > 0) {
		context.external_function_id_byte_len *= 2;
		quick_id_len--;
	}

	context.static_memory_len = mu_get_uint32_from_bytecode(&bytecode[8]);
	context.temp_memory_len = mu_get_uint32_from_bytecode(&bytecode[12]);
	context.dynamic_memory_len = 0;
	context.dynamic_memory = MU_NULL_PTR;

	context.main_return = 0;

	context.last_if = MU_FALSE;

	if (context.static_memory_len > 0) {
		context.static_memory = mu_malloc(context.static_memory_len * sizeof(muByte));
		mu_memset(context.static_memory, 0, context.static_memory_len * sizeof(muByte));
	} else {
		context.static_memory = MU_NULL_PTR;
	}

	if (context.temp_memory_len > 0) {
		context.temp_memory = mu_malloc(context.temp_memory_len * sizeof(muByte));
		mu_memset(context.temp_memory, 0, context.temp_memory_len * sizeof(muByte));
	} else {
		context.temp_memory = MU_NULL_PTR;
	}

	context.reg0 = MU_NULL_PTR;
	context.reg1 = MU_NULL_PTR;
	context.reg2 = MU_NULL_PTR;

	context.bytecode = mu_malloc(bytecode_len * sizeof(muByte));
	mu_memcpy(context.bytecode, bytecode, bytecode_len * sizeof(muByte));
	context.bytecode_len = bytecode_len;

	muByte* step = context.bytecode;
	context.jump_marker_len = 0;
	context.function_len = 0;
	context.external_function_len = 0;
	context.variable_len = 0;
	context.jump_markers = MU_NULL_PTR;
	context.functions = MU_NULL_PTR;
	context.external_functions = MU_NULL_PTR;
	context.variables = MU_NULL_PTR;

	muResult res = MU_SUCCESS;
	while (step < context.bytecode + context.bytecode_len) {
		if (step[0] == 0xF0) {
			context.jump_marker_len++;
		} else if (step[0] == 0xE0) {
			context.function_len++;
		} else if (step[0] == 0x12) {
			context.external_function_len++;
		} else if (step[0] == 0x11) {
			context.variable_len++;
		}
		step = mub_advance_header(&res, &context, step, context.bytecode, context.bytecode_len, MU_FALSE);
		if (res == MU_FAILURE) {
			if (fail_if_invalid_bytecode == MU_TRUE) {
				mu_print("[MUB] Failed to create context; unrecognized command in bytecode.\n");
				context = mu_context_destroy(MU_NULL_PTR, context);
				return context;
			}
		}
	}
	
	if (context.jump_marker_len > 0 || context.function_len > 0 || context.external_function_len > 0) {
		if (context.jump_marker_len > 0) {
			context.jump_markers = mu_malloc(sizeof(muJumpMarker) * context.jump_marker_len);
		}
		if (context.function_len > 0) {
			context.functions = mu_malloc(sizeof(muFunction) * context.function_len);
		}
		if (context.external_function_len > 0) {
			context.external_functions = mu_malloc(sizeof(muExternalFunction) * context.external_function_len);
		}
		muByte* new_step = context.bytecode;
		size_m jump_marker_count = 0;
		size_m function_count = 0;
		size_m external_function_count = 0;
		while (new_step < context.bytecode + context.bytecode_len) {
			if (new_step[0] == 0xF0) {
				context.jump_markers[jump_marker_count].bytecode_index = new_step - context.bytecode;
				switch (context.jump_marker_id_byte_len) { default: break;
					case 1: context.jump_markers[jump_marker_count].index = mu_get_uint8_from_bytecode(&new_step[1]); break;
					case 2: context.jump_markers[jump_marker_count].index = mu_get_uint16_from_bytecode(&new_step[1]); break;
					case 4: context.jump_markers[jump_marker_count].index = mu_get_uint32_from_bytecode(&new_step[1]); break;
					case 8: context.jump_markers[jump_marker_count].index = mu_get_uint64_from_bytecode(&new_step[1]); break;
				}
				jump_marker_count++;
			} else if (new_step[0] == 0xE0) {
				context.functions[function_count].bytecode_index = new_step - context.bytecode;
				switch (context.function_id_byte_len) { default: break;
					case 1: context.functions[function_count].index = mu_get_uint8_from_bytecode(&new_step[1]); break;
					case 2: context.functions[function_count].index = mu_get_uint16_from_bytecode(&new_step[1]); break;
					case 4: context.functions[function_count].index = mu_get_uint32_from_bytecode(&new_step[1]); break;
					case 8: context.functions[function_count].index = mu_get_uint64_from_bytecode(&new_step[1]); break;
				}
				context.functions[function_count].name = MU_NULL_PTR;
				function_count++;
			} else if (new_step[0] == 0x12) {
				context.external_functions[external_function_count].function = MU_NULL_PTR;
				switch (context.external_function_id_byte_len) { default: break; 
					case 1: context.external_functions[external_function_count].index = mu_get_uint8_from_bytecode(&new_step[1]); break;
					case 2: context.external_functions[external_function_count].index = mu_get_uint16_from_bytecode(&new_step[1]); break;
					case 4: context.external_functions[external_function_count].index = mu_get_uint32_from_bytecode(&new_step[1]); break;
					case 8: context.external_functions[external_function_count].index = mu_get_uint64_from_bytecode(&new_step[1]); break;
				}

				size_m offset = 1 + context.external_function_id_byte_len;
				size_m namelen = 0;
				while (new_step + offset + namelen < context.bytecode + context.bytecode_len && new_step[offset+namelen] != '\0') {
					namelen++;
				}
				namelen++;
				context.external_functions[external_function_count].name = mu_malloc(namelen);
				namelen = 0;
				while (new_step + offset + namelen < context.bytecode + context.bytecode_len && new_step[offset+namelen] != '\0') {
					context.external_functions[external_function_count].name[namelen] = new_step[offset+namelen];
					namelen++;
				}
				context.external_functions[external_function_count].name[namelen] = '\0';

				external_function_count++;
			}
			new_step = mub_advance_header(MU_NULL_PTR, &context, new_step, context.bytecode, context.bytecode_len, MU_FALSE);
		}
	}

	context.variables = mu_malloc(context.variable_len * sizeof(muVariable));

	step = context.bytecode;
	size_m variable_count = 0;
	while (step < context.bytecode + context.bytecode_len) {
		if (step[0] == 0x10) {
			muByte* original_original_step = step;
			muByte* original_step = step;
			step++;
			size_m index = 0;
			switch (context.function_id_byte_len) { default: break;
				case 1: index = mu_get_uint8_from_bytecode(step); break;
				case 2: index = mu_get_uint16_from_bytecode(step); break;
				case 4: index = mu_get_uint32_from_bytecode(step); break;
				case 8: index = mu_get_uint64_from_bytecode(step); break;
			}
			step += context.function_id_byte_len;
			size_m actual_index = 0;
			muBool found_index = MU_FALSE;
			if (index < context.function_len && context.functions[index-1].index == index) {
				actual_index = index-1;
				found_index = MU_TRUE;
			} else {
				for (size_m i = 0; i < context.function_len; i++) {
					if (context.functions[i].index == index) {
						actual_index = i;
						found_index = MU_TRUE;
						break;
					}
				}
			}
			if (found_index == MU_FALSE) {
				step = original_step;
			} else if (context.functions[actual_index].name == MU_NULL_PTR) {
				original_step = step;
				while (step < context.bytecode + context.bytecode_len && step[0] != '\0') {
					step++;
				}
				step++;
				context.functions[actual_index].name = mu_malloc(step - original_step);
				step = original_step;
				while (step < context.bytecode + context.bytecode_len && step[0] != '\0') {
					context.functions[actual_index].name[step-original_step] = step[0];
					step++;
				}
				context.functions[actual_index].name[step-original_step] = '\0';
				step = original_original_step;
			}
		} else if (step[0] == 0x11) {
			size_m offset = 1;
			switch (context.bytewidth) { default: break;
				case 1: context.variables[variable_count].address = mu_get_uint8_from_bytecode(&step[1]);  offset += 1; break;
				case 2: context.variables[variable_count].address = mu_get_uint16_from_bytecode(&step[1]); offset += 2; break;
				case 4: context.variables[variable_count].address = mu_get_uint32_from_bytecode(&step[1]); offset += 4; break;
				case 8: context.variables[variable_count].address = mu_get_uint64_from_bytecode(&step[1]); offset += 8; break;
			}
			context.variables[variable_count].datatype = mu_get_data_type_from_bytecode(&step[offset]);
			offset += 3;

			size_m namelen = 0;
			while (step + offset + namelen < context.bytecode + context.bytecode_len && step[offset + namelen] != '\0') {
				namelen++;
			}
			namelen++;
			context.variables[variable_count].name = mu_malloc(namelen);
			namelen = 0;
			while (step + offset + namelen < context.bytecode + context.bytecode_len && step[offset + namelen] != '\0') {
				context.variables[variable_count].name[namelen] = step[offset + namelen];
				namelen++;
			}
			context.variables[variable_count].name[namelen] = step[offset + namelen];
			namelen++;

			variable_count++;
		}
		step = mub_advance_header(MU_NULL_PTR, &context, step, context.bytecode, context.bytecode_len, MU_FALSE);
	}

	context.alive = MU_TRUE;

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

	if (context.jump_markers != MU_NULL_PTR) {
		mu_free(context.jump_markers);
		context.jump_markers = MU_NULL_PTR;
		context.jump_marker_len = 0;
	}

	if (context.functions != MU_NULL_PTR) {
		for (size_m i = 0; i < context.function_len; i++) {
			if (context.functions[i].name != MU_NULL_PTR) {
				mu_free(context.functions[i].name);
				context.functions[i].name = MU_NULL_PTR;
			}
		}
		mu_free(context.functions);
		context.functions = MU_NULL_PTR;
		context.function_len = 0;
	}

	if (context.external_functions != MU_NULL_PTR) {
		for (size_m i = 0; i < context.external_function_len; i++) {
			if (context.external_functions[i].name != MU_NULL_PTR) {
				mu_free(context.external_functions[i].name);
				context.external_functions[i].name = MU_NULL_PTR;
			}
		}
		mu_free(context.external_functions);
		context.external_functions = MU_NULL_PTR;
		context.external_function_len = 0;
	}

	if (context.variables != MU_NULL_PTR) {
		for (size_m i = 0; i < context.variable_len; i++) {
			if (context.variables[i].name != MU_NULL_PTR) {
				mu_free(context.variables[i].name);
				context.variables[i].name = MU_NULL_PTR;
			}
		}
		mu_free(context.variables);
		context.variables = MU_NULL_PTR;
		context.variable_len = 0;
	}

	if (context.static_memory != MU_NULL_PTR) {
		mu_free(context.static_memory);
		context.static_memory = MU_NULL_PTR;
		context.static_memory_len = 0;
	}

	if (context.temp_memory != MU_NULL_PTR) {
		mu_free(context.temp_memory);
		context.temp_memory = MU_NULL_PTR;
		context.temp_memory_len = 0;
	}

	if (context.dynamic_memory != MU_NULL_PTR) {
		mu_free(context.dynamic_memory);
		context.dynamic_memory = MU_NULL_PTR;
		context.dynamic_memory_len = 0;
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

	context.alive = MU_FALSE;

	if (result != MU_NULL_PTR) {
		*result = MU_SUCCESS;
	}

	return context;
}

MUDEF int mu_context_execute_main(muResult* result, muContext* context) {
	if (context->alive == MU_FALSE) {
		mu_print("[MUB] Failed to execute main; given context is not valid.\n");
		if (result != MU_NULL_PTR) {
			*result = MU_FAILURE;
		}
		return 0;
	}
	size_m main_function_id = 0;
	muBool found_main_function = MU_FALSE;
	if (context->function_len > 0 && context->functions[context->function_len-1].index == 0) {
		main_function_id = context->function_len - 1;
		found_main_function = MU_TRUE;
	} else {
		for (size_m i = 0; i < context->function_len; i++) {
			if (context->functions[i].index == 0) {
				main_function_id = context->function_len - 1;
				found_main_function = MU_TRUE;
				break;
			}
		}
	}
	if (found_main_function == MU_FALSE) {
		mu_print("[MUB] Failed to execute main; no main function specified in the bytecode.\n");
		if (result != MU_NULL_PTR) {
			*result = MU_FAILURE;
		}
		return 0;
	}

	muByte* step = &context->bytecode[context->functions[main_function_id].bytecode_index];
	while (step < context->bytecode + context->bytecode_len) {
		if (step[0] == 0xE1) {
			break;
		}
		muBool ifCheck = MU_FALSE;
		if (step[0] == 0xA0) {
			ifCheck = MU_TRUE;
		}
		if (mub_execute_command(context, step) != MU_SUCCESS) {
			if (result != MU_NULL_PTR) {
				*result = MU_FAILURE;
			}
			return 0;
		}
		if ((ifCheck && context->last_if == MU_FALSE) || step[0] == 0xA2) {
			size_m if_count = 1;
			while (if_count != 0 && step < context->bytecode + context->bytecode_len) {
				step = mub_advance_header(MU_NULL_PTR, context, step, context->bytecode, context->bytecode_len, MU_FALSE);
				if (step[0] == 0xA0 || step[0] == 0xA2) {
					if_count++;
				} else if (step[0] == 0xA1) {
					if_count--;
				}
			}
			if (step[1] == 0xA2) {
				step += 2;
			}
		} else {
			if (step[0] == 0x00) {
				return context->main_return;
			}
			step = mub_advance_header(MU_NULL_PTR, context, step, context->bytecode, context->bytecode_len, MU_TRUE);
		}
	}

	if (result != MU_NULL_PTR) {
		*result = MU_SUCCESS;
	}

	return 0;
}

MUDEF void mu_context_execute_function(muResult* result, muContext* context, size_m id) {
	if (context->alive == MU_FALSE) {
		mu_print("[MUB] Failed to execute function; given context is not valid.\n");
		if (result != MU_NULL_PTR) {
			*result = MU_FAILURE;
		}
		return;
	}
	if (id == 0) {
		mu_print("[MUB] Failed to execute function; ID #0 (aka main) cannot be executed via the \"execute function\" function, the \"execute main\" function needs to be used.\n");
		if (result != MU_NULL_PTR) {
			*result = MU_FAILURE;
		}
		return;
	}
	size_m actual_index = 0;
	muBool found_index = MU_FALSE;
	if (id < context->function_len && context->functions[id-1].index == id) {
		actual_index = id-1;
		found_index = MU_TRUE;
	} else {
		for (size_m i = 0; i < context->function_len; i++) {
			if (context->functions[i].index == id) {
				actual_index = i;
				found_index = MU_TRUE;
				break;
			}
		}
	}
	if (found_index == MU_FALSE) {
		mu_print("[MUB] Failed to execute function; no function has the given ID.\n");
		if (result != MU_NULL_PTR) {
			*result = MU_FAILURE;
		}
		return;
	}
	if (result != MU_NULL_PTR) {
		*result = mub_execute_function(context, &context->bytecode[context->functions[actual_index].bytecode_index]);
	} else {
		mub_execute_function(context, &context->bytecode[context->functions[actual_index].bytecode_index]);
	}
}

MUDEF size_m mu_context_get_function_id(muResult* result, muContext* context, const char* name) {
	if (context->alive == MU_FALSE) {
		mu_print("[MUB] Failed to get function id; given context is not valid.\n");
		if (result != MU_NULL_PTR) {
			*result = MU_FAILURE;
		}
		return 0;
	}
	for (size_m i = 0; i < context->function_len; i++) {
		if (context->functions[i].name != MU_NULL_PTR && mu_strcmp(name, context->functions[i].name) == 0) {
			if (result != MU_NULL_PTR) {
				*result = MU_SUCCESS;
			}
			return context->functions[i].index;
		}
	}
	mu_print("[MUB] Failed to get function id; no function name matches.\n");
	if (result != MU_NULL_PTR) {
		*result = MU_FAILURE;
	}
	return 0;
}

MUDEF muVariable mu_context_get_variable(muResult* result, muContext* context, const char* name) {
	if (context->alive == MU_FALSE) {
		mu_print("[MUB] Failed to get variable; given context is not valid.\n");
		if (result != MU_NULL_PTR) {
			*result = MU_FAILURE;
		}
		muVariable variable = {0};
		return variable;
	}
	for (size_m i = 0; i < context->variable_len; i++) {
		if (context->variables[i].name != MU_NULL_PTR && mu_strcmp(name, context->variables[i].name) == 0) {
			if (result != MU_NULL_PTR) {
				*result = MU_SUCCESS;
			}
			return context->variables[i];
		}
	}
	mu_print("[MUB] Failed to get variable; no variable with the given name could found.\n");
	if (result != MU_NULL_PTR) {
		*result = MU_FAILURE;
	}
	muVariable variable = {0};
	return variable;
}

MUDEF void mu_context_get_variable_data(muResult* result, muContext* context, muVariable variable, void* data) {
	if (context->alive == MU_FALSE) {
		mu_print("[MUB] Failed to get variable data; given context is not valid.\n");
		if (result != MU_NULL_PTR) {
			*result = MU_FAILURE;
		}
		return;
	}
	muByte* location = mu_context_get_memory_location(context, variable.address, variable.datatype.temp);
	if (location == MU_NULL_PTR || 
		mu_context_get_memory_location(context, variable.address + variable.datatype.byte_size - 1, variable.datatype.temp) == MU_NULL_PTR
	) {
		mu_print("[MUB] Failed to get variable data; memory address location is out of range.\n");
		if (result != MU_NULL_PTR) {
			*result = MU_FAILURE;
		}
		return;
	}
	mu_memcpy(data, location, variable.datatype.byte_size);
	if (result != MU_NULL_PTR) {
		*result = MU_SUCCESS;
	}
}

MUDEF void mu_context_set_variable_data(muResult* result, muContext* context, muVariable variable, void* data) {
	if (context->alive == MU_FALSE) {
		mu_print("[MUB] Failed to set variable data; given context is not valid.\n");
		if (result != MU_NULL_PTR) {
			*result = MU_FAILURE;
		}
		return;
	}
	muByte* location = mu_context_get_memory_location(context, variable.address, variable.datatype.temp);
	if (location == MU_NULL_PTR || 
		mu_context_get_memory_location(context, variable.address + variable.datatype.byte_size - 1, variable.datatype.temp) == MU_NULL_PTR
	) {
		mu_print("[MUB] Failed to set variable data; memory address location is out of range.\n");
		if (result != MU_NULL_PTR) {
			*result = MU_FAILURE;
		}
		return;
	}
	mu_memcpy(location, data, variable.datatype.byte_size);
	if (result != MU_NULL_PTR) {
		*result = MU_SUCCESS;
	}
}

MUDEF void mu_context_set_external_function(muResult* result, muContext* context, const char* name, void(*function)(muContext* context)) {
	if (context->alive == MU_FALSE) {
		mu_print("[MUB] Failed to set external function; given context is not valid.\n");
		if (result != MU_NULL_PTR) {
			*result = MU_FAILURE;
		}
		return;
	}
	for (size_m i = 0; i < context->external_function_len; i++) {
		if (context->external_functions[i].name != MU_NULL_PTR && mu_strcmp(name, context->external_functions[i].name) == 0) {
			context->external_functions[i].function = (void(*)(void* context))function;
			if (result != MU_NULL_PTR) {
				*result = MU_SUCCESS;
			}
			return;
		}
	}
	mu_print("[MUB] Failed to set external function; no external function with the given name could be found specified within the bytecode.\n");
	if (result != MU_NULL_PTR) {
		*result = MU_FAILURE;
	}
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
	muByte* mem_loc = mu_context_get_memory_location(context, reg1_val, dst_dt.temp);
	if (mem_loc == MU_NULL_PTR) {
		mu_print("[MUB] Failed to perform operation; invalid memory address.\n");
		return MU_FAILURE;
	}
	switch (operation) { default: break; 
		case MUB_OPERATION_MOVE: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_ADD: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0]) + *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0]) + *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0]) + *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0]) + *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0]) + *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0]) + *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0]) + *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0]) + *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0]) + *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0]) + *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_SUBTRACT: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0]) - *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0]) - *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0]) - *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0]) - *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0]) - *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0]) - *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0]) - *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0]) - *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0]) - *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0]) - *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0]) - *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0]) - *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0]) - *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0]) - *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0]) - *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0]) - *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0]) - *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0]) - *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0]) - *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0]) - *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0]) - *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0]) - *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0]) - *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0]) - *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0]) - *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0]) - *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0]) - *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0]) - *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0]) - *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0]) - *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0]) - *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0]) - *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0]) - *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0]) - *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0]) - *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0]) - *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0]) - *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0]) - *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0]) - *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0]) - *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0]) - *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0]) - *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0]) - *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0]) - *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0]) - *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0]) - *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0]) - *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0]) - *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0]) - *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0]) - *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0]) - *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0]) - *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0]) - *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0]) - *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0]) - *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0]) - *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0]) - *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0]) - *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0]) - *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0]) - *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0]) - *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0]) - *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0]) - *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0]) - *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0]) - *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0]) - *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0]) - *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0]) - *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0]) - *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0]) - *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0]) - *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0]) - *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0]) - *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0]) - *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0]) - *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0]) - *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0]) - *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0]) - *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0]) - *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0]) - *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0]) - *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0]) - *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0]) - *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0]) - *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0]) - *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0]) - *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0]) - *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0]) - *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0]) - *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0]) - *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0]) - *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0]) - *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0]) - *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0]) - *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0]) - *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0]) - *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0]) - *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0]) - *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0]) - *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0]) - *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_MULTIPLY: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0]) * *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0]) * *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0]) * *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0]) * *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0]) * *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0]) * *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0]) * *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0]) * *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0]) * *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0]) * *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0]) * *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0]) * *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0]) * *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0]) * *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0]) * *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0]) * *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0]) * *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0]) * *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0]) * *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0]) * *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0]) * *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0]) * *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0]) * *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0]) * *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0]) * *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0]) * *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0]) * *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0]) * *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0]) * *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0]) * *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0]) * *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0]) * *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0]) * *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0]) * *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0]) * *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0]) * *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0]) * *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0]) * *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0]) * *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0]) * *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0]) * *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0]) * *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0]) * *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0]) * *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0]) * *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0]) * *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0]) * *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0]) * *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0]) * *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0]) * *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0]) * *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0]) * *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0]) * *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0]) * *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0]) * *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0]) * *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0]) * *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0]) * *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0]) * *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0]) * *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0]) * *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0]) * *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0]) * *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0]) * *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0]) * *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0]) * *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0]) * *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0]) * *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0]) * *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0]) * *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0]) * *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0]) * *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0]) * *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0]) * *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0]) * *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0]) * *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0]) * *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0]) * *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0]) * *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0]) * *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0]) * *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0]) * *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0]) * *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0]) * *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0]) * *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0]) * *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0]) * *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0]) * *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0]) * *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0]) * *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0]) * *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0]) * *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0]) * *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0]) * *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0]) * *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0]) * *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0]) * *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0]) * *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0]) * *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0]) * *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_DIVIDE: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0]) / *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0]) / *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0]) / *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0]) / *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0]) / *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0]) / *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0]) / *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0]) / *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0]) / *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0]) / *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0]) / *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0]) / *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0]) / *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0]) / *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0]) / *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0]) / *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0]) / *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0]) / *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0]) / *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0]) / *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0]) / *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0]) / *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0]) / *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0]) / *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0]) / *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0]) / *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0]) / *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0]) / *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0]) / *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0]) / *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0]) / *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0]) / *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0]) / *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0]) / *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0]) / *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0]) / *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0]) / *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0]) / *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0]) / *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0]) / *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0]) / *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0]) / *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0]) / *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0]) / *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0]) / *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0]) / *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0]) / *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0]) / *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0]) / *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0]) / *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0]) / *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0]) / *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0]) / *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0]) / *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0]) / *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0]) / *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0]) / *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0]) / *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0]) / *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0]) / *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0]) / *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0]) / *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0]) / *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0]) / *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0]) / *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0]) / *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0]) / *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0]) / *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0]) / *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0]) / *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0]) / *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0]) / *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0]) / *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0]) / *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0]) / *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0]) / *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0]) / *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0]) / *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0]) / *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0]) / *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0]) / *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0]) / *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0]) / *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0]) / *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0]) / *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0]) / *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0]) / *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0]) / *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0]) / *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0]) / *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0]) / *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0]) / *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0]) / *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0]) / *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0]) / *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0]) / *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0]) / *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0]) / *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0]) / *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0]) / *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_MODULO: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0]) % *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0]) % *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0]) % *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0]) % *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0]) % *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0]) % *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0]) % *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0]) % *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0]) % *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0]) % *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0]) % *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0]) % *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0]) % *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0]) % *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0]) % *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0]) % *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0]) % *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0]) % *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0]) % *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0]) % *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0]) % *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0]) % *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0]) % *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0]) % *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0]) % *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0]) % *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0]) % *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0]) % *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0]) % *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0]) % *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0]) % *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0]) % *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0]) % *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0]) % *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0]) % *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0]) % *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0]) % *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0]) % *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0]) % *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0]) % *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0]) % *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0]) % *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0]) % *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0]) % *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0]) % *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0]) % *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0]) % *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0]) % *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0]) % *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0]) % *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0]) % *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0]) % *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0]) % *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0]) % *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0]) % *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0]) % *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0]) % *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0]) % *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0]) % *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0]) % *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0]) % *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0]) % *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0]) % *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0]) % *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0]) % *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0]) % *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0]) % *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0]) % *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0]) % *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0]) % *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0]) % *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0]) % *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0]) % *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0]) % *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0]) % *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0]) % *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0]) % *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0]) % *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0]) % *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0]) % *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0]) % *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0]) % *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0]) % *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0]) % *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0]) % *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0]) % *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0]) % *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0]) % *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0]) % *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0]) % *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0]) % *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0]) % *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0]) % *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0]) % *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0]) % *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0]) % *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0]) % *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0]) % *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0]) % *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0]) % *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_BW_NOT: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(~*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(~*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(~*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(~*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(~*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(~*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(~*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(~*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(~*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(~*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(~*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(~*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(~*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(~*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(~*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(~*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(~*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(~*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(~*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(~*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(~*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(~*((uint8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(~*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(~*((int8_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(~*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(~*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(~*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(~*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(~*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(~*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(~*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(~*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(~*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(~*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(~*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(~*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(~*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(~*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(~*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(~*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(~*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(~*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(~*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(~*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(~*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(~*((uint16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(~*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(~*((int16_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(~*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(~*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(~*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(~*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(~*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(~*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(~*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(~*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(~*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(~*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(~*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(~*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(~*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(~*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(~*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(~*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(~*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(~*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(~*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(~*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(~*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(~*((uint32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(~*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(~*((int32_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(~*((float*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(~*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(~*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(~*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(~*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((int8_m*)mem_loc) = (int8_m)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(~*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(~*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(~*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(~*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((int16_m*)mem_loc) = (int16_m)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(~*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(~*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(~*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(~*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(~*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(~*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(~*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(~*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((int32_m*)mem_loc) = (int32_m)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((float*)mem_loc) = (float)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((float*)mem_loc) = (float)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(~*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(~*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(~*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(~*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(~*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(~*((uint64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(~*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(~*((int64_m*)&context->reg0[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((int64_m*)mem_loc) = (int64_m)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((double*)mem_loc) = (double)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED: { *((double*)mem_loc) = (double)(~*((double*)&context->reg0[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_BW_AND: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0]) & *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0]) & *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0]) & *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0]) & *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0]) & *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0]) & *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0]) & *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0]) & *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0]) & *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0]) & *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0]) & *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0]) & *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0]) & *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0]) & *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0]) & *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0]) & *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0]) & *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0]) & *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0]) & *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0]) & *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0]) & *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0]) & *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0]) & *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0]) & *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0]) & *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0]) & *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0]) & *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0]) & *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0]) & *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0]) & *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0]) & *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0]) & *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0]) & *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0]) & *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0]) & *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0]) & *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0]) & *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0]) & *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0]) & *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0]) & *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0]) & *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0]) & *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0]) & *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0]) & *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0]) & *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0]) & *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0]) & *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0]) & *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0]) & *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0]) & *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0]) & *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0]) & *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0]) & *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0]) & *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0]) & *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0]) & *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0]) & *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0]) & *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0]) & *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0]) & *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0]) & *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0]) & *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0]) & *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0]) & *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0]) & *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0]) & *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0]) & *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0]) & *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0]) & *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0]) & *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0]) & *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0]) & *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0]) & *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0]) & *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0]) & *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0]) & *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0]) & *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0]) & *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0]) & *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0]) & *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0]) & *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0]) & *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0]) & *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0]) & *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0]) & *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0]) & *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0]) & *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0]) & *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0]) & *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0]) & *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0]) & *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0]) & *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0]) & *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0]) & *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0]) & *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0]) & *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0]) & *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0]) & *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0]) & *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0]) & *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_BW_OR: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0]) | *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0]) | *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0]) | *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0]) | *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0]) | *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0]) | *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0]) | *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0]) | *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0]) | *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0]) | *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0]) | *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0]) | *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0]) | *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0]) | *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0]) | *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0]) | *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0]) | *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0]) | *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0]) | *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0]) | *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0]) | *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0]) | *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0]) | *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0]) | *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0]) | *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0]) | *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0]) | *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0]) | *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0]) | *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0]) | *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0]) | *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0]) | *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0]) | *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0]) | *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0]) | *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0]) | *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0]) | *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0]) | *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0]) | *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0]) | *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0]) | *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0]) | *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0]) | *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0]) | *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0]) | *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0]) | *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0]) | *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0]) | *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0]) | *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0]) | *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0]) | *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0]) | *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0]) | *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0]) | *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0]) | *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0]) | *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0]) | *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0]) | *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0]) | *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0]) | *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0]) | *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0]) | *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0]) | *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0]) | *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0]) | *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0]) | *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0]) | *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0]) | *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0]) | *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0]) | *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0]) | *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0]) | *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0]) | *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0]) | *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0]) | *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0]) | *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0]) | *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0]) | *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0]) | *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0]) | *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0]) | *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0]) | *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0]) | *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0]) | *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0]) | *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0]) | *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0]) | *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0]) | *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0]) | *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0]) | *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0]) | *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0]) | *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0]) | *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0]) | *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0]) | *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0]) | *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0]) | *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0]) | *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0]) | *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0]) | *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_BW_XOR: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0]) ^ *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0]) ^ *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0]) ^ *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0]) ^ *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0]) ^ *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0]) ^ *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0]) ^ *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0]) ^ *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0]) ^ *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0]) ^ *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0]) ^ *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0]) ^ *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0]) ^ *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0]) ^ *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0]) ^ *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0]) ^ *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0]) ^ *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0]) ^ *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0]) ^ *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0]) ^ *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0]) ^ *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0]) ^ *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0]) ^ *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0]) ^ *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0]) ^ *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0]) ^ *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0]) ^ *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0]) ^ *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0]) ^ *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0]) ^ *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0]) ^ *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0]) ^ *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0]) ^ *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0]) ^ *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0]) ^ *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0]) ^ *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0]) ^ *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0]) ^ *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0]) ^ *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0]) ^ *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0]) ^ *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0]) ^ *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0]) ^ *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0]) ^ *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0]) ^ *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0]) ^ *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0]) ^ *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0]) ^ *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0]) ^ *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0]) ^ *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0]) ^ *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0]) ^ *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0]) ^ *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0]) ^ *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0]) ^ *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0]) ^ *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0]) ^ *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0]) ^ *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0]) ^ *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0]) ^ *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0]) ^ *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0]) ^ *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0]) ^ *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0]) ^ *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0]) ^ *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0]) ^ *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0]) ^ *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0]) ^ *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0]) ^ *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0]) ^ *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0]) ^ *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0]) ^ *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0]) ^ *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0]) ^ *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0]) ^ *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0]) ^ *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0]) ^ *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0]) ^ *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0]) ^ *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0]) ^ *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0]) ^ *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0]) ^ *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0]) ^ *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0]) ^ *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0]) ^ *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0]) ^ *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0]) ^ *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0]) ^ *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0]) ^ *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0]) ^ *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0]) ^ *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0]) ^ *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0]) ^ *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0]) ^ *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0]) ^ *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0]) ^ *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0]) ^ *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0]) ^ *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0]) ^ *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0]) ^ *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_BW_LSHIFT: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0]) << *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0]) << *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0]) << *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0]) << *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0]) << *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0]) << *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0]) << *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0]) << *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0]) << *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0]) << *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0]) << *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0]) << *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0]) << *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0]) << *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0]) << *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0]) << *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0]) << *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0]) << *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0]) << *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0]) << *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0]) << *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0]) << *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0]) << *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0]) << *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0]) << *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0]) << *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0]) << *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0]) << *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0]) << *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0]) << *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0]) << *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0]) << *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0]) << *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0]) << *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0]) << *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0]) << *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0]) << *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0]) << *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0]) << *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0]) << *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0]) << *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0]) << *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0]) << *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0]) << *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0]) << *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0]) << *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0]) << *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0]) << *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0]) << *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0]) << *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0]) << *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0]) << *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0]) << *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0]) << *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0]) << *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0]) << *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0]) << *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0]) << *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0]) << *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0]) << *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0]) << *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0]) << *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0]) << *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0]) << *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0]) << *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0]) << *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0]) << *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0]) << *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0]) << *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0]) << *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0]) << *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0]) << *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0]) << *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0]) << *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0]) << *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0]) << *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0]) << *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0]) << *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0]) << *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0]) << *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0]) << *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0]) << *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0]) << *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0]) << *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0]) << *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0]) << *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0]) << *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0]) << *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0]) << *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0]) << *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0]) << *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0]) << *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0]) << *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0]) << *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0]) << *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0]) << *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0]) << *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0]) << *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0]) << *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0]) << *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_BW_RSHIFT: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0]) >> *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0]) >> *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0]) >> *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0]) >> *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0]) >> *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0]) >> *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0]) >> *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0]) >> *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0]) >> *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0]) >> *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0]) >> *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0]) >> *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0]) >> *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0]) >> *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0]) >> *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0]) >> *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0]) >> *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0]) >> *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0]) >> *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0]) >> *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0]) >> *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0]) >> *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0]) >> *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0]) >> *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0]) >> *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0]) >> *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0]) >> *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0]) >> *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0]) >> *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0]) >> *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0]) >> *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0]) >> *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0]) >> *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0]) >> *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0]) >> *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0]) >> *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0]) >> *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0]) >> *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0]) >> *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0]) >> *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0]) >> *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0]) >> *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0]) >> *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0]) >> *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0]) >> *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0]) >> *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0]) >> *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0]) >> *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0]) >> *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0]) >> *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0]) >> *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0]) >> *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0]) >> *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0]) >> *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0]) >> *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0]) >> *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0]) >> *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0]) >> *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0]) >> *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0]) >> *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0]) >> *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0]) >> *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0]) >> *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0]) >> *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0]) >> *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0]) >> *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0]) >> *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0]) >> *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0]) >> *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0]) >> *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0]) >> *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0]) >> *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0]) >> *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0]) >> *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0]) >> *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0]) >> *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0]) >> *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0]) >> *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0]) >> *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0]) >> *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0]) >> *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0]) >> *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0]) >> *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0]) >> *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0]) >> *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0]) >> *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0]) >> *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0]) >> *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0]) >> *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0]) >> *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0]) >> *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0]) >> *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0]) >> *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0]) >> *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0]) >> *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0]) >> *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0]) >> *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0]) >> *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														//case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0]) >> *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														//case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0]) >> *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_COND_EQUAL: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0]) == *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0]) == *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0]) == *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0]) == *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0]) == *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0]) == *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0]) == *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0]) == *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0]) == *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0]) == *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0]) == *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0]) == *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0]) == *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0]) == *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0]) == *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0]) == *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0]) == *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0]) == *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0]) == *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0]) == *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0]) == *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0]) == *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0]) == *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0]) == *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0]) == *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0]) == *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0]) == *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0]) == *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0]) == *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0]) == *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0]) == *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0]) == *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0]) == *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0]) == *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0]) == *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0]) == *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0]) == *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0]) == *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0]) == *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0]) == *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0]) == *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0]) == *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0]) == *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0]) == *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0]) == *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0]) == *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0]) == *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0]) == *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0]) == *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0]) == *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0]) == *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0]) == *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0]) == *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0]) == *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0]) == *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0]) == *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0]) == *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0]) == *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0]) == *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0]) == *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0]) == *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0]) == *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0]) == *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0]) == *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0]) == *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0]) == *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0]) == *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0]) == *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0]) == *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0]) == *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0]) == *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0]) == *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0]) == *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0]) == *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0]) == *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0]) == *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0]) == *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0]) == *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0]) == *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0]) == *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0]) == *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0]) == *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0]) == *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0]) == *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0]) == *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0]) == *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0]) == *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0]) == *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0]) == *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0]) == *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0]) == *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0]) == *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0]) == *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0]) == *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0]) == *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0]) == *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0]) == *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0]) == *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0]) == *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0]) == *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_COND_NOT_EQUAL: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0]) != *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0]) != *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0]) != *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0]) != *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0]) != *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0]) != *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0]) != *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0]) != *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0]) != *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0]) != *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0]) != *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0]) != *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0]) != *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0]) != *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0]) != *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0]) != *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0]) != *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0]) != *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0]) != *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0]) != *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0]) != *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0]) != *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0]) != *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0]) != *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0]) != *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0]) != *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0]) != *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0]) != *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0]) != *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0]) != *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0]) != *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0]) != *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0]) != *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0]) != *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0]) != *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0]) != *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0]) != *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0]) != *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0]) != *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0]) != *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0]) != *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0]) != *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0]) != *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0]) != *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0]) != *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0]) != *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0]) != *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0]) != *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0]) != *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0]) != *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0]) != *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0]) != *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0]) != *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0]) != *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0]) != *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0]) != *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0]) != *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0]) != *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0]) != *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0]) != *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0]) != *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0]) != *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0]) != *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0]) != *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0]) != *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0]) != *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0]) != *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0]) != *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0]) != *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0]) != *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0]) != *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0]) != *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0]) != *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0]) != *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0]) != *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0]) != *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0]) != *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0]) != *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0]) != *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0]) != *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0]) != *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0]) != *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0]) != *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0]) != *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0]) != *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0]) != *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0]) != *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0]) != *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0]) != *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0]) != *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0]) != *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0]) != *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0]) != *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0]) != *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0]) != *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0]) != *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0]) != *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0]) != *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0]) != *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0]) != *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_COND_LESS: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0]) < *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0]) < *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0]) < *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0]) < *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0]) < *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0]) < *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0]) < *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0]) < *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0]) < *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0]) < *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0]) < *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0]) < *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0]) < *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0]) < *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0]) < *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0]) < *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0]) < *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0]) < *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0]) < *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0]) < *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0]) < *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0]) < *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0]) < *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0]) < *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0]) < *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0]) < *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0]) < *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0]) < *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0]) < *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0]) < *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0]) < *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0]) < *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0]) < *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0]) < *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0]) < *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0]) < *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0]) < *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0]) < *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0]) < *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0]) < *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0]) < *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0]) < *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0]) < *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0]) < *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0]) < *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0]) < *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0]) < *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0]) < *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0]) < *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0]) < *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0]) < *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0]) < *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0]) < *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0]) < *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0]) < *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0]) < *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0]) < *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0]) < *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0]) < *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0]) < *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0]) < *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0]) < *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0]) < *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0]) < *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0]) < *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0]) < *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0]) < *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0]) < *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0]) < *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0]) < *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0]) < *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0]) < *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0]) < *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0]) < *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0]) < *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0]) < *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0]) < *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0]) < *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0]) < *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0]) < *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0]) < *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0]) < *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0]) < *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0]) < *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0]) < *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0]) < *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0]) < *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0]) < *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0]) < *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0]) < *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0]) < *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0]) < *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0]) < *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0]) < *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0]) < *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0]) < *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0]) < *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0]) < *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0]) < *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0]) < *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_COND_LESS_EQUAL: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0]) <= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0]) <= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0]) <= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0]) <= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0]) <= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0]) <= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0]) <= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0]) <= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0]) <= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0]) <= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0]) <= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0]) <= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0]) <= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0]) <= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0]) <= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0]) <= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0]) <= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0]) <= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0]) <= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0]) <= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0]) <= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0]) <= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0]) <= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0]) <= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0]) <= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0]) <= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0]) <= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0]) <= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0]) <= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0]) <= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0]) <= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0]) <= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0]) <= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0]) <= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0]) <= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0]) <= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0]) <= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0]) <= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0]) <= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0]) <= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0]) <= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0]) <= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0]) <= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0]) <= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0]) <= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0]) <= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0]) <= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0]) <= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0]) <= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0]) <= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0]) <= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0]) <= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0]) <= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0]) <= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0]) <= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0]) <= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0]) <= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0]) <= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0]) <= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0]) <= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0]) <= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0]) <= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0]) <= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0]) <= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0]) <= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0]) <= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0]) <= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0]) <= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0]) <= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0]) <= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0]) <= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0]) <= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0]) <= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0]) <= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0]) <= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0]) <= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0]) <= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0]) <= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0]) <= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0]) <= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0]) <= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0]) <= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0]) <= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0]) <= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0]) <= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0]) <= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0]) <= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0]) <= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0]) <= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0]) <= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0]) <= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0]) <= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0]) <= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0]) <= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0]) <= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0]) <= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0]) <= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0]) <= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0]) <= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0]) <= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_COND_GREATER: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0]) > *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0]) > *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0]) > *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0]) > *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0]) > *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0]) > *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0]) > *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0]) > *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0]) > *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0]) > *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0]) > *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0]) > *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0]) > *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0]) > *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0]) > *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0]) > *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0]) > *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0]) > *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0]) > *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0]) > *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0]) > *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0]) > *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0]) > *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0]) > *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0]) > *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0]) > *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0]) > *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0]) > *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0]) > *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0]) > *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0]) > *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0]) > *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0]) > *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0]) > *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0]) > *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0]) > *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0]) > *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0]) > *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0]) > *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0]) > *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0]) > *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0]) > *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0]) > *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0]) > *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0]) > *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0]) > *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0]) > *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0]) > *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0]) > *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0]) > *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0]) > *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0]) > *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0]) > *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0]) > *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0]) > *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0]) > *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0]) > *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0]) > *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0]) > *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0]) > *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0]) > *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0]) > *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0]) > *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0]) > *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0]) > *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0]) > *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0]) > *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0]) > *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0]) > *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0]) > *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0]) > *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0]) > *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0]) > *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0]) > *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0]) > *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0]) > *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0]) > *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0]) > *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0]) > *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0]) > *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0]) > *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0]) > *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0]) > *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0]) > *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0]) > *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0]) > *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0]) > *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0]) > *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0]) > *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0]) > *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0]) > *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0]) > *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0]) > *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0]) > *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0]) > *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0]) > *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0]) > *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0]) > *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0]) > *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0]) > *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
		case MUB_OPERATION_COND_GREATER_EQUAL: {
			switch (src0_dt.byte_size) { default: break;
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
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint8_m*)&context->reg0[0]) >= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint8_m*)&context->reg0[0]) >= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int8_m*)&context->reg0[0]) >= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int8_m*)&context->reg0[0]) >= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint8_m*)&context->reg0[0]) >= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint8_m*)&context->reg0[0]) >= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int8_m*)&context->reg0[0]) >= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int8_m*)&context->reg0[0]) >= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint8_m*)&context->reg0[0]) >= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint8_m*)&context->reg0[0]) >= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int8_m*)&context->reg0[0]) >= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int8_m*)&context->reg0[0]) >= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint8_m*)&context->reg0[0]) >= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int8_m*)&context->reg0[0]) >= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint8_m*)&context->reg0[0]) >= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint8_m*)&context->reg0[0]) >= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int8_m*)&context->reg0[0]) >= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int8_m*)&context->reg0[0]) >= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint8_m*)&context->reg0[0]) >= *((uint8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int8_m*)&context->reg0[0]) >= *((int8_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint16_m*)&context->reg0[0]) >= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint16_m*)&context->reg0[0]) >= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int16_m*)&context->reg0[0]) >= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int16_m*)&context->reg0[0]) >= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint16_m*)&context->reg0[0]) >= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint16_m*)&context->reg0[0]) >= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int16_m*)&context->reg0[0]) >= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int16_m*)&context->reg0[0]) >= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint16_m*)&context->reg0[0]) >= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint16_m*)&context->reg0[0]) >= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int16_m*)&context->reg0[0]) >= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int16_m*)&context->reg0[0]) >= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint16_m*)&context->reg0[0]) >= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int16_m*)&context->reg0[0]) >= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint16_m*)&context->reg0[0]) >= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint16_m*)&context->reg0[0]) >= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int16_m*)&context->reg0[0]) >= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int16_m*)&context->reg0[0]) >= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint16_m*)&context->reg0[0]) >= *((uint16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int16_m*)&context->reg0[0]) >= *((int16_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint32_m*)&context->reg0[0]) >= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint32_m*)&context->reg0[0]) >= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int32_m*)&context->reg0[0]) >= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int32_m*)&context->reg0[0]) >= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((float*)&context->reg0[0]) >= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((float*)&context->reg0[0]) >= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint32_m*)&context->reg0[0]) >= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint32_m*)&context->reg0[0]) >= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int32_m*)&context->reg0[0]) >= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int32_m*)&context->reg0[0]) >= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((float*)&context->reg0[0]) >= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((float*)&context->reg0[0]) >= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint32_m*)&context->reg0[0]) >= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint32_m*)&context->reg0[0]) >= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int32_m*)&context->reg0[0]) >= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int32_m*)&context->reg0[0]) >= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint32_m*)&context->reg0[0]) >= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int32_m*)&context->reg0[0]) >= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((float*)&context->reg0[0]) >= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((float*)&context->reg0[0]) >= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((float*)&context->reg0[0]) >= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint32_m*)&context->reg0[0]) >= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint32_m*)&context->reg0[0]) >= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int32_m*)&context->reg0[0]) >= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int32_m*)&context->reg0[0]) >= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint32_m*)&context->reg0[0]) >= *((uint32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int32_m*)&context->reg0[0]) >= *((int32_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((float*)&context->reg0[0]) >= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((float*)&context->reg0[0]) >= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((float*)&context->reg0[0]) >= *((float*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((uint64_m*)&context->reg0[0]) >= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((uint64_m*)&context->reg0[0]) >= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((int64_m*)&context->reg0[0]) >= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((int64_m*)&context->reg0[0]) >= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint8_m*)mem_loc) = (uint8_m)(*((double*)&context->reg0[0]) >= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int8_m*)mem_loc) = (int8_m)(*((double*)&context->reg0[0]) >= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((uint64_m*)&context->reg0[0]) >= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((uint64_m*)&context->reg0[0]) >= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((int64_m*)&context->reg0[0]) >= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((int64_m*)&context->reg0[0]) >= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint16_m*)mem_loc) = (uint16_m)(*((double*)&context->reg0[0]) >= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int16_m*)mem_loc) = (int16_m)(*((double*)&context->reg0[0]) >= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((uint64_m*)&context->reg0[0]) >= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((uint64_m*)&context->reg0[0]) >= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((int64_m*)&context->reg0[0]) >= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((int64_m*)&context->reg0[0]) >= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((uint64_m*)&context->reg0[0]) >= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((int64_m*)&context->reg0[0]) >= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint32_m*)mem_loc) = (uint32_m)(*((double*)&context->reg0[0]) >= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int32_m*)mem_loc) = (int32_m)(*((double*)&context->reg0[0]) >= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((float*)mem_loc) = (float)(*((double*)&context->reg0[0]) >= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
									switch (dst_dt.type) { default: break;
										case MUB_DATA_TYPE_INTEGER: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((uint64_m*)&context->reg0[0]) >= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((uint64_m*)&context->reg0[0]) >= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((int64_m*)&context->reg0[0]) >= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((int64_m*)&context->reg0[0]) >= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_UNSIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((uint64_m*)&context->reg0[0]) >= *((uint64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((int64_m*)&context->reg0[0]) >= *((int64_m*)&context->reg1[0])); return MU_SUCCESS; } break;
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
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_UNSIGNED: { *((uint64_m*)mem_loc) = (uint64_m)(*((double*)&context->reg0[0]) >= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
														case MUB_DATA_TYPE_SIGNED:   { *((int64_m*)mem_loc) = (int64_m)(*((double*)&context->reg0[0]) >= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
													}
												} break;
											}
										} break;
										case MUB_DATA_TYPE_DECIMAL: {
											switch (src0_dt.sign) { default: break;
												case MUB_DATA_TYPE_SIGNED: {
													switch (dst_dt.sign) { default: break;
														case MUB_DATA_TYPE_SIGNED:   { *((double*)mem_loc) = (double)(*((double*)&context->reg0[0]) >= *((double*)&context->reg1[0])); return MU_SUCCESS; } break;
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
