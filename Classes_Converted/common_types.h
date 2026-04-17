#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <cstdint>
#include <cstddef>

// Basic type definitions for Revenant class structures
// These match the original Ghidra analysis types

// Unsigned integer types
typedef uint8_t  byte;    // Unsigned Byte (db)
typedef uint16_t word;    // Unsigned Word (dw)
typedef uint16_t ushort;  // Unsigned Short (common alias)
typedef uint32_t dword;   // Unsigned Double-Word (ddw, 4-bytes)
typedef uint64_t qword;   // Unsigned Quad-Word (8-bytes)
typedef uint32_t uint;    // Unsigned 32-bit integer (common alias)

// Note: char, short, int, long, float, double, and bool are built-in C++ types
// and cannot be redefined. Use them directly.

// Pointer types (for void* references)
typedef void*    ptr;     // Generic pointer

// Undefined types from Ghidra analysis - these are conservative guesses
// that can be easily corrected later if we determine their actual types
typedef uint8_t  undefined1;   // 1-byte undefined type
typedef uint16_t undefined2;   // 2-byte undefined type  
typedef uint32_t undefined4;   // 4-byte undefined type
typedef uint64_t undefined8;   // 8-byte undefined type
typedef uint32_t undefined;    // Default undefined type (assumed 4-byte)

// Common size constants
#define BYTE_SIZE    1
#define WORD_SIZE    2
#define DWORD_SIZE   4
#define QWORD_SIZE   8

// Common alignment constants
#define BYTE_ALIGN   1
#define WORD_ALIGN   2
#define DWORD_ALIGN  4
#define QWORD_ALIGN  8

// Common bit masks
#define BYTE_MASK    0xFF
#define WORD_MASK    0xFFFF
#define DWORD_MASK   0xFFFFFFFF
#define QWORD_MASK   0xFFFFFFFFFFFFFFFF

#endif // COMMON_TYPES_H 