#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include <condition_variable>
#include <exception>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/filereadstream.h"

#include "OpenCore/Asset/MapLoader.hpp"
#include "OpenCore/Asset/SoundLoader.hpp"
#include "OpenCore/Asset/TextureLoader.hpp"
#include "OpenCore/Core/Thread/ThreadManager.hpp" // 新增

using std::shared_ptr;
using std::unique_ptr;

// 任务保护（已不再需要，可以移除，但为保持兼容可保留注释）
// struct TaskGuard { ... };

class ResourceManager
{
  public:
    static ResourceManager &getInstance();

    bool Init();
    void CleanUp();

    void LoadMusic(short id, const std::string &path);
    Mix_Music *GetMusic(short id);

    void LoadSound(short id, const std::string &path);
    Mix_Chunk *GetSound(short id);

    void LoadTexture(short id, const std::string &path);
    shared_ptr<SDL_Texture> GetTexture(short id);

    void LoadFont(short id, const std::string &path, int size);
    TTF_Font *GetFont(short id);

    std::future<void> LoadMusicAsync(short id, const std::string &path);
    std::future<void> LoadTextureAsync(short id, const std::string &path);
    std::future<void> LoadFontAsync(short id, const std::string &path,
                                    int size);
    std::future<void> LoadSoundAsync(short id, const std::string &path);

    void ClearAll();
    // 主线程任务处理（现委托给 ThreadManager）
    void ProcessMainThreadTasks();

    // 通过json进行整个场景的资源加载
    std::future<void> LoadResourcesFromJson(short id);

    // 释放加载资源
    void FreeMusic(short id);
    void FreeTexture(short id);
    void FreeFont(short id);
    void FreeSound(short id);

    // 异步释放资源
    std::future<void> FreeMusicAsync(short id);
    std::future<void> FreeTextureAsync(short id);
    std::future<void> FreeFontAsync(short id);
    std::future<void> FreeSoundAsync(short id);

  private:
    SDL_Renderer *renderer = nullptr;

    template <typename F> std::future<void> EnqueueTask(F &&f);

    void ConvertToTexture(short id, SDL_Surface *surface);

    // 资源缓存
    std::mutex musicMutex_;
    std::unordered_map<short, MusicPtr> musicCache_;

    std::mutex soundMutex_;
    std::unordered_map<short, SoundPtr> soundCache_;

    std::mutex textureMutex_;
    std::unordered_map<short, TexturePtr> textureCache_;

    std::mutex fontMutex_;
    std::unordered_map<short, FontPtr> fontCache_;
};

template <typename F> std::future<void> ResourceManager::EnqueueTask(F &&f)
{
    // 直接使用 ThreadManager 提交任务
    return ThreadManager::getInstance().submit(std::forward<F>(f));
}

#endif //_RESOURCE_MANAGER_H_