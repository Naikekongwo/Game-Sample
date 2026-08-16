#pragma once

// AudioManager.hpp
// 项目级音频管理器（SDL3_mixer）
//
// 新版 OpenCore 已移除音频模块，本项目在项目层以 SDL3_mixer 重新实现，
// 接口与旧引擎的 SoundEffectManager 保持一致（loadBGM/playBGM/playSE/…）。

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

struct SDL_AudioSpec;
struct MIX_Mixer;
struct MIX_Track;

namespace Eclipsea
{

class AudioManager
{
  public:
    static AudioManager &getInstance();

    // 初始化 / 清理（由 Eclipsea::StartUp / CleanUp 调用）
    bool Init();
    void CleanUp();

    // 注册数字 ID → 音频文件路径
    void registerMusic(short id, std::string_view path);
    void registerSound(short id, std::string_view path);

    // 背景音乐
    bool loadBGM(short id);
    void playBGM();
    void stopBGM();
    bool changeBGM(short id);

    // 音效
    void playSE(int id, int loops = 0);
    void stopSE(int id);
    void stopAllSE();

    // 音量（0~128，兼容旧引擎语义；内部换算为 0.0~1.0 增益）
    void setVolume(int volume);
    void setSEVolume(int id, int volume);
    void setAllSEVolume(int volume);
    int  getVolume() const { return currentBGMVolume; }

  private:
    AudioManager() = default;
    ~AudioManager() = default;
    AudioManager(const AudioManager &) = delete;
    AudioManager &operator=(const AudioManager &) = delete;

    MIX_Track *ensureTrack(short id, bool isMusic);

    MIX_Mixer  *mixer          = nullptr;
    MIX_Track  *bgmTrack       = nullptr;
    MIX_Track  *bgmAudioRef    = nullptr; // 当前 BGM 音频（防止提前释放）
    bool        bgmReady       = false;
    int         currentBGMVolume = 128;

    std::unordered_map<short, std::string>            musicPaths_;
    std::unordered_map<short, std::string>            soundPaths_;
    std::unordered_map<short, MIX_Track *>            soundTracks_;
    std::unordered_map<short, void *>                 soundAudioRefs_;
};

} // namespace Eclipsea
