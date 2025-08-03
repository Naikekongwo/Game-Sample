#ifndef SOUNDEFFECT_H
#define SOUNDEFFECT_H

#include <string>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>


// 前向声明资源管理器类
// 这样是为了引用资源管理器类造成的循环依赖问题
class ResourceManager;

class SoundEffectManager {
public:

    // 单例函数
    static SoundEffectManager& getInstance();

    // 手动的初始化函数
    bool Init(ResourceManager* resManager);

    // 手动的销毁函数
    void CleanUp();

    void playBGM();//播放背景音乐
    void stopBGM();//停止背景音乐
    bool changeBGM(short id);//切换背景音乐
    //控制音效
   
    //控制音量
    void setVolume(int volume);//设置音量
    int getVolume() const;//获取音量

    bool loadBGM(int id); // 从资源系统加载背景音乐 [SHAOYANG]

   

private:
    Mix_Music* bgm = nullptr;//背景音乐
    int volume = 128;// 音量值，范围0-128

    ResourceManager* resourceManager; // 自带的资源系统指针 [SHAOYANG]
};

#endif // SOUNDEFFECT_H