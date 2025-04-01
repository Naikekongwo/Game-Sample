#ifndef _MODULES_H_
#define _MODULES_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

#include "../../macro.h"

enum ALIGN_POS {
    ALIGN_LEFT   = 1 << 0,  // 000001
    ALIGN_CENTER = 1 << 1,  // 000010
    ALIGN_RIGHT  = 1 << 2,  // 000100
    ALIGN_TOP    = 1 << 3,  // 001000
    ALIGN_BOTTOM = 1 << 4,  // 010000
    ALIGN_MIDDLE = 1 << 5   // 100000
};


void SoftInvertColors(SDL_Surface* surface); // 软件反色
void AlignRect(SDL_Texture* texture, SDL_Rect &rect, int aPos); // 控制 rect 和窗口的对齐方式

#endif //_MODULES_H_