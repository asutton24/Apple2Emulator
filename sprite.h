#ifndef SPRITE_H
#define SPRITE_H

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned short dbyte;
typedef dbyte Tile[8];
typedef Color Palette[3];
typedef struct animate {
    byte max;
    byte length;
    byte current;
    byte timer;
    byte xlen;
    byte ylen;
    dbyte palIndex;
    dbyte sprIndex;
} Animation;

extern byte res;
extern dbyte screen_width;
extern dbyte screen_height;
extern Tile* spritesheet;
extern Palette* palettes;
extern dbyte spr_length;
extern dbyte pal_length;
extern const char* dict;


void drawTile(Tile s, Palette p, int x, int y, int scale);
void drawSprite(dbyte index, dbyte palIndex, byte xlen, byte ylen, int x, int y);
void displayText(char* text, dbyte palIndex, int x, int y, byte size);
int loadAssets(void);
int freeAssets(void);
void resizeScreen(byte x);
void drawAnimation(Animation *anim, int x, int y);

#endif