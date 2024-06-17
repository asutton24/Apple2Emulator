#include "sprite.h"

typedef unsigned char byte;
typedef unsigned short dbyte;
typedef dbyte Tile[8];
typedef Color Palette[3];

byte res = 1;
dbyte screen_width = 280;
dbyte screen_height = 250;
Tile* spritesheet;
Palette* palettes;
dbyte spr_length;
dbyte pal_length;
const char* dict = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.!?:;,\"\'+-*/%%_()<>=#";

void drawTile(Tile s, Palette p, int x, int y, int scale){
    dbyte mask = 0xC000;
    byte length = 0;
    dbyte val = 0;
    dbyte last = 0;
    x *= res;
    y *= res;
    int factor = res * scale;
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            val = s[i] & mask;
            val = val >> (2 * (7 - j));
            if (length == 0){
                last = val;
                length++;
            } else if (last == val) {
                length++;
            } else {
                if (last){
                    DrawRectangle(x + (j - length) * factor, y + i * factor, length * factor, factor, p[last - 1]);
                }
                last = val;
                length = 1;
            }
            mask = mask >> 2;
        }
        if (length > 0 && last){
            DrawRectangle(x + (8 - length) * factor, y + i * factor, length * factor, factor, p[last - 1]);
            length = 0;
        }
        mask = 0xC000;
    }
}

void drawSprite(dbyte index, dbyte palIndex, byte xlen, byte ylen, int x, int y){
    if (index >= spr_length || palIndex >= pal_length){
        return;
    }
    for (int i = 0; i < ylen; i++){
        for (int j = 0; j < xlen; j++){
            drawTile(spritesheet[index], palettes[palIndex], x + j * 8 * res, y + i * 8 * res, 1);
            index += 1;
            if (index == spr_length){
                return;
            }
        }
    }
}

void displayText(char* text, dbyte palIndex, int x, int y, byte size){
    int index = 0;
    int dictIndex;
    int scale = size;
    size *= 8;
    int ox = x;
    while (text[index] != '\0'){
        if (text[index] == '\n'){
            y += size;
            x = ox;
        } else if (text[index] == ' '){
            x += size;
        } else {
            dictIndex = 0;
            while (dictIndex < 56 && text[index] != dict[dictIndex]){
                dictIndex += 1;
            }
            if (dictIndex != 56){
                drawTile(spritesheet[dictIndex], palettes[palIndex], x, y, scale);
                x += size;
            }
        }
        index += 1;
    }
}

int loadAssets(){
    FILE *file;
    byte *temp;
    dbyte *storage;
    int index = 0;
    file = fopen("spritesheet.bin", "rb");
    if (file == NULL){
        return -1;
    }
    storage = (dbyte*)(malloc(sizeof(dbyte)));
    fread(storage, 2, 1, file);
    spr_length = storage[0];
    spritesheet = (Tile*)malloc(spr_length * sizeof(Tile));
    fread(spritesheet, sizeof(Tile), spr_length, file);
    file = fopen("palettes.bin", "rb");
    fread(storage, 2, 1, file);
    pal_length = storage[0];
    free(storage);
    palettes = (Palette*)malloc(pal_length * sizeof(Palette));
    temp = (byte*)malloc(pal_length * 9);
    fread(temp, 1, pal_length * 9, file);
    for (int i = 0; i < pal_length; i++){
        for (int j = 0; j < 3; j++){
            index = 9 * i + 3 * j;
            palettes[i][j] = (Color){temp[index], temp[index + 1], temp[index + 2], 255};
        }
    }
    free(temp);
    file = NULL;
    return 0;
}

int freeAssets(){
    free(spritesheet);
    free(palettes);
    return 0;
}

void resizeScreen(byte x){
    if (0 < x && 5 > x){
        res = x;
        screen_width = 280 * res;
        screen_height = 250 * res;
    }
    SetWindowSize(screen_width, screen_height);
}

void drawAnimation(Animation *anim, int x, int y){
    drawSprite((*anim).current, (*anim).palIndex, (*anim).xlen, (*anim).ylen, x, y);
    (*anim).timer -= 1;
    if ((*anim).timer == 0){
        (*anim).timer = (*anim).max;
        (*anim).current += (*anim).xlen * (*anim).ylen;
        if ((*anim).current == (*anim).sprIndex + (*anim).length * (*anim).xlen * (*anim).ylen){
            (*anim).current = (*anim).sprIndex;
        }
    }
}