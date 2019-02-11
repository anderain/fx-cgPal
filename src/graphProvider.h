#define LCD_WIDTH_PX 384
#define LCD_HEIGHT_PX 216
typedef unsigned short color_t;

#ifndef _GRAPH_PROVIDER_H_
#   define _GRAPH_PROVIDER_H_
#   include "platform.h"
#   if defined(APP_MSVC)
#       include "SDL.h"

void setPixel(int x, int y, color_t color);
void putDisp();

#   elif defined(APP_FXCG)
#       include <display.h>
#       include <display_syscalls.h>
#       define VRAM ((color_t*)0xA8000000)
#       define setPixel(x, y, color) (*(VRAM + (x) + LCD_WIDTH_PX * (y)) = (color_t)color)
#       define putDisp() Bdisp_PutDisp_DD()
#   endif

typedef struct {
    int width, height;
    const color_t *palette;
    const unsigned char *image;
} PALETTED_IMAGE;

#define RGB24to565(r, g, b) (((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3)
#define COLOR_ALPHA ((color_t)0xf81f)

void initGraphApp();
void allClr();
//void drawSprite(const color_t* data, int x, int y, int width, int height);
//void drawSpriteZoom(const color_t* data, int x, int y, int width, int height, int zoom);
//void drawSpriteRev(const color_t* data, int x, int y, int width, int height);
//void drawSpriteRect(const color_t* data, int x, int y, int width, int height, int vx, int vy, int vw, int vh);
//void drawSpriteRectRev(const color_t* data, int x, int y, int width, int height, int vx, int vy, int vw, int vh);

void drawSpritePaletted(const PALETTED_IMAGE* img, int x, int y);
void drawSpritePalettedRev(const PALETTED_IMAGE* img, int x, int y);
void drawSpritePalettedRect(const PALETTED_IMAGE* img, int x, int y, int vx, int vy, int vw, int vh);
void drawSpritePalettedRectRev(const PALETTED_IMAGE* img, int x, int y, int vx, int vy, int vw, int vh);

void drawSpritePalettedZoom(const PALETTED_IMAGE* img, int x, int y, int zoom);

#endif
