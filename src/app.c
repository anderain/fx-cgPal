#include "graphProvider.h"
#include "inputProvider.h"
#include "res.h"

#define CENTER_X (LCD_WIDTH_PX / 2)
#define CENTER_Y (LCD_HEIGHT_PX / 2 + 20)

#define CHAR_IMG_WIDTH 50
#define CHAR_IMG_HEIGHT 108

#define DIR_DOWN        0
#define DIR_LEFT        1
#define DIR_RIGHT       2
#define DIR_UP          3
#define DIR_DOWN_LEFT   4
#define DIR_DOWN_RIGHT  5
#define DIR_TOP_LEFT    6
#define DIR_TOP_RIGHT   7

struct {
    float x;
    float y;
    int dir;

    int stepIndex;

    struct {
        int l;
        int r;
        int t;
        int b;
    } view;

} player;


void redrawCharacter() {
    int iy = 0;
    int ix = 0;
    int isRev = 0;
/*
    player.view.l = player.x / 2 - LCD_WIDTH_PX / 2;
    player.view.r = player.x / 2 + LCD_WIDTH_PX / 2;
    player.view.t = player.y / 2 - LCD_HEIGHT_PX / 2;
    player.view.b = player.y / 2 + LCD_HEIGHT_PX / 2;

    if (player.view.l < 0) player.view.l = 0;
    if (player.view.t < 0) player.view.t = 0;
    if (player.view.r >= 640) player.view.r = 320 - 1;
    if (player.view.b >= 480) player.view.b = 240 - 1;
*/
    // printf("(%.2f %.2f)\n", player.x, player.y);
    drawSpritePalettedZoom(&data_map1_bmp, CENTER_X - player.x, CENTER_Y - player.y, 2);
    //drawSpritePaletted(&data_map0_bmp, CENTER_X - player.x, CENTER_Y - player.y);

    switch (player.dir) {
    case DIR_DOWN:iy = 1; break;
    case DIR_LEFT: iy = 2; break;
    case DIR_RIGHT: iy = 2; isRev = 1; break;
    case DIR_UP: iy = 0; break;
    case DIR_DOWN_LEFT: iy = 4; break;
    case DIR_DOWN_RIGHT: iy = 4; isRev = 1; break;
    case DIR_TOP_LEFT: iy = 3; isRev = 1; break;
    case DIR_TOP_RIGHT:iy = 3; break;
    }

    switch (player.stepIndex) {
    case 0: ix = 0; break;
    case 1: ix = 1; break;
    case 2: ix = 0; break;
    case 3: ix = 2; break;
    }
    // printf("ix=%d, iy=%d\n", ix, iy);

    //drawSpritePaletted(&data_shadow_bmp,
    //    CENTER_X - data_shadow_bmp.width / 2, CENTER_Y + data_shadow_bmp.height / 2 + 5);

    if (isRev)
        drawSpritePalettedRectRev(&data_lxy_char_bmp, CENTER_X - CHAR_IMG_WIDTH / 2, CENTER_Y - CHAR_IMG_HEIGHT / 2 - 20,
            ix * CHAR_IMG_WIDTH, iy * CHAR_IMG_HEIGHT, CHAR_IMG_WIDTH, CHAR_IMG_HEIGHT);
    else 
        drawSpritePalettedRect(&data_lxy_char_bmp, CENTER_X - CHAR_IMG_WIDTH / 2, CENTER_Y - CHAR_IMG_HEIGHT / 2 - 20,
            ix * CHAR_IMG_WIDTH, iy * CHAR_IMG_HEIGHT, CHAR_IMG_WIDTH, CHAR_IMG_HEIGHT);
}

#ifdef APP_FXCG
int main(void) {
#else
int app(void) {
#endif
    unsigned int key;
    int quit = 0;
    float mp = 7.5;
    float m = mp * 0.8f;

    initGraphApp();

    player.x = 600;
    player.y = 324;
    player.dir = DIR_DOWN_RIGHT;

    while (!quit) {
        allClr();
        redrawCharacter();
        putDisp();

        key = waitKey();
        switch (key) {
        case KEY_CHAR_1: player.dir = DIR_DOWN_LEFT; break;
        case KEY_CHAR_2: player.dir = DIR_DOWN; break;
        case KEY_CHAR_3: player.dir = DIR_DOWN_RIGHT; break;
        case KEY_CHAR_4: player.dir = DIR_LEFT; break;
        case KEY_CHAR_6: player.dir = DIR_RIGHT; break;
        case KEY_CHAR_7: player.dir = DIR_TOP_LEFT; break;
        case KEY_CHAR_8: player.dir = DIR_UP; break;
        case KEY_CHAR_9: player.dir = DIR_TOP_RIGHT; break;
        case KEY_CTRL_EXIT: quit = 1; break;
        default: break;
        }

        switch (player.dir) {
        case DIR_DOWN:      player.y += mp; break;
        case DIR_LEFT:      player.x -= mp; break;
        case DIR_RIGHT:     player.x += mp; break;
        case DIR_UP:        player.y -= mp; break;
        case DIR_DOWN_LEFT: player.y += m; player.x -= m; break;
        case DIR_DOWN_RIGHT:player.y += m; player.x += m; break;
        case DIR_TOP_LEFT:  player.y -= m; player.x -= m; break;
        case DIR_TOP_RIGHT: player.y -= m; player.x += m; break;
        }

        player.stepIndex = (player.stepIndex + 1) & 3;
    }

	return 0;
}
