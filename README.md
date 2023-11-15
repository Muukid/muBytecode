# muBytecode

muBytecode (mub for short) is a public domain single-file C library for storing and executing bytecode. To use it, simply download the `muBytecode.h` file, add it to your include path, and include it like so:

```
#define MUB_IMPLEMENTATION
#include "muBytecode.h"
```

Note that not defining `MUB_IMPLEMENTATION` will define the functions, structs, and enums, but will not give an implementation for the functions, meaning that you can include it across multiple files without redefinition errors as long as you define `MUB_IMPLEMENTATION` once in your main file.

# Licensing

mub is licensed under public domain or MIT, whichever you prefer. More information is provided in the accompanying file `license.md` and at the bottom of `muBytecode.h`.

# General information

## General function syntax

Most mub functions have `muResult* result` as their first parameter. It's used to check if the function succeeded or failed. The function will dereference the given pointer as `MU_TRUE` or `MU_FALSE` to indicate so. `MU_NULL_PTR` can always be passed in if you decide to not check the result of the function.

# Macros

## Version

Near the top of the file, the version of mub is defined with `MUB_VERSION_MAJOR`, `MUB_VERSION_MINOR`, and `MUB_VERSION_PATCH`. These versions match the releases on the GitHub page and can be used to check if the correct version of mub is running.

## Static/Extern defines

Defining `MU_STATIC` before including `muBytecode.h` will set all the definitions of the functions to static. Otherwise, it defaults to extern. Additionally, you can define `MUDEF` directly to have full control over its definition.

## Quick compile

In order to quickly compile mub for basic testing purposes, define `MUB_QUICK_COMPILE` before including `muBytecode.h`. But note that this will exclude the modulo operator, the bitwise operators, and all conditional storing operators!

## C standard library dependencies

mub relies on several C standard library functions, types, and defines. The list of types can be found below:

`size_m`: equivalent to `size_t`, uses `stddef.h` for definition.

`uint64_m`: equivalent to `uint64_t`, uses `stdint.h` for definition.

`uint32_m`: equivalent to `uint32_t`, uses `stdint.h` for definition.

`uint16_m`: equivalent to `uint16_t`, uses `stdint.h` for definition.

`uint8_m`: equivalent to `uint8_t`, uses `stdint.h` for definition.

`int64_m`: equivalent to `int64_t`, uses `stdint.h` for definition.

`int32_m`: equivalent to `int32_t`, uses `stdint.h` for definition.

`int16_m`: equivalent to `int16_t`, uses `stdint.h` for definition.

`int8_m`: equivalent to `int8_t`, uses `stdint.h` for definition.

`wchar_m`: equivalent to `wchar_t`, uses `wchar.h` for definition.

The list of functions can be found below:

`mu_malloc`: equivalent to `malloc`, uses `stdlib.h` for definition.

`mu_realloc`: equivalent to `realloc`, uses `stdlib.h` for definition.

`mu_free`: equivalent to `free`, uses `stdlib.h` for definition.

`mu_memset`: equivalent to `memset`, uses `string of bytes.h` for definition.

`mu_memcpy`: equivalent to `memcpy`, uses `string of bytes.h` for definition.

`mu_strcmp`: equivalent to `strcmp`, uses `string of bytes.h` for definition.

`setlocale_m`: equivalent to `setlocale`, uses `locale.h` for definition.

The list of defines can be found below:

`PRIu8_m`: equivalent to `PRIu8`, uses `inttypes.h` for definition.

`PRId8_m`: equivalent to `PRId8`, uses `inttypes.h` for definition.

`PRIu16_m`: equivalent to `PRIu16`, uses `inttypes.h` for definition.

`PRId16_m`: equivalent to `PRId16`, uses `inttypes.h` for definition.

`PRIu32_m`: equivalent to `PRIu32`, uses `inttypes.h` for definition.

`PRId32_m`: equivalent to `PRId32`, uses `inttypes.h` for definition.

`PRIu64_m`: equivalent to `PRIu64`, uses `inttypes.h` for definition.

`PRId64_m`: equivalent to `PRId64`, uses `inttypes.h` for definition.

`MU_LC_ALL`: equivalent to `LC_ALL`, uses `locale.h` for definition.

Note that all of these macros can be overwritten by defining them before including `muBytecode.h`.

Note that overwriting all macros relating to a C standard library header file prevents it from being included. For example, if `setlocale_m` and `MU_LC_ALL` are overwritten, which are the only two macros that are used in relation to `locale.h`, then `locale.h` will not be included.

## Null macros

mub uses two macros for null values, `MU_NULL` and `MU_NULL_PTR`, who both are just 0.

## Boolean macros

There is one macro who is based on the `muBool` enum.

Defined below is `muResult`, used to determine the result of a task:

```
#define muResult   muBool
#define MU_FAILURE MU_FALSE
#define MU_SUCCESS MU_TRUE
```

## Print macros

There are two macros used to print, `mu_print` for printing just a string, and `mu_printf` for printing a formatted string of bytes. Both use `printf`, and thus have a dependency on `stdio.h`. If `MUB_NO_PRINT` is defined, these macros are set to do nothing, mub is guaranteed to never print any information, and `stdio.h` is never included. Additionally, `mu_print` and `mu_printf` can be overwritten by the user. If both are overwritten, `stdio.h` is not included.

## muByte

`muByte` is a macro for `unsigned char`.

# Enumerators

## Boolean values

There is one enum that is used for boolean values, `muBool`, defined below:

```
enum muBool { MU_FALSE, MU_TRUE };
typedef enum muBool muBool;
```

All other boolean enums are simply macros to `muBool`, covered in the **Boolean macros** section of **Macros**.

# Structs

## Data type

A data type in mub has a struct equivalent, `mubDataType`, defined below:

```
struct mubDataType {
	muBool sign;
	muByte type;
	muByte temp;
	muByte pointer_count;
	uint16_m byte_size;
};
typedef struct mubDataType mubDataType;
```

A more detailed description of what each of these mean is offered in the **Bytecode** section farther down this text.

## Variable

A variable in mub has a struct equivalent, `muVariable`, defined below:

```
struct muVariable {
	size_m address;
	mubDataType datatype;
	char* name;
};
typedef struct muVariable muVariable;
```

`address` refers to the address number that the variable is located at.

`datatype` refers to the data type of the variable.

`name` refers to the referencable name of the variable.

## Context

A mub context is defined as the struct '`muContext`', and is used to keep track of the context's state to properly execute the bytecode. Its definition will not be shown, as all of it is only used internally.

# Functions

## Context creation/destruction

### Context creation

The function `mu_context_create` is used to create a mub context, defined below:

```
MUDEF muContext mu_context_create(muResult* result, muByte* bytecode, size_m bytecode_len);
```

Note that this function is safe to use with a previously deleted context, as it will override all of its contents.

### Context destruction

The function `mu_context_destroy` is used to create a mub context, defined below:

```
MUDEF muContext mu_context_destroy(muResult* result, muContext context);
```

Note that the context returned is the version of the context that has been destroyed.

Note that it is safe to call this function numerous times, as long as the result is properly stored into the given context.

## Context execution

### Main function execution

The function `mu_context_execute_main` is used to execute the main function of a context, defined below:

```
MUDEF int mu_context_execute_main(muResult* result, muContext* context);
```

### Function execution

The function `mu_context_execute_function` is used to execute a particular function defined within a context, defined below:

```
MUDEF void mu_context_execute_function(muResult* result, muContext* context, size_m id);
```

Note that `id` can be retrieved from a named function with the function `mu_context_get_function_id`.

Note that this function *cannot* be used to execute the main function.

## Function information

### Getting a function ID

The function `mu_context_get_function_id` is used to find a named function's ID, defined below:

```
MUDEF size_m mu_context_get_function_id(muResult* result, muContext* context, const char* name);
```

## Getting and setting variables

### Get variable

The function `mu_context_get_variable` is used to locate a named variable in memory, defined below:

```
MUDEF muVariable mu_context_get_variable(muResult* result, muContext* context, const char* name);
```

### Get variable data

The function `mu_context_get_variable_data` is used to get the data of a variable, defined below:

```
MUDEF void mu_context_get_variable_data(muResult* result, muContext* context, muVariable variable, void* data);
```

Note that `data` is assumed to match the byte length of `variable`.

### Set variable data

The function `mu_context_set_variable_data` is used to set the data of a variable, defined below:

```
MUDEF void mu_context_set_variable_data(muResult* result, muContext* context, muVariable variable, void* data);
```

Note that `data` is assumed to match the byte length of `variable`.

## External functions

### Set external function

The function `mu_context_set_external_function` is used to set an external function to be associated with a particular name which can then be called in the context, defined below:

```
MUDEF void mu_context_set_external_function(muResult* result, muContext* context, const char* name, void(*function)(muContext* context));
```

# Bytecode

This is a full breakdown of how the bytecode format works.

## Demos

Demos of the bytecode in action can be found in the demos folder accompanying this file.

## Beginning/End headers

### Beginning header

The beginning header is used to identify if the bytecode isn't corrupted and to gather general information about how the bytecode should be interpreted. The beginning header is 16 bytes long.

The first four bytes are the mub file signature, simply the characters 'm', 'u', and 'b' in ASCII and then a fourth byte with nothing:

`01101101 01110101 01100010 00000000`

The next byte is used to determine the bytewidth of various aspects of the bytecode. It's split up into 4 different bytewidth definitions, in order from left to right being the memory address bytewidth, the jump marker ID bytewidth, the function ID bytewidth, and the external function ID bytewidth. Each one is interpreted in binary as `00` meaning 1 byte, `01` meaning 2 bytes, `10` meaning 4 bytes, and `11` meaning 8 bytes.

The next three bytes are the mub version that this was originally meant to execute on, in order from left to right being version major, minor, and patch. Each byte is a value between 0 and 255.

The next 4 bytes are an unsigned integer that determines how much static memory to allocate in bytes, ranging from 0 to 4294967295.

The next and final 4 bytes are an unsigned integer that determines how much temporary memory to allocate in bytes, ranging from 0 to 4294967295.

### End header

The end header is used to identify that the bytecode hasn't ended prematurely. It's simply 4 bytes that spell out 'e', 'n', and 'd' in ASCII, with the final byte being all 1's:

`01100101 01101110 01100100 11111111`

This end header will appear at the end of the bytecode.

## Memory

The memory in bytecode is split up into three sections: static, temporary (temp), and dynamic.

### Static

The static memory is the main memory that the program will use during execution. Its size is completely static, determined by the beginning header in advance. This memory can be freely accessed and modified at any given time, besides address 0, which is reserved to check for null pointer values.

### Temporary

The temp memory is the secondary memory that a program can use, whose size is completely static, determined by the beginning header in advance. This memory has no set defined purpose, but it's supposed to be used to store temporary values, such as the result of conditional operators.

### Dynamic

The dynamic memory is the memory used for dynamic allocation. Its default size is 0, and nothing is allocated on default, but its length can be modified internally as the program is executing. Its addresses sit on top of the static memory; address #n in the dynamic memory is accessed by address #static memory's length + n.

## Data type

### Data type byte explanation

A 'data type' in bytecode is a 3-byte specifier as to how a given value/address should be interpreted at runtime.

The first byte's first bit (starting from the left and on) determines its signed status, with `0` representing unsigned and `1` representing signed.

The next two bits represent the type of value stored, with `00` meaning void (no associated type), `01` meaning struct (acts the same as void type), `10` meaning integer, and `11` meaning decimal.

The next bit is used for addresses, and determines whether or not the given address is stored within the static memory buffer or the temp memory buffer, with `0` meaning static and `1` meaning temp.

The next 4 bits determine how many times the value should be treated as an address and dereferenced, ranging from 0 to 15. "Dereferenced" in this context means that the value following the data type is an address, and the value can be found by finding the value stored at the address that it's indicating, and these 4 bits determine how many times it treats the value like an address and dereferences it. Note that if this value is above 0, all of the other information stored in the data type (such as byte size, type, and others) is ignored when reading the value initially, and will only be considered when the value has been fully dereferenced. ***The actual value that follows a data type will be assumed to be an unsigned integer whose byte size matches that of the memory address byte size specified in the beginning header if its dereference count is over 0.***

The next and final 2 bytes determine the byte size of the data type, ranging from 0 to 65535.

### "Invalid" data type

There is such a thing as an "invalid" data type, which will cause any command that it's entered into to fail. The "rule set" for a valid data type follows as such:

1. If the data type is an integer, then its byte size has to be rather 1, 2, 4, or 8.
2. If the data type is a decimal, then it has to be signed.
3. If the data type is a decimal, then its byte size has to be rather 4 or 8.

## Functions

### Function declaration

A function in bytecode is declared like so:

`0xE0 ID ... 0xE1`

0xE0 indicates the beginning of the function declaration, 'ID' is an function ID whose bytesize matches that specified in the beginning file header, all the code stored within '...' is the function's respective code that will be executed each time the function is called, and '0xE1' marks the end of the function declaration.

Within the function, `0xE2` can be used to end the function early if executed, but doesn't end the function declaration. It can be thought of as a blank 'return' that halts function execution instantly.

Note that functions in bytecode do not have return values or parameters. All respective functionality must be replicated manually!

Note that functions are most ideally ordered in the bytecode starting from 1 and increasing by 1 for each function, with the last function being #0, which is the main function. If this order is not kept, the program will still execute properly, but most likely a tid bit slower.

### The main function

The main function is declared like a normal function, except for with the ID #0. This means that in bytecode, the function with ID #0 is always assumed to be the "main function".

The main function has the special privilege of being able to use `0x00` to return a value to the context and instantly halt all program execution. It's expected to be followed by a data type and corresponding value.

### Function calling

A function is called by the value `0xE3`, followed by a data type and corresponding value that indicates what function ID to execute.

### Calling external functions

An external function can be called with the value `0xE4`, followed by a data type and corresponding value that indicates that external function ID to execute.

## Naming

There are multiple aspects of the bytecode that are nameable for the program that it's running in to be more easily able to observe and modify particular parts of the program. Note that all of the following commands in this section are non-executed, meaning that they're applied as the bytecode is scanned, not as it is ran, which also means that these names are static and unchanging, and that they don't have to appear within a function to be executed.

### Function names

A function ID can be associated with a name with `0x10`, followed by a function ID and its name, a null-terminated string of bytes.

### Variable names

A section of memory, AKA a variable, can be associated with a name with `0x11`, followed by the memory address, its data type, and its name, a null-terminated string of bytes.

### External function names

An external function ID can be associated with a name with `0x12`, followed by its external function ID, and its name, a null-terminated string of bytes.

## Conditionals

Conditionals change the control flow of the program based on whether a given value is *false*, AKA 0, or *true*, AKA any non-zero value.

### If conditional

An if conditional specifies a section of code to be executed only if a given value is true, declared like so:

`0xA0 datatype value ... 0xA1`

All the code within '`...`' will only be executed if the given value is true.

### Else conditional

An else conditional specifies a section of code to be executed only if the previous if conditional failed, declared like so:

`0xA2 ... 0xA1`

Note that the conditional will only be considered if it comes right after its related if conditional, like so:

`0xA0 datatype value ... 0xA1 0xA2 ... 0xA1`

## Jump markers

Jump markers are used to be able to jump from one part of the code to another.

A jump marker is declared with `0xF0`, followed by its jump marker ID. It can then be jumped to at any given moment with `0xF1`, followed by the jump marker ID.

## Commands

### Command terminology

An 'executed' command means that the command is considered when being manually executed by the interpreter. A 'non-executed' command means that it's fully considered when being initially scanned by the interpreter, and ignored during execution.

'dt' is short for data type, 'val' is short for value, 'src' is short for source, and 'dst' is short for destination.

The difference between a source value and a destination value is that the destination value is interpreted as a place to store the result of the operation being performed on the source value(s). Note that the destination value is always assumed to be an address, even if the dereference count of the corresponding data type is zero. In this case, the destination data type is only describing what the result of the operation should be casted to, and ***the destination value is always assumed to be an address, an unsigned integer of memory address byte length***. It can still be dereferenced numerous times via the data type, but the final type is always assumed to be an address.

### List of all commands

#### 0x0_ - miscellaneous special commands

**0x00 - return main (src_dt, src_val)** : An executed command that sets the return value of the main function to the given value, casting it to a signed memory-address-bytewidth integer, and instantly ending all further program execution.

Note that this command can only be called within the main function.

**0x01 - print (src_dt, src_val, spec)** : An executed command that prints the given source value to the console. 'spec' is a byte whose first two bits starting from the left indicate how to interpret the source value when printing, with `00` printing it as an integer, `01` as decimal, `10` as character, and `11` as raw binary. The other 6 bits are treated as an unsigned integer that indicates decimal precision, from 0 to 63, although it cannot be reliably expected to print anything over 6-10 decimals.

**0x02 - set dynamic buffer size (src_dt, src_val)** : An executed command that sets the dynamic memory buffer size to the given source value in bytes.

Note that data will only be preserved if size is increased or stays the same.

#### 0x1_ - names

**0x10 - set function name (id, name)** : A non-executed command that sets a name to be associated with a function ID. 'id' is a function ID whose size is determined in the beginning header, and 'name' is a null-terminated string of bytes.

Note that if a function ID is repeated, only its first instance is considered.

Note that this command can be called before a function with its ID is declared, as long as the function is declared at some point.

**0x11 - set variable name (address, datatype, name)** : A non-executed command that sets a name to be associated with a point in memory. ‘address’ is a memory address whose size matches the bytesize specified in the beginning-header, ‘datatype’ is the data type associated with that address, and ‘name’ is a null-terminated string of bytes.

**0x12 - set external function name (id, name)** : A non-executed command that sets an external function name to be associated with an external function ID. 'id' is an external function ID whose size is determined in the beginning header, and 'name' is a null-terminated string of bytes.

#### 0x8_ - numerical/assignment operators

**0x80 - move (src_dt, src_val, dst_dt, dst_val)** : An executed command that moves the given source value to the destination address.

**0x81 - add (src0_dt, src0_val, src1_dt, src1_val, dst_dt, dst_val)** : An executed command that adds the two source values together and stores the result in the destination address.

**0x82 - subtract (src0_dt, src0_val, src1_dt, src1_val, dst_dt, dst_val)** : An executed command that subtracts source value 1 from source value 0 and stores the result in the destination address.

**0x83 - multiply (src0_dt, src0_val, src1_dt, src1_val, dst_dt, dst_val)** : An executed command that multiplies the two source values together and stores the result in the destination address.

**0x84 - divide (src0_dt, src0_val, src1_dt, src1_val, dst_dt, dst_val)** : An executed command that divides source value 1 by source value 2 and stores the result in the destination address.

**0x85 - modulo (src0_dt, src0_val, src1_dt, src1_val, dst_dt, dst_val)** : An executed command that divides source value 1 by source value 2 and stores the remainder in the destination address.

**0x86 - bitwise NOT (src_dt, src_val, dst_dt, dst_val)** : An executed command that performs a bitwise NOT operation on the source value and stores the result in the destination address.

**0x87 - bitwise AND (src0_dt, src0_val, src1_dt, src1_val, dst_dt, dst_val)** : An executed command that performs a bitwise AND operation between the two source values and stores the result in the destination address.

**0x88 - bitwise OR (src0_dt, src0_val, src1_dt, src1_val, dst_dt, dst_val)** : An executed command that performs a bitwise OR operation between the two source values and stores the result in the destination address.

**0x89 - bitwise XOR (src0_dt, src0_val, src1_dt, src1_val, dst_dt, dst_val)** : An executed command that performs a bitwise XOR operation between the two source values and stores the result in the destination address.

**0x8A - bitwise left shift (src0_dt, src0_val, src1_dt, src1_val, dst_dt, dst_val)** : An executed command that shifts source value 0 by source value 1 bits left and stores the result in the destination address.

**0x8B - bitwise right shift (src0_dt, src0_val, src1_dt, src1_val, dst_dt, dst_val)** : An executed command that shifts source value 0 by source value 1 bits right and stores the result in the destination address.

#### 0x9_ - conditional storing

**0x90 - equal (src0_dt, src0_val, src1_dt, src1_val, dst_dt, dst_val)** : An executed command that stores whether or not source value 1 and source value 2 equal each other in the destination address.

**0x91 - not equal (src0_dt, src0_val, src1_dt, src1_val, dst_dt, dst_val)** : An executed command that stores whether or not source value 1 and source value 2 don't equal each other in the destination address.

**0x92 - less (src0_dt, src0_val, src1_dt, src1_val, dst_dt, dst_val)** : An executed command that stores whether or not source value 1 is less than source value 2 in the destination address.

**0x93 - less equal (src0_dt, src0_val, src1_dt, src1_val, dst_dt, dst_val)** : An executed command that stores whether or not source value 1 is less than or equal to source value 2 in the destination address.

**0x94 - greater (src0_dt, src0_val, src1_dt, src1_val, dst_dt, dst_val)** :  An executed command that stores whether or not source value 1 is greater than source value 2 in the destination address.

**0x95 - greater equal (src0_dt, src0_val, src1_dt, src1_val, dst_dt, dst_val)** : An executed command that stores whether or not source value 1 is greater than or equal to source value 2 in the destination address.

#### 0xA_ - conditional control flow

**0xA0 src_dt src_val ... 0xA1 - if conditional** : An executed control flow command that executes all of the code in '...' if source value 1 is not zero.

**0xA2 ... 0xA1 - else conditional** : An executed control flow command that executes all of the code in '...' if the previous if conditional didn't execute.

Note that this must come directly after the if conditional, like "`0xA0 src_dt src_val ... 0xA1 0xA2 ... 0xA1`". If any other commands are put between 0xA1 and 0xA2, the conditional will not work properly.

#### 0xE_ - function declaration and execution

**0xE0 id ... 0xE1 - function declaration** : A non-executed command that stores all the code in '...' and associates it with the given id. 'id' is a function ID whose size is determined in the beginning header.

**0xE2 - function return** : An executed command that ends a function early, instantly returning back to where the function was called.

Note that this command can only be called within a non-main function.

**0xE3 - function call (src_dt, src_val)** : An executed command that calls a function, whose id is specified in the source value.

**0xE4 - external function call (src_dt, src_val)** : An executed command that calls an external function, whose id is specified in the source value.

#### 0xF_ - jump markers

**0xF0 - jump marker declaration (id)** : A non-executed command that associates a point in code with an id. 'id' is a jump marker ID whose size is determined in the beginning header.

**0xF1 - jump (id)** : An executed command that jumps to a given jump marker, identified by the id. 'id' is a jump marker ID whose size is determined in the beginning header.
