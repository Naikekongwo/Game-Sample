#ifndef SOUNDEFFECT_H
#define SOUNDEFFECT_H

#include <string>
#include <map>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>

// 前向声明资源管理器类
class ResourceManager;

class SoundEffectManager {
public:
    // 单例函数
    static SoundEffectManager& getInstance();

    // 手动的初始化函数
    bool Init(ResourceManager* resManager);

    // 手动的销毁函数
    void CleanUp();

    // 背景音乐控制
    void playBGM();//播放背景音乐
    void stopBGM();//停止背景音乐
    bool changeBGM(short id);//切换背景音乐
    
    // 音效控制
    void playSE(int id, int loops = 0); // 播放音效，loops为循环次数(0=播放一次，-1=无限循环)
    void stopSE(int id); // 停止特定音效的所有实例
    void stopAllSE(); // 停止所有音效
    
    // 音量控制
    void setVolume(int volume);//设置背景音乐音量
    void setSEVolume(int id, int volume); // 设置特定音效的音量
    void setAllSEVolume(int volume); // 设置所有音效的音量
    int getVolume() const;//获取背景音乐音量

    bool loadBGM(int id); // 从资源系统加载背景音乐

private:
    Mix_Music* bgm = nullptr;//背景音乐
    int volume = 128;// 背景音乐音量值，范围0-128
    
    // 音效管理 - 使用原始指针，不管理生命周期
    std::map<int, Mix_Chunk*> soundEffectRefs; // 存储音效的原始指针引用
    std::map<int, std::vector<int>> playingChannels; // 跟踪每个音效ID正在播放的频道
    
    ResourceManager* resourceManager; // 自带的资源系统指针

    // 私有构造函数和拷贝控制
    SoundEffectManager() = default;
    ~SoundEffectManager() = default;
    SoundEffectManager(const SoundEffectManager&) = delete;
    SoundEffectManager& operator=(const SoundEffectManager&) = delete;
};

#endif // SOUNDEFFECT_H