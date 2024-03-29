#include "n2DLib/n2DLib_math.h"

#if !defined(min) && !defined(max)

INT_TYPE min(INT_TYPE a, INT_TYPE b)
{
	return a < b ? a : b;
}

INT_TYPE max(INT_TYPE a, INT_TYPE b)
{
	return a > b ? a : b;
}

#undef IMPL_MIN_MAX
#endif

INT_TYPE clamp(INT_TYPE x, INT_TYPE a, INT_TYPE b)
{
	return min(max(x, a), b);
}

INT_TYPE sign(INT_TYPE x)
{
	return (x > 0) - (x < 0);
}

INT_TYPE sq(INT_TYPE x)
{
	return x * x;
}

INT_TYPE cube(INT_TYPE x)
{
	return x * x * x;
}

// Adapted from https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Binary_numeral_system_(base_2)
// with ideas from https://graphics.stanford.edu/~seander/bithacks.html
UINT_TYPE isqrt(UINT_TYPE x)
{
	uint32_t c = 0;

	// Find the highest power of 4 smaller than x.
	// First, the smallest power of 2 bigger than x.
	uint32_t d = x - 1;
	d |= d >> 1;
	d |= d >> 2;
	d |= d >> 4;
	d |= d >> 8;
	d |= d >> 16;

	d++;
	// If it is a power of 4, divide by 4, otherwise by 2.
	d >>= 1 + ((d & 0xAAAAAAAA) == 0);

	while (d != 0)
	{
		if (x >= c + d)
		{
			x -= c + d;
			c = (c >> 1) + d;
		}
		else
			c >>= 1;
		d >>= 2;
	}

	return c;
}

Fixed fixmul(Fixed x, Fixed y)
{
	// x = (xint << N2DLIB_FIXED_BITS) + xdec, y = (yint << N2DLIB_FIXED_BITS) + ydec
	Fixed xdec = x & 0xff, ydec = y & 0xff, xint = x >> N2DLIB_FIXED_BITS, yint = y >> N2DLIB_FIXED_BITS;
	// Like (x * y) >> N2DLIB_FIXED_BITS ; a bit slower but without any risk of overflow (noticeable when squaring and cubing)
	return ((xint * yint) << N2DLIB_FIXED_BITS) + xint * ydec + xdec * yint + ((xdec * ydec) >> N2DLIB_FIXED_BITS);
}

Fixed fixdiv(Fixed x, Fixed y)
{
	// TODO : prevent overflow ?
	return (x << N2DLIB_FIXED_BITS) / y;
}

Fixed fixcos(Fixed angle)
{
	Fixed const cosLUT[] = { 256, 255, 255, 255, 254, 254, 253, 252, 251, 249, 248, 246, 244, 243, 241, 238, 236, 234, 231, 228, 225, 222, 219, 216, 212, 209, 205, 201, 197, 193, 189, 185, 181, 176, 171, 167, 162, 157, 152, 147, 142, 136, 131, 126, 120, 115, 109, 103, 97, 92, 86, 80, 74, 68, 62, 56, 49, 43, 37, 31, 25, 18, 12, 6, 0, -6, -12, -18, -25, -31, -37, -43, -49, -56, -62, -68, -74, -80, -86, -92, -97, -103, -109, -115, -120, -126, -131, -136, -142, -147, -152, -157, -162, -167, -171, -176, -181, -185, -189, -193, -197, -201, -205, -209, -212, -216, -219, -222, -225, -228, -231, -234, -236, -238, -241, -243, -244, -246, -248, -249, -251, -252, -253, -254, -254, -255, -255, -255, -256, -255, -255, -255, -254, -254, -253, -252, -251, -249, -248, -246, -244, -243, -241, -238, -236, -234, -231, -228, -225, -222, -219, -216, -212, -209, -205, -201, -197, -193, -189, -185, -181, -176, -171, -167, -162, -157, -152, -147, -142, -136, -131, -126, -120, -115, -109, -103, -97, -92, -86, -80, -74, -68, -62, -56, -49, -43, -37, -31, -25, -18, -12, -6, 0, 6, 12, 18, 25, 31, 37, 43, 49, 56, 62, 68, 74, 80, 86, 92, 97, 103, 109, 115, 120, 126, 131, 136, 142, 147, 152, 157, 162, 167, 171, 176, 181, 185, 189, 193, 197, 201, 205, 209, 212, 216, 219, 222, 225, 228, 231, 234, 236, 238, 241, 243, 244, 246, 248, 249, 251, 252, 253, 254, 254, 255, 255, 255 };
	return cosLUT[angle & 0xff];
}

Fixed fixsin(Fixed angle)
{
	return fixcos(angle - 64);
}

Fixed fixsq(Fixed x)
{
	return fixmul(x, x);
}

Fixed fixcube(Fixed x)
{
	return fixmul(fixmul(x, x), x);
}

Fixed fixlerp(Fixed a, Fixed b, Fixed x)
{
	return a + fixmul(b - a, x);
}

void rotate(INT_TYPE x, INT_TYPE y, INT_TYPE cx, INT_TYPE cy, Fixed angle, Rect* out)
{
	x -= cx;
	y -= cy;
	out->x = fixtoi(fixmul(itofix(x), fixcos(angle)) + fixmul(itofix(y), fixsin(angle))) + cx;
	out->y = fixtoi(fixmul(itofix(x), -fixsin(angle)) + fixmul(itofix(y), fixcos(angle))) + cy;
}

void getBoundingBox(INT_TYPE x, INT_TYPE y, INT_TYPE w, INT_TYPE h, INT_TYPE cx, INT_TYPE cy, Fixed angle, Rect* out)
{
	Rect tl, tr, bl, br;
	rotate(x, y, cx, cy, angle, &tl);
	rotate(x + w, y, cx, cy, angle, &tr);
	rotate(x, y + h, cx, cy, angle, &bl);
	rotate(x + w, y + h, cx, cy, angle, &br);
	out->x = min(min(min(tl.x, tr.x), bl.x), br.x);
	out->y = min(min(min(tl.y, tr.y), bl.y), br.y);
	out->w = max(max(max(tl.x, tr.x), bl.x), br.x) - out->x;
	out->h = max(max(max(tl.y, tr.y), bl.y), br.y) - out->y;
}
