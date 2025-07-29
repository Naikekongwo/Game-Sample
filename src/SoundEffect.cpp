#include "OpenCore/SoundEffect.h"


// 构造函数：初始化音频系统和设置默认音量
SoundEffectManager::SoundEffectManager(ResourceManager* res) {
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    volume = MIX_MAX_VOLUME;
    resourceManager = res; // 保存资源管理器指针
}

// 析构函数：清理音频资源并关闭系统
SoundEffectManager::~SoundEffectManager() {
    stopBGM();
    for (auto& pair : soundEffects) {
        Mix_FreeChunk(pair.second);
    }
    soundEffects.clear();
    Mix_CloseAudio();
    SDL_Quit();
}

// 加载背景音乐文件
bool SoundEffectManager::loadBGM(const std::string& path) {
    if (bgm) Mix_FreeMusic(bgm);
    bgm = Mix_LoadMUS(path.c_str());
    return bgm != nullptr;
}

// [SHAOYANG] 从资源系统中加载音乐
bool SoundEffectManager::loadBGMR(int id)
{
    if(bgm) Mix_FreeMusic(bgm);
    bgm = resourceManager->GetMusic(id);
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
        Mix_FreeMusic(bgm);
        bgm = nullptr;
    }
}

// 切换到新的背景音乐
bool SoundEffectManager::changeBGM(const std::string& path) {
    stopBGM();
    if (loadBGM(path)) {
        playBGM();
        return true;
    }
    return false;
}

// 加载音效文件并与名称关联
bool SoundEffectManager::loadEffect(const std::string& name, const std::string& path) {
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (chunk) {
        soundEffects[name] = chunk;
        return true;
    }
    return false;
}

// 从资源中加载音效
bool SoundEffectManager::loadEffectR(int id, const std::string& name) {
    Mix_Chunk* chunk = resourceManager->GetSound(id);
    if (chunk) {
        soundEffects[name] = chunk;
        return true;
    }
    return false;
}

// 播放指定名称的音效
void SoundEffectManager::playEffect(const std::string& name) {
    auto it = soundEffects.find(name);
    if (it != soundEffects.end()) {
        Mix_PlayChannel(-1, it->second, 0);
    }
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