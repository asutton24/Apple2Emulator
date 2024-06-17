#include "raylib.h"
#include "sprite.h"
#include "6502emu.h" 

enum mode {TEXT, COLOR};
enum gmode {HIRES, LORES};
enum version {ORIGINAL, PLUS};
byte graphics;
byte flashMode = 0;
double flashClock;
Color colors[16];
Color hiResColors[6];
byte mono = 0;
const Color green = (Color){41, 245, 43, 255};
byte mixed = 1;
byte hiResPage = 0;
byte model;
byte graphicsMode = LORES;

int getAscii(){
    bool shiftMode = false;
    char* nums = ")!@#$%^&*(";
    if (IsKeyDown(KEY_LEFT_SHIFT)){
        shiftMode = true;
    }
    for (int i = 65; i < 91; i++){
        if (IsKeyDown(i)){
            return i;
        }
    }
    for (int i = 48; i < 58; i++){
        if (IsKeyDown(i)){
            if (shiftMode){
                return nums[i - 48];
            }
            return i;
        }
    }
    if (IsKeyDown(KEY_SEMICOLON)){
        if (shiftMode){
            return ':';
        }
        return ';';
    }
    if (IsKeyDown(KEY_ENTER)){
        return 13;
    }
    if (IsKeyDown(KEY_APOSTROPHE)){
        if (shiftMode){
            return '\"';
        }
        return '\'';
    }
    if (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_RIGHT)){
        return ' ';
    }
    if (IsKeyDown(KEY_LEFT_BRACKET)){
        return '[';
    }
    if (IsKeyDown(KEY_RIGHT_BRACKET)){
        return ']';
    }
    if (IsKeyDown(KEY_EQUAL)){
        if (shiftMode){
            return '+';
        }
        return '=';
    }
    if (IsKeyDown(KEY_PERIOD)){
        if (shiftMode){
            return '>';
        }
        return '.';
    }
    if (IsKeyDown(KEY_COMMA)){
        if (shiftMode){
            return '<';
        }
        return ',';
    }
    if (IsKeyDown(KEY_MINUS)){
        if (shiftMode){
            return '_';
        }
        return '-';
    }
    if (IsKeyDown(KEY_SLASH)){
        if (shiftMode){
            return '?';
        }
        return '/';
    }
    if (IsKeyDown(KEY_BACKSLASH)){
        return '\\';
    }
    if (IsKeyDown(KEY_LEFT)){
        return 8;
    }
    return -1;
}

void drawHiresMixed(){
    dbyte low = 0x2000 + 0x2000 * hiResPage;
    int row = -1;
    dbyte line;
    byte mode1;
    byte mode2;
    dbyte active;
    byte textMode;
    for (int i = 0; i < 3840; i++){
        if (i % 20 == 0){
            row++;
            if (((row % 3) * 64 + ((row / 3) % 8) * 8 + row / 24) >= 160){
                i += 19;
                continue;
            }
        }
        line = mem[low + 2 * i + (row / 3) * 8];
        mode1 = (line & 128) >> 7;
        mode2 = (mem[low + 2 * i + 1 + (row / 3) * 8] & 128) >> 7;
        line += (mem[low + 2 * i + 1 + (row / 3) * 8] & 127) << 7;
        if (mono){
            for (int j = 0; j < 14; j++){
                active = line & 1;
                if (active){
                    DrawRectangle(((i % 20) * 14 + j) * res, ((row % 3) * 64 + ((row / 3) % 8) * 8 + row / 24) * res, res, res, green);
                }
                line = line >> 1;
            }
        } else {
            for (int j = 0; j < 7; j++){
                active = line & 3;
                if (active == 3 || active == 0){
                    DrawRectangle(((i % 20) * 14 + j * 2) * res, ((row % 3) * 64 + ((row / 3) % 8) * 8 + row / 24) * res, 2 * res, res, hiResColors[active]);
                } else if (j < 3) {
                    DrawRectangle(((i % 20) * 14 + j * 2) * res, ((row % 3) * 64 + ((row / 3) % 8) * 8 + row / 24) * res, 2 * res, res, hiResColors[active + 2 * mode1]);
                } else if (j > 3) {
                    DrawRectangle(((i % 20) * 14 + j * 2) * res, ((row % 3) * 64 + ((row / 3) % 8) * 8 + row / 24) * res, 2 * res, res, hiResColors[active + 2 * mode2]);
                } else if (active == 1){
                    DrawRectangle(((i % 20) * 14 + j * 2) * res, ((row % 3) * 64 + ((row / 3) % 8) * 8 + row / 24) * res, 2 * res, res, hiResColors[active + 2 * mode2]);
                } else {
                    DrawRectangle(((i % 20) * 14 + j * 2) * res, ((row % 3) * 64 + ((row / 3) % 8) * 8 + row / 24) * res, 2 * res, res, hiResColors[active + 2 * mode1]);
                }
                line = line >> 2;
            }
        }
    }
    row = 11;
    for (int i = 0; i < 160; i++){
        if (i % 40 == 0){
            row += 3;
        }
        textMode = mem[1024 + (row * 40) + (i % 40) + (row / 3) * 8];
        if (textMode / 64 == 0){
            drawSprite(textMode % 64, 1 + 2 * mono, 1, 1, (i % 40) * 7, (row % 3) * 64 + (row / 3) * 8);
        } else if (textMode / 64 == 1){
            drawSprite(textMode % 64, flashMode + 2 * mono, 1, 1, (i % 40) * 7, (row % 3) * 64 + (row / 3) * 8);
        } else {
            drawSprite(textMode % 64, 2 * mono, 1, 1, (i % 40) * 7, (row % 3) * 64 + (row / 3) * 8);
        }
    }
}

void drawHiresScreen(){
    dbyte low = 0x2000 + 0x2000 * hiResPage;
    int row = -1;
    dbyte line;
    byte mode1;
    byte mode2;
    dbyte active;
    for (int i = 0; i < 3840; i++){
        if (i % 20 == 0){
            row++;
        }
        line = mem[low + 2 * i + (row / 3) * 8];
        mode1 = (line & 128) >> 7;
        line = line & 127;
        mode2 = (mem[low + 2 * i + 1 + (row / 3) * 8] & 128) >> 7;
        line += (mem[low + 2 * i + 1 + (row / 3) * 8] & 127) << 7;
        if (mono){
            for (int j = 0; j < 14; j++){
                active = line & 1;
                if (active){
                    DrawRectangle(((i % 20) * 14 + j) * res, ((row % 3) * 64 + row / 3) * res, res, res, green);
                }
                line = line >> 1;
            }
        } else {
            for (int j = 0; j < 7; j++){
                active = line & 3;
                if (active == 3 || active == 0){
                    DrawRectangle(((i % 20) * 14 + j * 2) * res, ((row % 3) * 64 + row / 3) * res, 2 * res, res, hiResColors[active]);
                } else if (j < 3) {
                    DrawRectangle(((i % 20) * 14 + j * 2) * res, ((row % 3) * 64 + row / 3) * res, 2 * res, res, hiResColors[active + 2 * mode1]);
                } else if (j > 3) {
                    DrawRectangle(((i % 20) * 14 + j * 2) * res, ((row % 3) * 64 + row / 3) * res, 2 * res, res, hiResColors[active + 2 * mode2]);
                } else if (active == 1){
                    DrawRectangle(((i % 20) * 14 + j * 2) * res, ((row % 3) * 64 + row / 3) * res, 2 * res, res, hiResColors[active + 2 * mode2]);
                } else {
                    DrawRectangle(((i % 20) * 14 + j * 2) * res, ((row % 3) * 64 + row / 3) * res, 2 * res, res, hiResColors[active + 2 * mode1]);
                }
                line = line >> 2;
            }
        }
    }
}

void drawLoresScreen(){
    int row = -1;
    byte col1;
    byte col2;
    for (int i = 0; i < 960; i++){
        if (i % 40 == 0){
            row++;
        }
        col1 = mem[1024 + i + (row / 3) * 8] / 16;
        col2 = mem[1024 + i + (row / 3) * 8] % 16;
        if (mono){
            if (col2){
                DrawRectangle((i % 40) * 7 * res, ((row % 3) * 64 + (row / 3) * 8) * res, 7 * res, 4 * res, green);
            }
            if (col1){
                DrawRectangle((i % 40) * 7 * res, ((row % 3) * 64 + (row / 3) * 8 + 4) * res, 7 * res, 4 * res, green);
            }
        } else {
            DrawRectangle((i % 40) * 7 * res, ((row % 3) * 64 + (row / 3) * 8) * res, 7 * res, 4 * res, colors[col2]);
            DrawRectangle((i % 40) * 7 * res, ((row % 3) * 64 + (row / 3) * 8 + 4) * res, 7 * res, 4 * res, colors[col1]);
        }
    }
}

void drawMixedScreen(){
    int row = -1;
    byte textMode;
    byte col1;
    byte col2;
    for (int i = 0; i < 960; i++){
        if (i % 40 == 0){
            row++;
        }
        textMode = mem[1024 + i + (row / 3) * 8];
        if (row == 14 || row == 17 || row == 20 || row == 23){
            if (textMode / 64 == 0){
                drawSprite(textMode % 64, 1 + 2 * mono, 1, 1, (i % 40) * 7, (row % 3) * 64 + (row / 3) * 8);
            } else if (textMode / 64 == 1){
                drawSprite(textMode % 64, flashMode + 2 * mono, 1, 1, (i % 40) * 7, (row % 3) * 64 + (row / 3) * 8);
            } else {
                drawSprite(textMode % 64, 2 * mono, 1, 1, (i % 40) * 7, (row % 3) * 64 + (row / 3) * 8);
            }
        } else {
            col1 = textMode / 16;
            col2 = textMode % 16;
            if (mono){
                if (col2){
                    DrawRectangle((i % 40) * 7 * res, ((row % 3) * 64 + (row / 3) * 8) * res, 7 * res, 4 * res, green);
                }
                if (col1){
                    DrawRectangle((i % 40) * 7 * res, ((row % 3) * 64 + (row / 3) * 8 + 4) * res, 7 * res, 4 * res, green);
                }
            } else {
                DrawRectangle((i % 40) * 7 * res, ((row % 3) * 64 + (row / 3) * 8) * res, 7 * res, 4 * res, colors[col2]);
                DrawRectangle((i % 40) * 7 * res, ((row % 3) * 64 + (row / 3) * 8 + 4) * res, 7 * res, 4 * res, colors[col1]);
            }
        }
    }
}

void drawTextScreen(){
    int row = -1;
    byte textMode;
    for (int i = 0; i < 960; i++){
        if (i % 40 == 0){
            row++;
        }
        textMode = mem[1024 + i + (row / 3) * 8];
        if (textMode / 64 == 0){
            drawSprite(textMode % 64, 1 + 2 * mono, 1, 1, (i % 40) * 7, (row % 3) * 64 + (row / 3) * 8);
        } else if (textMode / 64 == 1){
            drawSprite(textMode % 64, flashMode + 2 * mono, 1, 1, (i % 40) * 7, (row % 3) * 64 + (row / 3) * 8);
        } else {
            drawSprite(textMode % 64, 2 * mono, 1, 1, (i % 40) * 7, (row % 3) * 64 + (row / 3) * 8);
        }
    }
}

int getKeyDown(){
    for (int i = 0; i < 258; i++){
        if (IsKeyDown(i)) return i;
    }
    return -1;
}

void softSwitches(){
    switch(lastAccess){
        case (0xC010):
            mem[0xC000] = mem[0xC000] & 127;
            break;
        case (0xC051):
            graphics = TEXT;
            break;
        case (0xC052):
            mixed = 0;
            break;
        case (0xC053):
            mixed = 1;
            break;
        case (0xC050):
            graphics = COLOR;
            break;
        case (0xC054):
            hiResPage = 0;
            break;
        case (0xC055):
            hiResPage = 1;
        case (0xC056):
            graphicsMode = LORES;
            break;
        case (0xC057):
            graphicsMode = HIRES;
            break;
        case (-1):
            return;
    }
}

int main(){
    InitWindow(280, 250, "APPLE 2");
    //dbyte *trace = (dbyte*)malloc(sizeof(dbyte) * 5000);
    int count = 0;
    dbyte breakpoint = 0;
    model = PLUS;
    switch (model){
        case (ORIGINAL):
            loadData("apple2.rom", 0xD000, 0xFFFF, 1);
            break;
        case (PLUS):
            loadData("apple2plus.rom", 0xD000, 0xFFFF, 1);
            break;
    }
    reset();
    loadAssets();
    resizeScreen(2);
    colors[0] = BLACK;
    colors[1] = (Color){153, 3, 95, 255};
    colors[2] = (Color){66, 4, 225, 255};
    colors[3] = (Color){202, 19, 254, 255};
    colors[4] = (Color){0, 115, 16, 255};
    colors[5] = (Color){127, 127, 127, 255};
    colors[6] = (Color){36, 151, 255, 255};
    colors[7] = (Color){170, 162, 255, 255};
    colors[8] = (Color){79, 81, 1, 255};
    colors[9] = (Color){240, 92, 0, 255};
    colors[10] = (Color){190, 190, 190, 255};
    colors[11] = (Color){255, 133, 225, 255};
    colors[12] = (Color){18, 202, 7, 255};
    colors[13] = (Color){206, 212, 19, 255};
    colors[14] = (Color){81, 245, 149, 255};
    colors[15] = WHITE;
    hiResColors[0] = BLACK;
    hiResColors[1] = (Color){67, 195, 0, 255};
    hiResColors[2] = (Color){182, 61, 255, 255};
    hiResColors[3] = WHITE;
    hiResColors[4] = (Color){234, 93, 21, 255};
    hiResColors[5] = (Color){16, 164, 227, 255};
    const double refresh = 1.0 / 60.0;
    double cycle = 1.0 / 1024768.0;
    double wait;
    double curTime;
    double time2;
    int key;
    int check = 0;
    byte monoCheck = 1;
    graphics = TEXT;
    flashClock = GetTime();
    while (!WindowShouldClose()){
        BeginDrawing();
        curTime = GetTime();
        ClearBackground(BLACK);
        if (curTime - flashClock >= .5){
            flashClock = curTime;
            if (flashMode == 0){
                flashMode = 1;
            } else {
                flashMode = 0;
            }
        }
        switch (graphics){
            case (TEXT):
                drawTextScreen();
                break;
            case (COLOR):
                switch (graphicsMode){
                    case (LORES):
                        if (mixed){
                            drawMixedScreen();
                            break;
                        }
                        drawLoresScreen();
                        break;
                    case (HIRES):
                        if (mixed){
                            drawHiresMixed();
                            break;
                        }
                        drawHiresScreen();
                        break;
                }
                break;
        } 
        DrawText(TextFormat("PC: %.04X A: %.02X X: %.02X Y: %.02X F: %.02X\n\n%d", pgc, a, x, y, flags, count), 10 * res, 210 * res, 10 * res, RED);
        EndDrawing();
        key = getAscii();
        if (IsKeyDown(KEY_F1)){
            if (monoCheck){
                monoCheck = 0;
                if (mono == 0){
                    mono = 1;
                } else {
                    mono = 0;
                }
            }
        } else {
            monoCheck = 1;
        }
        if (IsKeyDown(KEY_F2)){
            reset();
        }
        if (key != -1 && check >= 8){
            mem[0xC000] = key + 128;
            mem[0xC010] = key + 128;
            check = 0;
        } 
        check++;
        while (GetTime() - curTime < refresh){
            time2 = GetTime();
            runcmd();
            if (!breakpoint && pgc != 0xFCAA && pgc != 0xFCAC) count++;
            if (pgc == 0xFD1B) breakpoint = 1;
            //DrawText(TextFormat("PC: %.04X A: %.02X X: %.02X Y: %.02X F: %.02X SLOW\n %.02X %.02X", pgc, a, x, y, flags, mem[0x4C], mem[0x4D]), 10 * res, 210 * res, 10 * res, RED);
            softSwitches();
            wait = cycles * cycle;
            while (GetTime() - time2 < wait){
                continue;
            }
        }
    }
    freeAssets();
    return 0;
}