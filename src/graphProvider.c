#include "graphProvider.h"

#if defined(APP_MSVC)

SDL_Surface * screen;

void initGraphApp() {
    printf("Initializing SDL.\n");
    /* Initialize Video */
    if ((SDL_Init(SDL_INIT_VIDEO) == -1)) {
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
    }
    printf("SDL initialized.\n");
    atexit(SDL_Quit);

    // screen = SDL_SetVideoMode(640, 480, 16, SDL_SWSURFACE);
    screen = SDL_SetVideoMode(LCD_WIDTH_PX, LCD_HEIGHT_PX, 16, SDL_SWSURFACE);
    if (screen == NULL) {
        fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_WM_SetCaption("fx-CG Project SDL Wrapper", 0);
}

void SDL_PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        }
        else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

void putDisp() {
    SDL_UpdateRect(screen, 0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX);
}

inline void setPixel(int x, int y, color_t color) {
    SDL_PutPixel(screen, x, y, color);
}

void allClr() {
    SDL_FillRect(screen, NULL, 0);
}

#elif defined(APP_FXCG)

#define setPixel(x, y, color) (*(VRAM + (x) + LCD_WIDTH_PX * (y)) = (color_t)color)

void initGraphApp() {
    Bdisp_EnableColor(1);
    DisplayStatusArea();
}

void allClr() {
    Bdisp_AllClr_VRAM();
}

#endif

void drawSprite(const color_t* data, int x, int y, int width, int height) {
    int i, j;
    for (j = y; j < y + height; j++) {
        if (j >= LCD_HEIGHT_PX) break;
        for (i = x; i < x + width; i++) {
            if (i >= 0 && i < LCD_WIDTH_PX && *data != COLOR_ALPHA) {
                setPixel(i, j, *data);
            }
            data++;
        }
    }
}

void drawSpriteZoom(const color_t* data, int x, int y, int width, int height, int zoom) {
    int i, j, rx, ry, tx, ty, a, b;
    for (j = 0; j < height; j++) {
        ry = j * zoom + y;
        for (i = 0; i < width; i++) {
            rx = i * zoom + x;
            for (tx = 0; tx < zoom; ++tx) {
                a = tx + rx;
                for (ty = 0; ty < zoom; ++ty) {
                    b = ty + ry;
                    if (b >= 0 && b < LCD_HEIGHT_PX && a >= 0 && a < LCD_WIDTH_PX && *data != COLOR_ALPHA) {
                        setPixel(a, b, *data);
                    }
                }
            }
            data++;
        }
    }
}

void drawSpriteRev(const color_t* data, int x, int y, int width, int height) {
    int i, j, rx;
    for (j = y; j < y + height; j++) {
        if (j >= LCD_HEIGHT_PX) break;
        for (i = 0; i < width; i++) {
            rx = width - i - 1 + x;
            if (rx >= 0 && rx < LCD_WIDTH_PX && *data != COLOR_ALPHA) {
                setPixel(width - i - 1 + x, j, *data);
            }
            data++;
        }
    }
}


void drawSpriteRect(const color_t* data,
    int x, int y, int width, int height,
    int vx, int vy, int vw, int vh) {

    int i, j, rx, ry;
    const color_t *pdata;

    for (j = 0; j < vh; j++) {
        ry = y + j;

        if (ry >= LCD_HEIGHT_PX) break;

        pdata = data + width * (vy + j) + vx;

        for (i = 0; i < vw; i++, pdata++) {
            rx = i + x;
            if (rx >= 0 && rx < LCD_WIDTH_PX && *pdata != COLOR_ALPHA) {
                setPixel(rx, ry, *pdata);
            }
        }
    }

}

void drawSpriteRectRev(const color_t* data,
    int x, int y, int width, int height,
    int vx, int vy, int vw, int vh) {

    int i, j, rx, ry;
    const color_t *pdata;

    for (j = 0; j < vh; j++) {
        ry = y + j;
        if (ry >= LCD_HEIGHT_PX) break;

        pdata = data + width * (vy + j) + vx;

        for (i = 0; i < vw; i++, pdata++) {
            rx = vw - i - 1 + x;
            if (rx >= 0 && rx < LCD_WIDTH_PX && *pdata != COLOR_ALPHA) {
                setPixel(rx, ry, *pdata);
            }
        }
    }

}

void drawSpritePaletted(const PALETTED_IMAGE* img, int x, int y) {
    int i, j;
    color_t pixel;
    const unsigned char *data = img->image;

    for (j = y; j < y + img->height; j++) {
        if (j < 0) continue;
        else if (j >= LCD_HEIGHT_PX) break;
        for (i = x; i < x + img->width; i++) {
            pixel = img->palette[*data];
            // printf("%d,", *data);
            if (i >= 0 && i < LCD_WIDTH_PX && pixel != COLOR_ALPHA) {
                setPixel(i, j, pixel);
            }
            data++;
        }
        // printf("\n");
    }
}

void drawSpritePalettedRev(const PALETTED_IMAGE* img, int x, int y) {
    int i, j, rx;
    const unsigned char *data = img->image;

    for (j = y; j < y + img->height; j++) {
        if (j >= LCD_HEIGHT_PX) break;
        for (i = 0; i < img->width; i++) {
            rx = img->width - i - 1 + x;
            if (rx >= 0 && rx < LCD_WIDTH_PX && *data != COLOR_ALPHA) {
                setPixel(img->width - i - 1 + x, j, *data);
            }
            data++;
        }
    }
}

void drawSpritePalettedRect(const PALETTED_IMAGE* img,
    int x, int y,
    int vx, int vy, int vw, int vh) {

    int i, j, rx, ry;
    const unsigned char *data = img->image;
    const unsigned char *pdata;
    color_t pixel;

    for (j = 0; j < vh; j++) {
        ry = y + j;

        if (ry >= LCD_HEIGHT_PX) break;

        pdata = data + img->width * (vy + j) + vx;

        for (i = 0; i < vw; i++, pdata++) {
            rx = i + x;
            pixel = img->palette[*pdata];
            if (rx >= 0 && rx < LCD_WIDTH_PX && pixel != COLOR_ALPHA) {
                setPixel(rx, ry, pixel);
            }
        }
    }

}

void drawSpritePalettedRectRev(const PALETTED_IMAGE* img,
    int x, int y,
    int vx, int vy, int vw, int vh) {

    int i, j, rx, ry;
    const unsigned char *data = img->image;
    const unsigned char *pdata;
    color_t pixel;

    for (j = 0; j < vh; j++) {
        ry = y + j;

        if (ry >= LCD_HEIGHT_PX) break;

        pdata = data + img->width * (vy + j) + vx;

        for (i = 0; i < vw; i++, pdata++) {
            rx = vw - i - 1 + x;
            pixel = img->palette[*pdata];
            if (rx >= 0 && rx < LCD_WIDTH_PX && pixel != COLOR_ALPHA) {
                setPixel(rx, ry, pixel);
            }
        }
    }

}

void drawSpritePalettedZoom(const PALETTED_IMAGE *img, int x, int y, int zoom) {
    int i, j, rx, ry, tx, ty, a, b;
    const unsigned char *data = img->image;
    color_t pixel;

    for (j = 0; j < img->height; j++) {
        ry = j * zoom + y;
        if (ry + zoom < 0) continue;
        if (ry - zoom >= LCD_HEIGHT_PX) break;
        data = img->image + j * img->width;
        for (i = 0; i < img->width; i++, data++) {
            rx = i * zoom + x;
            if (rx + zoom < 0) continue;
            if (rx - zoom >= LCD_WIDTH_PX) break;
            pixel = img->palette[*data];
            for (tx = 0; tx < zoom; ++tx) {
                a = tx + rx;
                for (ty = 0; ty < zoom; ++ty) {
                    b = ty + ry;
                    //printf("%d,%d\n", a, b);
                    if (b >= 0 && b < LCD_HEIGHT_PX && a >= 0 && a < LCD_WIDTH_PX && pixel != COLOR_ALPHA) {
                        setPixel(a, b, pixel);
                    }
                }
            }
        }
    }
}
