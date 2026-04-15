/**
 * @file SoundLoader.hpp
 * @brief 音效与音乐加载工具模块。
 * 
 * 提供从文件加载音效（Mix_Chunk）和音乐（Mix_Music）的功能，
 * 并定义了相应的智能指针删除器，确保资源自动释放。
 */
#pragma once



#include <SDL2/SDL_mixer.h>

#include <memory>
#include <string>

using std::string;
using std::unique_ptr;
/**
 * @struct SoundDeleter
 * @brief Mix_Chunk 和 Mix_Music 的智能指针删除器。
 * 
 * 用于 unique_ptr 的自定义删除器，确保音效块和音乐资源被正确释放。
 */
struct SoundDeleter
{
    void operator()(Mix_Chunk *chunk) const;
    void operator()(Mix_Music *music) const;
};

using SoundPtr = unique_ptr<Mix_Chunk, SoundDeleter>;
using MusicPtr = unique_ptr<Mix_Music, SoundDeleter>;

/**
 * @brief 从 WAV 文件加载音效。
 * 
 * 调用 Mix_LoadWAV 加载指定路径的音效文件，返回一个 SoundPtr 智能指针。
 * 若加载失败，返回的 SoundPtr 为空（nullptr）。
 * 
 * @param path 音效文件路径（通常为 .wav 格式）。
 * @return SoundPtr 包含 Mix_Chunk 的智能指针，失败时为空。
 * 
 * @note 调用者无需手动释放，SoundPtr 离开作用域时会自动调用 Mix_FreeChunk。
 */
inline SoundPtr LoadSoundFromFile(const std::string &path)
{

    SoundPtr sound(Mix_LoadWAV(path.c_str()));

    return std::move(sound);
}
/**
 * @brief 从文件加载音乐。
 * 
 * 调用 Mix_LoadMUS 加载指定路径的音乐文件（支持 MP3、WAV 等格式），
 * 返回一个 MusicPtr 智能指针。若加载失败，返回的 MusicPtr 为空。
 * 
 * @param path 音乐文件路径。
 * @return MusicPtr 包含 Mix_Music 的智能指针，失败时为空。
 * 
 * @note 调用者无需手动释放，MusicPtr 离开作用域时会自动调用 Mix_FreeMusic。
 */
inline MusicPtr LoadMusicFromFile(const std::string &path)
{
    MusicPtr music(Mix_LoadMUS(path.c_str()));

    return std::move(music);
}