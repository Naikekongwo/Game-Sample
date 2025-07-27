#include "Music.h"

// 构造函数：初始化音频系统和设置默认音量
MusicSystem::MusicSystem() {
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    volume = MIX_MAX_VOLUME;
}

// 析构函数：清理音频资源并关闭系统
MusicSystem::~MusicSystem() {
    stopBGM();
    for (auto& pair : soundEffects) {
        Mix_FreeChunk(pair.second);
    }
    soundEffects.clear();
    Mix_CloseAudio();
    SDL_Quit();
}

// 加载背景音乐文件
bool MusicSystem::loadBGM(const std::string& path) {
    if (bgm) Mix_FreeMusic(bgm);
    bgm = Mix_LoadMUS(path.c_str());
    return bgm != nullptr;
}

// 开始播放背景音乐
void MusicSystem::playBGM() {
    if (bgm) Mix_PlayMusic(bgm, -1);
}

// 停止并释放背景音乐
void MusicSystem::stopBGM() {
    Mix_HaltMusic();
    if (bgm) {
        Mix_FreeMusic(bgm);
        bgm = nullptr;
    }
}

// 切换到新的背景音乐
bool MusicSystem::changeBGM(const std::string& path) {
    stopBGM();
    if (loadBGM(path)) {
        playBGM();
        return true;
    }
    return false;
}

// 加载音效文件并与名称关联
bool MusicSystem::loadEffect(const std::string& name, const std::string& path) {
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (chunk) {
        soundEffects[name] = chunk;
        return true;
    }
    return false;
}

// 播放指定名称的音效
void MusicSystem::playEffect(const std::string& name) {
    auto it = soundEffects.find(name);
    if (it != soundEffects.end()) {
        Mix_PlayChannel(-1, it->second, 0);
    }
}

// 设置背景音乐的音量
void MusicSystem::setVolume(int volume) {
    this->volume = volume;
    Mix_VolumeMusic(volume);
}

// 获取当前音量值
int MusicSystem::getVolume() const {
    return volume;
}