#ifndef SOUNDEFFECT_H
#define SOUNDEFFECT_H

#include "OpenCore/ResourceManager.h"

#include <string>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

class SoundEffectManager {
public:
    //构造函数
    SoundEffectManager(ResourceManager *res);
    ~SoundEffectManager();
    //控制音乐播放
    bool loadBGM(const std::string& path);//加载背景音乐
    void playBGM();//播放背景音乐
    void stopBGM();//停止背景音乐
    bool changeBGM(const std::string& path);//切换背景音乐
    //控制音效
    bool loadEffect(const std::string& name, const std::string& path);//加载音效
    void playEffect(const std::string& name);//播放音效
    //控制音量
    void setVolume(int volume);//设置音量
    int getVolume() const;//获取音量

    bool loadBGMR(int id); // 从资源系统加载背景音乐 [SHAOYANG]
    bool loadEffectR(int id, const std::string& name); // 从资源系统加载音效 [SHAOYANG]
   

private:
    Mix_Music* bgm = nullptr;//背景音乐
    std::map<std::string, Mix_Chunk*> soundEffects;//音效
    int volume = 128;// 音量值，范围0-128

    ResourceManager* resourceManager; // 自带的资源系统指针 [SHAOYANG]
};

#endif // SOUNDEFFECT_H