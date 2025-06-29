#include "modules.h"

void SoftInvertColors(SDL_Surface* surface)
{
    if(SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);

    Uint32* pixels = (Uint32*)surface->pixels;
    int width = surface->w;
    int height = surface->h;
    SDL_PixelFormat* format = surface->format;

    for(int y = 0; y < height;++y)
    {
        for(int x = 0; x < width;++x)
        {
            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, format, &r, &g, &b, &a);
            pixels[y * width + x] = SDL_MapRGBA(format, 255 - r, 255 - g, 255 - b, a);
        }
    }

    if(SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
}

void AlignRect(SDL_Texture* texture, SDL_Rect &rect, int aPos)
{
    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height); // 获取纹理的宽高

    int windowWidth, windowHeight;
    SDL_GetWindowSize(SDL_GetWindowFromID(1), &windowWidth, &windowHeight); // 获取窗口的宽高

    rect.w = width;
    rect.h = height;

    if(aPos & ALIGN_LEFT) rect.x = 0;
    else if(aPos & ALIGN_CENTER) rect.x = (windowWidth - width) / 2;
    else if(aPos & ALIGN_RIGHT) rect.x = windowWidth - width;

    if(aPos & ALIGN_TOP) rect.y = 0;
    else if(aPos & ALIGN_MIDDLE) rect.y = (windowHeight - height) / 2;
    else if(aPos & ALIGN_BOTTOM) rect.y = windowHeight - height;
}


