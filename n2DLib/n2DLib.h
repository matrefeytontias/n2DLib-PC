#pragma once

#include "n2DLib/n2DLib_types.h"

// TODO : use sized types

#ifdef __cplusplus
extern "C" {
#endif

struct SDL_Texture;
typedef struct SDL_Texture SDL_Texture;

#define MAX_TIMER 2

void initBuffering();
void constrainFrameRate(INT_TYPE);
void displayFrameRate();
void updateScreen();
void updateKeys();
BOOL_TYPE isKeyPressed(t_key _k);
void deinitBuffering();
void timer_load(ID_TYPE, UINT_TYPE);
UINT_TYPE timer_read(ID_TYPE);
void clearBufferB();
void clearBufferW();
void clearBuffer(PIXEL_TYPE);
PIXEL_TYPE rgbToPixel(unsigned char r, unsigned char g, unsigned char b);
PIXEL_TYPE getPixelUnsafe(const PIXEL_TYPE*, UINT_TYPE, UINT_TYPE);
PIXEL_TYPE getPixel(const PIXEL_TYPE*, UINT_TYPE, UINT_TYPE);
void setPixelUnsafe(UINT_TYPE, UINT_TYPE, PIXEL_TYPE);
void setPixel(UINT_TYPE, UINT_TYPE, PIXEL_TYPE);
void drawHLine(INT_TYPE, INT_TYPE, INT_TYPE, PIXEL_TYPE);
void drawVLine(INT_TYPE, INT_TYPE, INT_TYPE, PIXEL_TYPE);
void fillRect(INT_TYPE, INT_TYPE, INT_TYPE, INT_TYPE, PIXEL_TYPE);
void drawSprite(const PIXEL_TYPE*, INT_TYPE, INT_TYPE, INT_TYPE, PIXEL_TYPE);
void drawSpritePart(const PIXEL_TYPE*, INT_TYPE, INT_TYPE, const Rect*, INT_TYPE, PIXEL_TYPE);
void drawSpriteScaled(const PIXEL_TYPE*, const Rect*, INT_TYPE, PIXEL_TYPE);
void drawSpriteRotated(const PIXEL_TYPE*, const Rect*, const Rect*, Fixed, INT_TYPE, PIXEL_TYPE);
void drawLine(INT_TYPE, INT_TYPE, INT_TYPE, INT_TYPE, PIXEL_TYPE);
void drawPolygon(PIXEL_TYPE, INT_TYPE, ...);
void fillCircle(INT_TYPE, INT_TYPE, INT_TYPE, PIXEL_TYPE);
void fillEllipse(INT_TYPE, INT_TYPE, INT_TYPE, INT_TYPE, PIXEL_TYPE);
void drawString(INT_TYPE*, INT_TYPE*, INT_TYPE, const char*, PIXEL_TYPE, PIXEL_TYPE);
void drawDecimal(INT_TYPE*, INT_TYPE*, INT_TYPE, PIXEL_TYPE, PIXEL_TYPE);
void drawChar(INT_TYPE*, INT_TYPE*, INT_TYPE, char, PIXEL_TYPE, PIXEL_TYPE);
void drawStringF(INT_TYPE*, INT_TYPE*, INT_TYPE, PIXEL_TYPE, PIXEL_TYPE, const char*, ...);
UINT_TYPE numberWidth(INT_TYPE);
UINT_TYPE stringWidth(const char*);
void wait_no_key_pressed(t_key);
BOOL_TYPE get_key_pressed(t_key*);
BOOL_TYPE isKey(t_key, t_key);

PIXEL_TYPE* loadBMP(const char*, PIXEL_TYPE);

#define BUFF_BYTES_SIZE (320*240*2)
extern PIXEL_TYPE* BUFF_BASE_ADDRESS;
extern SDL_Texture* MAIN_SCREEN;

#ifdef __cplusplus
}
#endif