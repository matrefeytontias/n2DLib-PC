#pragma once

#include "n2DLib/n2DLib_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct SDL_Texture;
typedef struct SDL_Texture SDL_Texture;

#define MAX_TIMER 2

// Initialize display and buffering.
void n2D_init();
// Clear and free resources.
void n2D_deinit();
// Limits the framerate to a fixed amount.
void n2D_constrainFrameRate(INT_TYPE fps);
// Displays the framerate at the bottom of the screen.
void n2D_displayFrameRate();
// Refreshes the screen with the buffer's contents.
void n2D_updateScreen();
// TODO : drop timers support
void n2D_timerLoad(ID_TYPE timerId, UINT_TYPE value);
UINT_TYPE n2D_timerRead(ID_TYPE timerId);
// Clear the buffer with black.
void n2D_clearBufferB();
// Clear the buffer with white.
void n2D_clearBufferW();
// Clear the buffer with a given color.
void n2D_clearBuffer(PIXEL_TYPE color);
// Convert an RGB triplet to the currently chosen pixel representation.
PIXEL_TYPE n2D_rgbToPixel(uint8_t r, uint8_t g, uint8_t b);
// Read a pixel from an image with no bounds-checking.
PIXEL_TYPE n2D_getPixelUnsafe(const PIXEL_TYPE* src, UINT_TYPE x, UINT_TYPE y);
// Read a pixel from an image with bounds-checking.
PIXEL_TYPE n2D_getPixel(const PIXEL_TYPE* src, UINT_TYPE x, UINT_TYPE y);
// Sets a pixel in the buffer with no bounds-checking.
void n2D_setPixelUnsafe(UINT_TYPE x, UINT_TYPE y, PIXEL_TYPE color);
// Sets a pixel in the buffer with bounds-checking.
void n2D_setPixel(UINT_TYPE x, UINT_TYPE y, PIXEL_TYPE color);
// Draw a horizontal line.
void n2D_drawHLine(INT_TYPE y, INT_TYPE x1, INT_TYPE x2, PIXEL_TYPE color);
// Draw a vertical line.
void n2D_drawVLine(INT_TYPE x, INT_TYPE y1, INT_TYPE y2, PIXEL_TYPE color);
// Fill a rectangle with a color given top-left corner position, width and height.
void n2D_fillRect(INT_TYPE x, INT_TYPE y, INT_TYPE w, INT_TYPE h, PIXEL_TYPE color);
// Draw a sprite, optionally flashing a color.
void n2D_drawSprite(const PIXEL_TYPE* img, INT_TYPE x, INT_TYPE y, BOOL_TYPE flash, PIXEL_TYPE color);
// Draw a part of a sprite, optionally flashing a color.
void n2D_drawSpritePart(const PIXEL_TYPE* img, INT_TYPE x, INT_TYPE y, const Rect* part, BOOL_TYPE flash, PIXEL_TYPE color);
// Draw a sprite to fill a destination rectangle, optionally flashing.
void n2D_drawSpriteScaled(const PIXEL_TYPE* img, const Rect* destination, BOOL_TYPE flash, PIXEL_TYPE color);
// Draw a sprite rotated around an optionally provided center point. Pass in NULL to compute the image's center.
void n2D_drawSpriteRotated(const PIXEL_TYPE* img, const Rect* xy, const Rect* center, Fixed angle, BOOL_TYPE flash, PIXEL_TYPE color);
// Draw an arbitrary line.
void n2D_drawLine(INT_TYPE x1, INT_TYPE y1, INT_TYPE x2, INT_TYPE y2, PIXEL_TYPE color);
// Link a polygon's vertices with lines. Points come in order of `INT_TYPE x, INT_TYPE y`.
void n2D_drawPolygon(PIXEL_TYPE color, INT_TYPE nbPoints, ...);
// Fill a circle with a color given its center coordinates and radius.
void n2D_fillCircle(INT_TYPE x, INT_TYPE y, INT_TYPE radius, PIXEL_TYPE color);
// Fill an ellipse with a color given its center coordinates and radii.
void n2D_fillEllipse(INT_TYPE x, INT_TYPE y, INT_TYPE radiusW, INT_TYPE radiusH, PIXEL_TYPE);
// Draw a string with foreground and outline colors, updating the position as it goes. Supports newlines
// by resetting the x position to the provided margin value.
void n2D_drawString(INT_TYPE* x, INT_TYPE* y, INT_TYPE margin, const char* str, PIXEL_TYPE textColor, PIXEL_TYPE outlineColor);
// Same as `n2D_drawString` but draws a decimal number.
void n2D_drawDecimal(INT_TYPE* x, INT_TYPE* y, INT_TYPE number, PIXEL_TYPE textColor, PIXEL_TYPE outlineColor);
// Same as `n2D_drawString` but draws a single character.
void n2D_drawChar(INT_TYPE* x, INT_TYPE* y, INT_TYPE margin, char ch, PIXEL_TYPE textColor, PIXEL_TYPE outlineColor);
// Same as `n2D_drawString` but takes a format string and arguments.
void n2D_drawStringF(INT_TYPE* x, INT_TYPE* y, INT_TYPE margin, PIXEL_TYPE textColor, PIXEL_TYPE outlineColor, const char* fstr, ...);
// Returns the width of a number in pixels when drawn with the font currently in use.
UINT_TYPE n2D_numberWidth(INT_TYPE number);
// Returns the width of a string in pixels when drawn with the font currently in use.
UINT_TYPE n2D_stringWidth(const char* str);

// Loads a 24-bpp bitmap image into an n2DLib-compatible PIXEL_TYPE* array, using a color key
// for transparency. Has to be freed when done using.
PIXEL_TYPE* n2D_loadBMP(const char* path, PIXEL_TYPE colorKey);

#define BUFF_BYTES_SIZE (320*240*2)
extern PIXEL_TYPE* BUFF_BASE_ADDRESS;

#ifdef __cplusplus
}
#endif