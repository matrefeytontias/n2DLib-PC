#pragma once

#include "n2DLib/n2DLib_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define itofix(x) ((x) << N2DLIB_FIXED_BITS)
#define fixtoi(x) ((x) >> N2DLIB_FIXED_BITS)

#if !defined(min) && !defined(max)

// Compute the minimum of two values.
INT_TYPE min(INT_TYPE a, INT_TYPE b);
// Compute the maximum of two values.
INT_TYPE max(INT_TYPE a, INT_TYPE b);

#endif

// Keep a value between two bounds.
INT_TYPE clamp(INT_TYPE x, INT_TYPE a, INT_TYPE b);
// 
INT_TYPE sign(INT_TYPE x);
INT_TYPE sq(INT_TYPE x);
INT_TYPE cube(INT_TYPE x);
UINT_TYPE isqrt(UINT_TYPE x);

Fixed fixmul(Fixed x, Fixed y);
Fixed fixdiv(Fixed x, Fixed y);
Fixed fixcos(Fixed angle);
Fixed fixsin(Fixed angle);
Fixed fixsq(Fixed x);
Fixed fixcube(Fixed x);
Fixed fixlerp(Fixed a, Fixed b, Fixed x);

void rotate(INT_TYPE x, INT_TYPE y, INT_TYPE cx, INT_TYPE cy, Fixed angle, Rect* out);
void getBoundingBox(INT_TYPE x, INT_TYPE y, INT_TYPE w, INT_TYPE h, INT_TYPE cx, INT_TYPE cy, Fixed angle, Rect* out);

#ifdef __cplusplus
}
#endif
