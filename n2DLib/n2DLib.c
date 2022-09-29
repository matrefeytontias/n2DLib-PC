#include "n2DLib.h"

#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "n2DLib_font.h"
#include "n2DLib/n2DLib_math.h"

/*             *
 *  Buffering  *
 *             */

PIXEL_TYPE *BUFF_BASE_ADDRESS;
SDL_Window *sdlWindow;
SDL_Renderer *sdlRenderer;
SDL_Texture *MAIN_SCREEN;

Uint32 baseFPS;

#ifdef PIXEL_TYPE_RGB565
#define BACKEND_PIXEL_FORMAT SDL_PIXELFORMAT_RGB565
#else
#error Please define a backend pixel format for the current PIXEL_TYPE_* constant.
#endif

void n2D_init()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	
	// This fixes the fullscreen/resize crash, see n2D_updateScreen
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
	
	SDL_CreateWindowAndRenderer(320 * 2, 240 * 2, SDL_WINDOW_BORDERLESS, &sdlWindow, &sdlRenderer);
	SDL_RenderSetLogicalSize(sdlRenderer, 320, 240);
	if(!sdlWindow || !sdlRenderer)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Initialisation error", "Error when initialising SDL2, check stdout.txt for details.", NULL);
		printf("Couldn't initialise SDL2 : %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
	MAIN_SCREEN = SDL_CreateTexture(sdlRenderer, BACKEND_PIXEL_FORMAT, SDL_TEXTUREACCESS_STREAMING, 320, 240);

	// TODO : look into SDL_LockTexture on MAIN_SCREEN
	BUFF_BASE_ADDRESS = (PIXEL_TYPE*)malloc(320 * 240 * sizeof(PIXEL_TYPE));
	memset(BUFF_BASE_ADDRESS, 0, sizeof(BUFF_BASE_ADDRESS));
	
	baseFPS = SDL_GetTicks();
}

void n2D_deinit()
{
	SDL_DestroyTexture(MAIN_SCREEN);
	SDL_DestroyRenderer(sdlRenderer);
	SDL_DestroyWindow(sdlWindow);
	SDL_Quit();
	free(BUFF_BASE_ADDRESS);
}

void toggleFullscreen()
{
	if(SDL_GetWindowFlags(sdlWindow) & SDL_WINDOW_MAXIMIZED)
		SDL_RestoreWindow(sdlWindow);
	else
		SDL_MaximizeWindow(sdlWindow);
}

void n2D_constrainFrameRate(INT_TYPE fps)
{
	static Uint32 secondCount = 1001, secondBase = 0, FPScount = 0, FPSdisp = 0;
	Uint32 d = 1000 / fps;
	Uint32 elapsed = SDL_GetTicks() - baseFPS;
	if(elapsed < d)
		SDL_Delay(d - elapsed);
	baseFPS = SDL_GetTicks();
}

void n2D_displayFrameRate()
{
	static Uint32 secondCount = 1001, secondBase = 0, FPScount = 0, FPSdisp = 0;
	FPScount++;
	INT_TYPE x, y;
	secondCount = SDL_GetTicks();
	if (secondCount - secondBase > 1000)
	{
		FPSdisp = FPScount;
		FPScount = 0;
		secondCount = 0;
		secondBase = SDL_GetTicks();
	}
	x = 150, y = 230;
	n2D_drawDecimal(&x, &y, FPSdisp, 0xffff, 0);
}

void n2D_updateScreen()
{
	static BOOL_TYPE toggled = 0;
	UINT_TYPE di;
	uint8_t *pixels;
	uint8_t *buf = (uint8_t*)BUFF_BASE_ADDRESS;
	INT_TYPE pitch;
	SDL_LockTexture(MAIN_SCREEN, NULL, (void**)&pixels, &pitch);
	for (di = 0; di < 320 * 240 * sizeof(PIXEL_TYPE); di += sizeof(UINT_TYPE))
		*(UINT_TYPE*)(pixels + di) = *(UINT_TYPE*)(buf + di);
	SDL_UnlockTexture(MAIN_SCREEN);
	
	SDL_RenderCopy(sdlRenderer, MAIN_SCREEN, NULL, NULL);
	SDL_RenderPresent(sdlRenderer);
}

/*        *
 * Timers *
 *        */

// Timers decrease from a given value to 0, and stop until a new value is loaded in them

Uint32 timerBase[MAX_TIMER], timerValue[MAX_TIMER];

void n2D_timerLoad(ID_TYPE timer, UINT_TYPE value) // milliseconds
{
	timerValue[timer] = value;
	timerBase[timer] = SDL_GetTicks();
}

Uint32 n2D_timerRead(ID_TYPE timer) // returns milliseconds
{
	return timerValue[timer] - min(SDL_GetTicks() - timerBase[timer], timerValue[timer]);
}

/*            *
 *  Graphics  *
 *            */

void n2D_clearBufferB()
{
	INT_TYPE i;
	for(i = 0; i < 160 * 240; i++)
		((UINT_TYPE*)BUFF_BASE_ADDRESS)[i] = 0;
}

void n2D_clearBufferW()
{
	INT_TYPE i;
	for(i = 0; i < 160 * 240; i++)
		((UINT_TYPE*)BUFF_BASE_ADDRESS)[i] = 0xffffffff;
}

void n2D_clearBuffer(PIXEL_TYPE c)
{
	INT_TYPE i;
	UINT_TYPE ci = (c << 16) | c;
	for(i = 0; i < 160 * 240; i++)
		*((UINT_TYPE*)BUFF_BASE_ADDRESS + i) = ci;
}

PIXEL_TYPE n2D_rgbToPixel(uint8_t r, uint8_t g, uint8_t b)
{
#ifdef PIXEL_TYPE_RGB565
	return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
#else
#error Please define a function body for n2D_rgbToPixel for the current PIXEL_TYPE_* constant.
#endif
}

PIXEL_TYPE n2D_getPixelUnsafe(const PIXEL_TYPE *src, UINT_TYPE x, UINT_TYPE y)
{
	return src[x + y * src[0] + 3];
}

PIXEL_TYPE n2D_getPixel(const PIXEL_TYPE *src, UINT_TYPE x, UINT_TYPE y)
{
	if(x < src[0] && y < src[1])
		return src[x + y * src[0] + 3];
	else
		return src[2];
}

void n2D_setPixelUnsafe(UINT_TYPE x, UINT_TYPE y, PIXEL_TYPE c)
{
	*((PIXEL_TYPE*)BUFF_BASE_ADDRESS + x + y * 320) = c;
}

void n2D_setPixel(UINT_TYPE x, UINT_TYPE y, PIXEL_TYPE c)
{
	if(x < 320 && y < 240)
		*((PIXEL_TYPE*)BUFF_BASE_ADDRESS + x + y * 320) = c;
}

void n2D_drawHLine(INT_TYPE y, INT_TYPE x1, INT_TYPE x2, PIXEL_TYPE c)
{
	UINT_TYPE _x1, _x2;
	if((x1 & x2) < 0 || x1 + x2 >= 640 || (UINT_TYPE)y > 239)
	{
		return;
	}
	
	if(x1 < x2)
	{
		_x1 = max(x1, 0);
		_x2 = min(x2, 319);
	}
	else
	{
		_x1 = max(x2, 0);
		_x2 = min(x1, 319);
	}
	for(; _x1 <= _x2; _x1++)
		n2D_setPixelUnsafe(_x1, y, c);
}

void n2D_drawVLine(INT_TYPE x, INT_TYPE y1, INT_TYPE y2, PIXEL_TYPE c)
{
	UINT_TYPE _y1, _y2;
	if((y1 & y2) < 0 || y1 + y2 >= 480 || (UINT_TYPE)x > 319)
	{
		return;
	}
	
	if(y1 < y2)
	{
		_y1 = max(y1, 0);
		_y2 = min(y2, 239);
	}
	else
	{
		_y1 = max(y2, 0);
		_y2 = min(y1, 239);
	}
	for(; _y1 <= _y2; _y1++)
		n2D_setPixelUnsafe(x, _y1, c);
}

void n2D_fillRect(INT_TYPE x, INT_TYPE y, INT_TYPE w, INT_TYPE h, PIXEL_TYPE c)
{
	UINT_TYPE _x = max(x, 0), _y = max(y, 0), _w = min(320 - _x, w - _x + x), _h = min(240 - _y, h - _y + y), i, j;
	if(_x < 320 && _y < 240)
	{
		for(j = _y; j < _y + _h; j++)
			for(i = _x; i < _x + _w; i++)
				n2D_setPixelUnsafe(i, j, c);
	}
}

void n2D_drawSprite(const PIXEL_TYPE *src, INT_TYPE _x, INT_TYPE _y, BOOL_TYPE flash, PIXEL_TYPE flashColor)
{
	INT_TYPE x, y, w = src[0] + _x, h = src[1] + _y, c = 3;
	for(y = _y; y < h; y++)
	{
		for(x = _x; x < w; x++, c++)
		{
			if(src[c] != src[2])
				n2D_setPixel(x, y, flash ? flashColor : src[c]);
		}
		if(y > 239) break;
	}
}

void n2D_drawSpritePart(const PIXEL_TYPE *src, INT_TYPE _x, INT_TYPE _y, const Rect* part, BOOL_TYPE flash, PIXEL_TYPE flashColor)
{
	PIXEL_TYPE c;
	INT_TYPE x, y, w = part->w + _x, h = part->h + _y, z = part->x, t = part->y;
	for(y = _y; y < h; y++, t++)
	{
		for(x = _x, z = part->x; x < w; x++, z++)
		{
			c = n2D_getPixel(src, z, t);
			if(c != src[2])
				n2D_setPixel(x, y, flash ? flashColor : c);
			if(x > 319) break;
		}
		if(y > 239) break;
	}
}

void n2D_drawSpriteScaled(const PIXEL_TYPE* source, const Rect* info, BOOL_TYPE flash, PIXEL_TYPE flashColor)
{
	Fixed dx = itofix(source[0]) / info->w;
	Fixed dy = itofix(source[1]) / info->h;
	INT_TYPE x, y, _x = info->x + info->w / 2, _y = info->y + info->h / 2;
	Fixed tx = 0, ty = 0;
	PIXEL_TYPE c;
	
	for(y = info->y - info->h / 2; y < _y; y++, ty += dy)
	{
		for(x = info->x - info->w / 2, tx = 0; x < _x; x++, tx += dx)
		{
			c = n2D_getPixel(source, fixtoi(tx), fixtoi(ty));
			if(c != source[2])
				n2D_setPixel(x, y, flash ? flashColor : c);
			if(x > 319) break;
		}
		if(y > 239) break;
	}
}

void n2D_drawSpriteRotated(const PIXEL_TYPE* source, const Rect* sr, const Rect* rc, Fixed angle, BOOL_TYPE flash, PIXEL_TYPE flashColor)
{
	Rect defaultRect = { source[0] / 2, source[1] / 2, 0, 0 };
	Rect fr;
	PIXEL_TYPE currentPixel;
	Fixed dX = fixcos(angle), dY = fixsin(angle);
	
	if(rc == NULL)
		rc = &defaultRect;
	
	getBoundingBox(-rc->x, -rc->y, source[0], source[1], 0, 0, angle, &fr);
	fr.x += sr->x;
	fr.y += sr->y;
	fr.w += fr.x;
	fr.h += fr.y;
	
	Rect cp, lsp, cdrp;
	
	// Feed fixed-point to get fixed-point
	rotate(itofix(fr.x - sr->x), itofix(fr.y - sr->y), 0, 0, -angle, &lsp);
	
	for(cp.y = fr.y; cp.y <= fr.h; cp.y++)
	{
		cdrp.x = lsp.x;
		cdrp.y = lsp.y;
		
		for(cp.x = fr.x; cp.x <= fr.w; cp.x++)
		{
			if(cp.x >= 0 && cp.x < 320 && cp.y >= 0 && cp.y < 240)
			{
				currentPixel = n2D_getPixel(source, fixtoi(cdrp.x) + rc->x, fixtoi(cdrp.y) + rc->y);
				if(currentPixel != source[2])
				{
					n2D_setPixelUnsafe(cp.x, cp.y, flash ? flashColor : currentPixel);
				}
			}
			cdrp.x += dX;
			cdrp.y += dY;
		}
		
		lsp.x -= dY;
		lsp.y += dX;
	}
}

/*            *
 *  Geometry  *
 *            */
 
void n2D_drawLine(INT_TYPE x1, INT_TYPE y1, INT_TYPE x2, INT_TYPE y2, PIXEL_TYPE c)
{
	INT_TYPE dx = abs(x2 - x1);
	INT_TYPE dy = abs(y2 - y1);
	INT_TYPE sx = (x1 < x2) ? 1 : -1;
	INT_TYPE sy = (y1 < y2) ? 1 : -1;
	INT_TYPE err = dx - dy;
	INT_TYPE e2;

	while (!(x1 == x2 && y1 == y2))
	{
		n2D_setPixel(x1, y1, c);
		e2 = 2 * err;
		if (e2 > -dy)
		{		 
			err = err - dy;
			x1 = x1 + sx;
		}
		if (e2 < dx)
		{		 
			err = err + dx;
			y1 = y1 + sy;
		}
	}
}

// TODO : get rid of malloc/free
void n2D_drawPolygon(PIXEL_TYPE c, INT_TYPE pointsNb, ...)
{
	// the number of arguments in the <...> must be even
	INT_TYPE i;
	INT_TYPE *pointsList = (INT_TYPE *)malloc(pointsNb * 2 * sizeof(INT_TYPE));
	
	if (!pointsList) return;
	
	va_list ap;
	INT_TYPE cur_arg = 1;

	va_start(ap, pointsNb);
	
	for (i = 0; i < pointsNb * 2; i++)
	{
		cur_arg = va_arg(ap, INT_TYPE);
		*(pointsList + i) = cur_arg;
	}
	
	for (i = 0; i < pointsNb * 2 - 2; i += 2)
	{
		n2D_drawLine(*(pointsList + i), *(pointsList + i + 1), *(pointsList + i + 2), *(pointsList + i + 3), c);
	}
	n2D_drawLine(*(pointsList + pointsNb * 2 - 2), *(pointsList + pointsNb * 2 - 1), *(pointsList), *(pointsList + 1), c);
	va_end(ap);
	free(pointsList);
}

// TODO : do better than that
void n2D_fillCircle(INT_TYPE x, INT_TYPE y, INT_TYPE radius, PIXEL_TYPE c)
{
	INT_TYPE i,j;
	for(j=-radius; j<=radius; j++)
		for(i=-radius; i<=radius; i++)
			if(i*i+j*j <= radius*radius)
				n2D_setPixel(x + i, y + j, c);
}

// TODO : do better than that
void n2D_fillEllipse(INT_TYPE x, INT_TYPE y, INT_TYPE w, INT_TYPE h, PIXEL_TYPE c)
{
	INT_TYPE i,j;
	for(j=-h; j<=h; j++)
		for(i=-w; i<=w; i++)
			if(i*i*h*h+j*j*w*w <= h*h*w*w)
				n2D_setPixel(x + i, y + j, c);
}

/*        *
 *  Text  *
 *        */

INT_TYPE isOutlinePixel(unsigned char* charfont, INT_TYPE x, INT_TYPE y)
{
	INT_TYPE xis0 = !x, xis7 = x == 7, yis0 = !y, yis7 = y == 7;
	
	if(xis0)
	{
		if(yis0)
		{
			return !(*charfont & 0x80) && ((*charfont & 0x40) || (charfont[1] & 0x80) || (charfont[1] & 0x40));
		}
		else if(yis7)
		{
			return !(charfont[7] & 0x80) && ((charfont[7] & 0x40) || (charfont[6] & 0x80) || (charfont[6] & 0x40));
		}
		else
		{
			return !(charfont[y] & 0x80) && (
				(charfont[y - 1] & 0x80) || (charfont[y - 1] & 0x40) ||
				(charfont[y] & 0x40) ||
				(charfont[y + 1] & 0x80) || (charfont[y + 1] & 0x40));
		}
	}
	else if(xis7)
	{
		if(yis0)
		{
			return !(*charfont & 0x01) && ((*charfont & 0x02) || (charfont[1] & 0x01) || (charfont[1] & 0x02));
		}
		else if(yis7)
		{
			return !(charfont[7] & 0x01) && ((charfont[7] & 0x02) || (charfont[6] & 0x01) || (charfont[6] & 0x02));
		}
		else
		{
			return !(charfont[y] & 0x01) && (
				(charfont[y - 1] & 0x01) || (charfont[y - 1] & 0x02) ||
				(charfont[y] & 0x02) ||
				(charfont[y + 1] & 0x01) || (charfont[y + 1] & 0x02));
		}
	}
	else
	{
		char b = 1 << (7 - x);
		if(yis0)
		{
			return !(*charfont & b) && (
				(*charfont & (b << 1)) || (*charfont & (b >> 1)) ||
				(charfont[1] & (b << 1)) || (charfont[1] & b) || (charfont[1] & (b >> 1)));
		}
		else if(yis7)
		{
			return !(charfont[7] & b) && (
				(charfont[7] & (b << 1)) || (charfont[7] & (b >> 1)) ||
				(charfont[6] & (b << 1)) || (charfont[6] & b) || (charfont[6] & (b >> 1)));
		}
		else
		{
			return !(charfont[y] & b) && (
				(charfont[y] & (b << 1)) || (charfont[y] & (b >> 1)) ||
				(charfont[y - 1] & (b << 1)) || (charfont[y - 1] & b) || (charfont[y - 1] & (b >> 1)) ||
				(charfont[y + 1] & (b << 1)) || (charfont[y + 1] & b) || (charfont[y + 1] & (b >> 1)));
		}
	}
}

void n2D_drawChar(INT_TYPE *x, INT_TYPE *y, INT_TYPE margin, char ch, PIXEL_TYPE fc, PIXEL_TYPE olc)
{
	INT_TYPE i, j;
	unsigned char *charSprite;
	if(ch == '\n')
	{
		*x = margin;
		*y += 8;
	}
	else if(*y < 239)
	{
		charSprite = ch * 8 + n2DLib_font;
		// Draw charSprite as monochrome 8*8 image using given color
		for(i = 0; i < 8; i++)
		{
			for(j = 7; j >= 0; j--)
			{
				if((charSprite[i] >> j) & 1)
					n2D_setPixel(*x + (7 - j), *y + i, fc);
				else if(isOutlinePixel(charSprite, 7 - j, i))
					n2D_setPixel(*x + (7 - j), *y + i, olc);
			}
		}
		*x += 8;
	}
}

void n2D_drawString(INT_TYPE *x, INT_TYPE *y, INT_TYPE _x, const char *str, PIXEL_TYPE fc, PIXEL_TYPE olc)
{
	INT_TYPE i, max = (INT_TYPE)strlen(str);
	for(i = 0; i < max; i++)
		n2D_drawChar(x, y, _x, str[i], fc, olc);
}

void n2D_drawDecimal(INT_TYPE *x, INT_TYPE *y, INT_TYPE n, PIXEL_TYPE fc, PIXEL_TYPE olc)
{
	// Ints are in [-2147483648, 2147483647]
	//               |        |
	INT_TYPE divisor =    1000000000, num, numHasStarted = 0;
	
	if(n < 0)
	{
		n2D_drawChar(x, y, 0, '-', fc, olc);
		n = -n;
	}
	while(divisor != 0)
	{
		num = n / divisor;
		if(divisor == 1 || num != 0 || numHasStarted)
		{
			numHasStarted = 1;
			n2D_drawChar(x, y, 0, num + '0', fc, olc);
		}
		n %= divisor;
		divisor /= 10;
	}
}

void n2D_drawStringF(INT_TYPE *x, INT_TYPE *y, INT_TYPE _x, PIXEL_TYPE fc, PIXEL_TYPE olc, const char *s, ...)
{
	va_list specialArgs;
	char str[1200] = { 0 };
	// Max nb of chars on-screen
	
	va_start(specialArgs, s);
	vsprintf_s(str, 1200, s, specialArgs);
	n2D_drawString(x, y, _x, str, fc, olc);
	va_end(specialArgs);
}

UINT_TYPE n2D_numberWidth(INT_TYPE n)
{
	// Ints are in [-2147483648, 2147483647]
	UINT_TYPE divisor = 10, result = 8, number = n;
	
	if(n < 0)
	{
		result += 8;
		number = -n;
	}
	
	while(1)
	{
		if(number < divisor)
			return result;
		result += 8;
		divisor *= 10;
	}
}

UINT_TYPE n2D_stringWidth(const char* s)
{
	UINT_TYPE i, result = 0, size = (UINT_TYPE)strlen(s);
	for(i = 0; i < size; i++)
	{
		if(s[i] >= 0x20)
			result += 8;
	}
	return result;
}

/*               *
 * Miscellaneous *
 *               */

PIXEL_TYPE *n2D_loadBMP(const char *path, PIXEL_TYPE transparency)
{
	INT_TYPE size, width, height, offset, i, j;
	uint16_t *returnValue;
	FILE* temp;
	fopen_s(&temp, path, "rb");
	
	if(!temp) return NULL;

	// Check if the file's 2 first char are BM (indicates bitmap)
	if(!(fgetc(temp) == 0x42 && fgetc(temp) == 0x4d))
	{
		printf("Image is not a bitmap\n");
		fclose(temp);
		return NULL;
	}
	
	// Check if the file is in 24 bpp
	fseek(temp, 0x1c, SEEK_SET);
	if(fgetc(temp) != 24)
	{
		printf("Wrong format : bitmap must use 24 bpp\n");
		fclose(temp);
		return NULL;
	}
	
	// Get the 4-bytes pixel width and height, situated respectively at 0x12 and 0x16
	fseek(temp, 0x12, SEEK_SET);
	width = fgetc(temp) | (fgetc(temp) << 8) | (fgetc(temp) << 16) | (fgetc(temp) << 24);
	fseek(temp, 0x16, SEEK_SET);
	height = fgetc(temp) | (fgetc(temp) << 8) | (fgetc(temp) << 16) | (fgetc(temp) << 24);
	size = width * height + 3; // include header
	
	// Gets the 4-bytes offset to the start of the pixel table, situated at 0x0a
	fseek(temp, 0x0a, SEEK_SET);
	offset = fgetc(temp) | (fgetc(temp) << 8) | (fgetc(temp) << 16) | (fgetc(temp) << 24);
	
	fseek(temp, offset, SEEK_SET);
	
	returnValue = (uint16_t*)malloc(size * sizeof(PIXEL_TYPE));
	if(!returnValue)
	{
		printf("Couldn't allocate memory\n");
		fclose(temp);
		return NULL;
	}
	returnValue[0] = width;
	returnValue[1] = height;
	returnValue[2] = transparency;
	for (j = height - 1; j >= 0; j--)
	{
		for (i = 0; i < width; i++)
		{
			uint8_t b = fgetc(temp), g = fgetc(temp), r = fgetc(temp);
			returnValue[j * width + i + 3] = n2D_rgbToPixel(r, g, b);
		}
	}
	
	fclose(temp);

	return returnValue;
}
