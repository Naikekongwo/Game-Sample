#include "OpenCore/OpenCore.hpp"

void SoundDeleter::operator()(Mix_Music *music) const
{
    if (music)
        Mix_FreeMusic(music);
}

void SoundDeleter::operator()(Mix_Chunk *chunk) const
{
    if (chunk)
        Mix_FreeChunk(chunk);
}