#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// For booleans
typedef uint8_t BOOL_TYPE;
// For counting sets of few things
typedef uint8_t ID_TYPE;
// For general math
typedef int32_t INT_TYPE;
// For general math but unsigned
typedef uint32_t UINT_TYPE;

#ifdef PIXEL_TYPE_RGB565
// For pixel storage and usage
typedef uint16_t PIXEL_TYPE;
#else
#error Please define a pixel type for the current PIXEL_TYPE_* constant.
#endif

#ifndef N2DLIB_FIXED_BITS
// Define this on your own to control the format
// of the fixed-point type.
#define N2DLIB_FIXED_BITS 8
#endif

// Fixed point numbers. The position of the fixed point
// is controlled by the N2DLIB_FIXED_BITS define. If
// undefined, defaults to 24.8 fixed point.
typedef INT_TYPE Fixed;

typedef struct
{
	INT_TYPE x, y, w, h;
} Rect;

#ifdef __cplusplus
}
#endif
