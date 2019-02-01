#include <display_syscalls.h>
#include <keyboard_syscalls.h>
#include <sprite.h>
#include <keyboard.hpp>
#include "res.h"

#define RGB24to565(r, g, b) (((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3)
#define VRAM ((color_t*)0xA8000000)
#define SetPixel(x, y, color) (*(VRAM + (x) + LCD_WIDTH_PX * (y)) = (color_t)color)
#define COLOR_ALPHA ((color_t)0xf81f)

const color_t *tileset[] = {
    (color_t *) & data_tile003_bmp,
    (color_t *) & data_tile000_bmp,
    (color_t *) & data_tile001_bmp,
    (color_t *) & data_tile002_bmp,
};

const color_t *playerAni[][4] = {
    {
        (color_t *) & data_char0_bmp,
        (color_t *) & data_char1_bmp,
        (color_t *) & data_char0_bmp,
        (color_t *) & data_char2_bmp,
    },
    {
        (color_t *) & data_char3_bmp,
        (color_t *) & data_char4_bmp,
        (color_t *) & data_char3_bmp,
        (color_t *) & data_char5_bmp,
    }
};

extern const unsigned char map[50][50];

void drawSprite(const color_t* data, int x, int y, int width, int height) {
    int i, j, rx;
    for (j = y; j < y + height; j++) {
        if (j >= LCD_HEIGHT_PX) break;
        for (i = x; i < x + width; i++) {
            if (i >= 0 && i < LCD_WIDTH_PX && *data != COLOR_ALPHA) {
                SetPixel(i, j, *data);
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
                SetPixel(width - i - 1 + x, j, *data);
            }
            data++;
        }
    }
}

struct {
    int x;
    int y;

    struct {
        int l;
        int r;
        int t;
        int b;
    } view;

    int dir;
    int stepIndex;

} player;

const int VIEW_RANGE = 6;
const int MAP_WIDTH = 50;
const int MAP_HEIGHT = 50;
const int CENTER_X = LCD_WIDTH_PX / 2;
const int CENTER_Y = LCD_HEIGHT_PX / 2;
const int PLAYER_CENTER_X = LCD_WIDTH_PX / 2 - 11;
const int PLAYER_CENTER_Y = LCD_HEIGHT_PX / 2 - 45;

int main(void) {
	int key;

    player.x = 5;
    player.y = 5;
    player.dir = 0;
    player.stepIndex = 0;

    Bdisp_EnableColor(1);
    DisplayStatusArea();

	while (1) {
        Bdisp_AllClr_VRAM();
        // drawSprite(tileset[0], 0, 0, 32, 16);
        // drawSprite(tileset[0], 16, 8, 32, 16);
        {
            int mx, my, ox, oy, rx, ry;

            player.view.l = player.x - VIEW_RANGE;
            player.view.r = player.x + VIEW_RANGE;
            player.view.t = player.y - VIEW_RANGE;
            player.view.b = player.y + VIEW_RANGE;

            if (player.view.l < 0) player.view.l = 0;
            if (player.view.t < 0) player.view.t = 0;
            if (player.view.r >= MAP_WIDTH) player.view.r = MAP_WIDTH - 1;
            if (player.view.b >= MAP_HEIGHT) player.view.b = MAP_HEIGHT - 1;

            for (mx = player.view.l; mx <= player.view.r; ++mx) {
                for (my = player.view.t; my <= player.view.b; ++my) {
                    ox = mx - player.x;
                    oy = my - player.y;
                    rx = (CENTER_X - 16) + 16 * ox - 16 * oy;
                    ry = (CENTER_Y - 8) + 8 * ox + 8 * oy;
                    drawSprite(tileset[map[my][mx]], rx, ry, 32, 16);
                }
            }

            if (player.dir == 0 || player.dir == 1) {
                drawSprite(playerAni[player.dir][player.stepIndex],
                    PLAYER_CENTER_X, PLAYER_CENTER_Y, 22, 50);
            }
            else {
                drawSpriteRev(playerAni[player.dir - 2][player.stepIndex],
                    PLAYER_CENTER_X, PLAYER_CENTER_Y, 22, 50);
            }

            player.stepIndex = (player.stepIndex + 1) % 4;
            
        }
        GetKey(&key);

		switch (key) {
        case KEY_CTRL_DOWN:
            player.dir = 0;
            player.y++;
            break;
        case KEY_CTRL_LEFT:
            player.dir = 1;
            player.x--;
            break;
        case KEY_CTRL_RIGHT:
            player.dir = 2;
            player.x++;
            break;
         case KEY_CTRL_UP:
            player.dir = 3;
            player.y--;
            break;
		}
	}
 
	return 0;
}

const unsigned char map[50][50] = {
{1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 3, 1, 0, 1, 1, 1, 0, 0, 1, 1, 3, 0, 1, 3, 0, 0, 1, 1, 0, 0, 1, 1, 1, 2, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, },
{0, 0, 0, 2, 0, 2, 2, 1, 1, 3, 0, 0, 1, 1, 1, 3, 1, 1, 0, 0, 3, 0, 1, 0, 3, 0, 0, 0, 1, 1, 0, 2, 3, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 3, },
{1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 1, 3, 0, 3, 1, 1, 0, 1, 1, 0, 0, 2, 0, 1, 3, 1, 1, 3, 1, 1, 0, 0, 2, 0, 2, 0, 1, },
{2, 1, 0, 1, 1, 1, 0, 1, 1, 0, 3, 3, 1, 2, 1, 1, 0, 0, 1, 2, 2, 0, 0, 0, 1, 0, 3, 0, 0, 1, 0, 0, 2, 0, 0, 1, 0, 1, 2, 0, 1, 0, 1, 0, 3, 1, 0, 2, 1, 0, },
{2, 1, 0, 3, 1, 2, 0, 1, 2, 0, 1, 0, 3, 1, 1, 0, 0, 0, 3, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 2, 1, 1, 0, 3, 0, 0, 0, 0, 1, 0, 1, 2, },
{0, 1, 0, 0, 3, 0, 0, 3, 3, 2, 3, 3, 0, 0, 0, 2, 0, 0, 1, 0, 2, 1, 1, 0, 0, 3, 1, 1, 1, 1, 0, 3, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 3, 2, 2, 0, 0, 0, },
{1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 3, 1, 0, 0, 0, 2, 1, 0, 0, 0, 1, 0, 0, 0, 1, 2, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 3, 2, 0, 1, 2, 0, },
{0, 0, 1, 0, 3, 0, 0, 0, 1, 1, 0, 2, 1, 1, 3, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 2, 1, 0, 1, 2, 0, 3, 1, 0, 1, 3, 1, 0, 1, 0, 0, 3, 1, 1, 1, 1, 1, 1, },
{2, 0, 1, 1, 2, 1, 0, 1, 1, 0, 1, 3, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 3, 0, 0, 0, 0, 0, 2, 0, 1, 2, 1, 0, 0, 0, 0, 1, 1, 1, 2, 0, 0, 0, 0, 0, 1, 3, 0, 1, },
{1, 0, 1, 1, 0, 1, 1, 2, 0, 1, 2, 0, 1, 0, 0, 2, 0, 3, 0, 0, 2, 0, 0, 0, 0, 1, 3, 1, 2, 2, 0, 0, 2, 0, 1, 3, 0, 1, 0, 0, 3, 0, 1, 0, 1, 1, 1, 1, 0, 0, },
{0, 0, 0, 2, 2, 0, 0, 0, 2, 0, 2, 0, 1, 0, 2, 1, 3, 1, 1, 1, 3, 1, 2, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 2, 1, 0, 0, 1, 1, 3, 1, 0, 2, 1, 1, 3, 0, 0, 1, 0, },
{0, 1, 0, 2, 0, 1, 2, 1, 3, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 2, 1, 0, 0, 0, 2, 0, 0, 1, 2, 0, 1, 2, 0, 0, 2, 0, 2, 1, 0, 1, 0, 0, 1, 1, 3, 1, 3, 0, 0, },
{0, 0, 1, 3, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 1, 1, 2, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 3, 3, 0, 0, 0, 1, },
{0, 2, 0, 1, 2, 0, 0, 0, 3, 0, 1, 0, 0, 1, 1, 1, 1, 2, 0, 2, 0, 1, 1, 1, 0, 1, 0, 2, 0, 3, 0, 2, 2, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 3, 0, 0, 0, 0, 1, },
{0, 1, 3, 0, 0, 2, 3, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 3, 0, 0, 0, 0, 3, 0, 0, 1, 1, 1, 0, 0, },
{1, 1, 0, 1, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 1, 0, 1, 3, 0, 0, 0, 1, 0, 3, 1, 1, 0, 2, 1, 0, 2, 1, 1, 0, 0, 1, 1, 0, 0, 0, 2, },
{1, 1, 0, 0, 0, 0, 2, 0, 0, 2, 0, 3, 1, 0, 1, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 2, 1, 0, 0, 2, 2, 1, 1, 0, 3, 3, 2, 1, 0, 3, 0, 0, 0, 2, 0, 1, 0, 0, 0, 2, },
{0, 3, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 2, 1, 1, 0, 3, 0, 0, 0, 0, 3, 1, 2, 0, 3, 0, 0, 1, 3, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 2, 0, },
{1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 3, 0, 2, 0, 2, 3, 2, 2, 1, 1, 3, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 3, 0, 1, 0, 0, 1, 0, 3, 1, 2, 2, },
{0, 0, 1, 0, 2, 0, 3, 0, 0, 2, 2, 1, 2, 3, 1, 1, 2, 0, 0, 0, 0, 1, 2, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 3, 0, 1, 0, 0, 0, 0, 0, 1, 0, 2, 0, },
{3, 1, 2, 0, 2, 0, 1, 0, 2, 0, 1, 0, 0, 0, 0, 0, 2, 0, 1, 0, 0, 0, 1, 2, 3, 3, 0, 1, 1, 0, 0, 1, 3, 2, 2, 0, 0, 2, 3, 1, 0, 1, 1, 0, 0, 1, 3, 1, 0, 0, },
{1, 0, 0, 1, 3, 2, 0, 0, 2, 0, 2, 0, 3, 0, 3, 0, 1, 0, 1, 0, 1, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 1, 2, 0, 3, 1, 2, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 2, 0, 1, },
{3, 1, 0, 0, 1, 3, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 3, 2, 0, 1, 0, 3, 0, 2, 0, 2, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, },
{1, 1, 1, 0, 2, 0, 3, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0, 0, 0, 1, 1, 1, 0, 3, 2, 0, 0, 1, 0, 0, 1, 0, 2, 0, 0, 0, 0, 1, 0, 2, 0, 2, 3, 0, 1, 1, 1, 1, 1, 1, },
{3, 2, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 2, 3, 0, 1, 1, 3, 1, 1, 0, 0, 0, 3, 2, 0, 3, 1, 0, 0, 1, 0, 1, 0, 0, 3, 0, 0, 1, 3, 0, 0, 2, 2, 2, 0, 1, 0, 0, 0, },
{1, 2, 0, 1, 2, 0, 2, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 2, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 2, 1, 1, 1, 0, 1, 2, 0, 0, 0, 0, 0, 2, 0, 1, 1, 2, },
{1, 1, 1, 1, 2, 0, 1, 3, 0, 0, 0, 1, 0, 3, 0, 3, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 3, 3, 0, 0, 0, 3, 0, 0, 0, 0, 1, 3, 0, 0, 1, 0, 0, 1, 0, 1, 1, 3, 0, 0, },
{0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 2, 0, 0, 3, 3, 1, 0, 3, 0, 0, 0, 1, 1, 1, 2, 0, },
{2, 0, 0, 0, 1, 0, 0, 3, 1, 2, 0, 0, 3, 0, 2, 0, 2, 0, 1, 0, 0, 0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 0, 1, 0, 1, 0, 0, 0, 0, 3, 2, 1, 1, 0, 1, 3, 1, 0, 0, 1, },
{1, 0, 1, 1, 0, 1, 0, 0, 1, 3, 0, 1, 1, 0, 3, 0, 1, 1, 3, 0, 3, 0, 2, 1, 0, 1, 0, 0, 0, 1, 0, 0, 2, 3, 1, 0, 0, 1, 0, 0, 2, 0, 0, 0, 3, 2, 0, 1, 1, 0, },
{3, 1, 2, 0, 0, 1, 1, 0, 0, 1, 0, 0, 2, 1, 0, 1, 0, 0, 3, 2, 3, 0, 0, 0, 1, 0, 2, 0, 2, 1, 0, 1, 2, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 2, 0, },
{0, 2, 1, 0, 1, 0, 0, 0, 0, 1, 3, 0, 0, 0, 1, 0, 1, 0, 3, 0, 0, 0, 1, 3, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 2, 1, 3, 1, 2, 1, 3, 1, 1, 0, 0, 0, 1, },
{0, 0, 0, 0, 3, 0, 0, 1, 0, 1, 2, 1, 0, 0, 1, 0, 0, 0, 0, 2, 2, 3, 0, 0, 1, 2, 0, 1, 1, 0, 0, 1, 0, 2, 0, 0, 0, 3, 0, 0, 0, 3, 1, 0, 1, 0, 2, 3, 1, 1, },
{0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 2, 0, 3, 1, 3, 0, 0, 2, 0, 0, 0, 0, 0, 2, 1, 0, 1, 0, 0, 0, 0, 1, 2, 0, 2, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 2, 2, 2, 1, },
{1, 0, 0, 0, 1, 0, 0, 0, 0, 3, 0, 2, 0, 1, 1, 3, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 2, 1, 0, 2, 3, 3, 3, 1, 2, 0, 1, 2, 0, 3, 0, 0, 0, 3, 0, 1, },
{3, 1, 0, 3, 3, 3, 0, 0, 1, 1, 0, 0, 3, 1, 1, 0, 0, 2, 0, 0, 1, 1, 0, 0, 2, 2, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 2, 0, 0, 0, 2, 0, 3, 3, 3, 0, 0, 0, 1, 0, },
{2, 1, 0, 0, 0, 1, 0, 0, 2, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 3, 1, 2, 0, 1, 0, 0, 3, 1, 0, 0, 2, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 3, 0, 0, },
{0, 3, 0, 2, 2, 3, 0, 1, 0, 0, 1, 3, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 2, 1, 1, 1, 1, 2, 0, 1, 0, 2, 1, 1, 0, 0, 2, 0, 3, 1, 0, 1, 1, 2, 0, 1, 1, },
{3, 3, 0, 2, 0, 0, 2, 0, 0, 1, 1, 0, 1, 0, 1, 3, 0, 1, 3, 3, 1, 0, 0, 0, 1, 1, 3, 1, 0, 3, 1, 0, 1, 2, 1, 0, 1, 0, 2, 2, 0, 1, 0, 1, 0, 2, 0, 0, 1, 1, },
{1, 1, 0, 1, 0, 3, 1, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 1, 1, 0, 1, 0, 2, 0, 3, 1, 0, 0, 1, 1, 3, 2, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 0, 1, 1, 1, },
{1, 1, 1, 0, 0, 0, 0, 2, 3, 0, 1, 0, 1, 1, 1, 2, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 2, 3, 1, 1, 1, 0, 1, 0, 2, },
{0, 0, 0, 0, 1, 2, 1, 1, 1, 1, 0, 0, 2, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 1, 3, 3, 1, 2, 0, 1, 1, 0, 0, 0, 1, 1, 0, 3, 0, 0, 0, 0, 0, },
{0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 1, 0, 2, 2, 0, 2, 0, 0, 0, 0, 1, 1, 1, 0, 2, 0, 1, 0, 3, 0, 0, 1, 1, 0, 2, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 1, 3, },
{1, 1, 0, 0, 0, 0, 0, 1, 3, 3, 2, 1, 1, 0, 0, 0, 3, 0, 0, 0, 1, 2, 1, 2, 3, 1, 0, 2, 1, 1, 0, 1, 2, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 2, 1, 1, 0, 3, 0, },
{1, 1, 2, 2, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 2, 0, 1, 1, 3, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 3, 0, 1, 0, 0, 1, 2, },
{3, 0, 0, 0, 1, 0, 0, 3, 1, 2, 3, 1, 2, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 1, 1, 0, 2, 0, 0, 1, 0, 0, 0, 2, 0, 2, 3, 0, 0, 1, 1, 0, 0, 0, 0, 3, },
{0, 0, 0, 2, 0, 1, 0, 0, 0, 1, 3, 1, 0, 0, 0, 3, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 3, 0, 1, 0, 0, 1, 0, 0, 0, 3, 1, 2, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, },
{0, 0, 2, 0, 1, 1, 1, 0, 1, 3, 1, 1, 0, 1, 0, 2, 0, 1, 0, 1, 1, 1, 0, 0, 0, 3, 2, 2, 0, 1, 2, 0, 1, 1, 0, 0, 0, 0, 3, 0, 2, 1, 1, 0, 1, 3, 0, 3, 3, 0, },
{1, 0, 1, 2, 0, 0, 1, 1, 1, 2, 2, 1, 0, 0, 3, 0, 3, 1, 1, 0, 3, 1, 0, 1, 0, 0, 0, 0, 0, 2, 2, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 3, 1, 0, },
{1, 0, 3, 0, 0, 3, 2, 1, 1, 0, 2, 0, 3, 2, 0, 0, 3, 1, 2, 0, 0, 0, 0, 3, 0, 0, 1, 0, 1, 2, 1, 2, 0, 1, 0, 1, 0, 3, 1, 0, 1, 3, 0, 0, 0, 0, 2, 1, 0, 0, },
};