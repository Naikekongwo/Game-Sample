#include "Eclipsea/Core/AudioManager.hpp"

#include "OpenCore.hpp"
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <utility>

namespace Eclipsea
{

AudioManager &AudioManager::getInstance()
{
    static AudioManager instance;
    return instance;
}

bool AudioManager::Init()
{
    if (!MIX_Init())
    {
        LOG("AudioManager::Init() MIX_Init failed: {}", SDL_GetError());
        return false;
    }

    SDL_AudioSpec spec;
    SDL_zero(spec);
    spec.format   = SDL_AUDIO_S16;
    spec.channels = 2;
    spec.freq     = 44100;

    mixer = MIX_CreateMixer(&spec);
    if (!mixer)
    {
        LOG("AudioManager::Init() MIX_CreateMixer failed: {}", SDL_GetError());
        return false;
    }

    bgmTrack = MIX_CreateTrack(mixer);
    if (!bgmTrack)
    {
        LOG("AudioManager::Init() failed to create BGM track: {}",
            SDL_GetError());
        return false;
    }

    LOG("AudioManager::Init() success");
    return true;
}

void AudioManager::CleanUp()
{
    stopAllSE();
    stopBGM();

    if (bgmTrack)
    {
        MIX_DestroyTrack(bgmTrack);
        bgmTrack = nullptr;
    }

    for (auto &[id, track] : soundTracks_)
    {
        if (track)
            MIX_DestroyTrack(track);
    }
    soundTracks_.clear();

    if (mixer)
    {
        MIX_DestroyMixer(mixer);
        mixer = nullptr;
    }

    MIX_Quit();
}

void AudioManager::registerMusic(short id, std::string_view path)
{
    musicPaths_[id] = std::string(path);
}

void AudioManager::registerSound(short id, std::string_view path)
{
    soundPaths_[id] = std::string(path);
}

MIX_Track *AudioManager::ensureTrack(short id, bool isMusic)
{
    if (isMusic)
        return bgmTrack;

    auto it = soundTracks_.find(id);
    if (it != soundTracks_.end())
        return it->second;

    MIX_Track *track = MIX_CreateTrack(mixer);
    if (track)
        soundTracks_[id] = track;
    return track;
}

bool AudioManager::loadBGM(short id)
{
    if (!mixer || !bgmTrack)
        return false;

    auto it = musicPaths_.find(id);
    if (it == musicPaths_.end())
    {
        LOG("AudioManager::loadBGM() 未注册的BGM ID {}", id);
        return false;
    }

    MIX_Audio *audio = MIX_LoadAudio(mixer, it->second.c_str(), true);
    if (!audio)
    {
        LOG("AudioManager::loadBGM() 加载失败 {}: {}", it->second,
            SDL_GetError());
        return false;
    }

    if (!MIX_SetTrackAudio(bgmTrack, audio))
    {
        LOG("AudioManager::loadBGM() 绑定音频失败: {}", SDL_GetError());
        MIX_DestroyAudio(audio);
        return false;
    }

    bgmReady = true;
    return true;
}

void AudioManager::playBGM()
{
    if (!bgmReady || !bgmTrack)
        return;

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1); // 无限循环
    MIX_PlayTrack(bgmTrack, props);
    SDL_DestroyProperties(props);
}

void AudioManager::stopBGM()
{
    if (bgmTrack)
        MIX_StopTrack(bgmTrack, 0);
    bgmReady = false;
}

bool AudioManager::changeBGM(short id)
{
    stopBGM();
    if (loadBGM(id))
    {
        playBGM();
        return true;
    }
    return false;
}

void AudioManager::playSE(int id, int loops)
{
    if (!mixer)
        return;

    auto it = soundPaths_.find(id);
    if (it == soundPaths_.end())
    {
        LOG("AudioManager::playSE() 未注册的音效 ID {}", id);
        return;
    }

    MIX_Track *track = ensureTrack(id, false);
    if (!track)
        return;

    MIX_Audio *audio = MIX_LoadAudio(mixer, it->second.c_str(), false);
    if (!audio)
    {
        LOG("AudioManager::playSE() 加载失败 {}: {}", it->second,
            SDL_GetError());
        return;
    }

    MIX_SetTrackAudio(track, audio);

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
    MIX_PlayTrack(track, props);
    SDL_DestroyProperties(props);
}

void AudioManager::stopSE(int id)
{
    auto it = soundTracks_.find(id);
    if (it != soundTracks_.end() && it->second)
        MIX_StopTrack(it->second, 0);
}

void AudioManager::stopAllSE()
{
    if (mixer)
        MIX_StopAllTracks(mixer, 0);
}

void AudioManager::setVolume(int volume)
{
    currentBGMVolume = volume;
    if (bgmTrack)
    {
        float gain = static_cast<float>(volume) / 128.0f;
        if (gain < 0.0f)
            gain = 0.0f;
        if (gain > 1.0f)
            gain = 1.0f;
        MIX_SetTrackGain(bgmTrack, gain);
    }
}

void AudioManager::setSEVolume(int id, int volume)
{
    auto it = soundTracks_.find(id);
    if (it == soundTracks_.end() || !it->second)
        return;

    float gain = static_cast<float>(volume) / 128.0f;
    if (gain < 0.0f)
        gain = 0.0f;
    if (gain > 1.0f)
        gain = 1.0f;
    MIX_SetTrackGain(it->second, gain);
}

void AudioManager::setAllSEVolume(int volume)
{
    float gain = static_cast<float>(volume) / 128.0f;
    if (gain < 0.0f)
        gain = 0.0f;
    if (gain > 1.0f)
        gain = 1.0f;

    for (auto &[id, track] : soundTracks_)
    {
        if (track)
            MIX_SetTrackGain(track, gain);
    }
}

} // namespace Eclipsea
