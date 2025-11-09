#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"

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
    return true;
}

void SoundEffectManager::CleanUp()
{
    stopBGM();
}

// [SHAOYANG] 从资源系统中加载音乐
bool SoundEffectManager::loadBGM(int id)
{
    bgm = resourceManager->GetMusic(id);
    SDL_Log("SFXManager::loadBGM() loading music from memory %d", (bgm != nullptr));
    return bgm != nullptr;
}

// 开始播放背景音乐
void SoundEffectManager::playBGM()
{
    if (bgm)
        Mix_PlayMusic(bgm, -1);
}

// 停止并释放背景音乐
void SoundEffectManager::stopBGM()
{
    Mix_HaltMusic();
    if (bgm)
    {
        bgm = nullptr;
    }
}

// 切换到新的背景音乐
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

// 设置背景音乐的音量
void SoundEffectManager::setVolume(int volume)
{
    this->volume = volume;
    Mix_VolumeMusic(volume);
}

// 获取当前音量值
int SoundEffectManager::getVolume() const
{
    return volume;
}