#include "OpenCore/OpenCore.hpp"

void TextureDeleter::operator()(SDL_Texture *texture) const
{
    if (texture)
        SDL_DestroyTexture(texture);
}

void TextureDeleter::operator()(TTF_Font *font) const
{
    if (font)
        TTF_CloseFont(font);
}
