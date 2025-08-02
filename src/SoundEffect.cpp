#include "OpenCore/OpenCore.h"

// 构造函数：初始化音频系统和设置默认音量
SoundEffectManager::SoundEffectManager(ResourceManager* res) {
    resourceManager = res; // 保存资源管理器指针
}

// 析构函数：停止播放
// 注意，资源的销毁权利交至 resourceManager
SoundEffectManager::~SoundEffectManager() {
    stopBGM();
}

// [SHAOYANG] 从资源系统中加载音乐
bool SoundEffectManager::loadBGM(int id)
{
    bgm = resourceManager->GetMusic(id);
    SDL_Log("SFXManager::loadBGM() loading music from memory %d", (bgm!=nullptr));
    return bgm != nullptr;
}

// 开始播放背景音乐
void SoundEffectManager::playBGM() {
    if (bgm) Mix_PlayMusic(bgm, -1);
}

// 停止并释放背景音乐
void SoundEffectManager::stopBGM() {
    Mix_HaltMusic();
    if (bgm) {
        bgm = nullptr;
    }
}

// 切换到新的背景音乐
bool SoundEffectManager::changeBGM(short id) {
    stopBGM();
    if (loadBGM(id)) {
        playBGM();
        return true;
    }
    return false;
}


// 设置背景音乐的音量
void SoundEffectManager::setVolume(int volume) {
    this->volume = volume;
    Mix_VolumeMusic(volume);
}

// 获取当前音量值
int SoundEffectManager::getVolume() const {
    return volume;
}