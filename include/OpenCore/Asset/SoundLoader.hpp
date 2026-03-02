#pragma once

// SoundLoader.hpp
// 音乐加载工具

#include <SDL2/SDL_mixer.h>

#include <memory>
#include <string>

using std::string;
using std::unique_ptr;

struct SoundDeleter
{
    void operator()(Mix_Chunk *chunk) const;
    void operator()(Mix_Music *music) const;
};

using SoundPtr = unique_ptr<Mix_Chunk, SoundDeleter>;
using MusicPtr = unique_ptr<Mix_Music, SoundDeleter>;

inline SoundPtr LoadSoundFromFile(const std::string &path)
{

    SoundPtr sound(Mix_LoadWAV(path.c_str()));

    return std::move(sound);
}

inline MusicPtr LoadMusicFromFile(const std::string &path)
{
    MusicPtr music(Mix_LoadMUS(path.c_str()));

    return std::move(music);
}