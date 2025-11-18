#include "SoundEffectManager.hpp"
#include <algorithm>

SoundEffectManager &SoundEffectManager::getInstance()
{
    static SoundEffectManager instance;
    return instance;
}

bool SoundEffectManager::Init(ResourceManager *resManager)
{
    if (!resManager)
    {
        SDL_Log("SFXManager::Init() failed to init, encountering a null resource manager.");
        return false;
    }

    resourceManager = resManager;
    soundEffectRefs.clear();
    playingChannels.clear();
    
    return true;
}

void SoundEffectManager::CleanUp()
{
    stopBGM();
    stopAllSE();
    
    // 注意：这里只清除引用，不释放资源
    soundEffectRefs.clear();
    playingChannels.clear();
}

// 播放音效 - 每次从资源管理器获取最新引用
void SoundEffectManager::playSE(int id, int loops)
{
    if (!resourceManager) {
        SDL_Log("SFXManager::playSE() resource manager is null");
        return;
    }
    
    // 直接从资源管理器获取音效指针
    Mix_Chunk* chunk = resourceManager->GetSound(id);
    if (!chunk) {
        SDL_Log("SFXManager::playSE() failed to get sound effect %d", id);
        return;
    }
    
    // 缓存引用（可选，用于后续音量控制等）
    soundEffectRefs[id] = chunk;
    
    // 播放音效并获取分配的频道
    int channel = Mix_PlayChannel(-1, chunk, loops);
    if (channel != -1) {
        // 记录这个频道正在播放该音效
        playingChannels[id].push_back(channel);
    } else {
        SDL_Log("SFXManager::playSE() failed to play sound effect %d, no available channels", id);
    }
}

// 停止特定音效的所有实例
void SoundEffectManager::stopSE(int id)
{
    auto it = playingChannels.find(id);
    if (it != playingChannels.end()) {
        for (int channel : it->second) {
            Mix_HaltChannel(channel);
        }
        it->second.clear();
    }
}

// 停止所有音效
void SoundEffectManager::stopAllSE()
{
    Mix_HaltChannel(-1); // 停止所有频道
    for (auto& [id, channels] : playingChannels) {
        channels.clear();
    }
}

// 设置特定音效的音量
void SoundEffectManager::setSEVolume(int id, int volume)
{
    // 尝试从缓存获取，如果没有则从资源管理器获取
    Mix_Chunk* chunk = nullptr;
    auto it = soundEffectRefs.find(id);
    if (it != soundEffectRefs.end()) {
        chunk = it->second;
    } else if (resourceManager) {
        chunk = resourceManager->GetSound(id);
        if (chunk) {
            soundEffectRefs[id] = chunk;
        }
    }
    
    if (chunk) {
        Mix_VolumeChunk(chunk, volume);
    }
}

// 设置所有音效的音量
void SoundEffectManager::setAllSEVolume(int volume)
{
    for (auto& [id, chunk] : soundEffectRefs) {
        Mix_VolumeChunk(chunk, volume);
    }
}

bool SoundEffectManager::loadBGM(int id)
{
    bgm = resourceManager->GetMusic(id);
    SDL_Log("SFXManager::loadBGM() loading music from memory %d", (bgm != nullptr));
    return bgm != nullptr;
}

void SoundEffectManager::playBGM()
{
    if (bgm)
        Mix_PlayMusic(bgm, -1);
}

void SoundEffectManager::stopBGM()
{
    Mix_HaltMusic();
    if (bgm)
    {
        bgm = nullptr;
    }
}

bool SoundEffectManager::changeBGM(short id)
{
    stopBGM();
    if (loadBGM(id))
    {
        playBGM();
        return true;
    }
    return false;
}

void SoundEffectManager::setVolume(int volume)
{
    this->volume = volume;
    Mix_VolumeMusic(volume);
}

int SoundEffectManager::getVolume() const
{
    return volume;
}