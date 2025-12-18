#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <memory>
#include <unordered_map>
#include <string>
#include <future>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include <exception>
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/error/en.h"


// 对应的删除器
struct SDLDeleter
{
    void operator()(Mix_Music *music) const;
    void operator()(SDL_Texture *texture) const;
    void operator()(TTF_Font *font) const;
    void operator()(Mix_Chunk *chunk) const;
};

using MusicPtr = std::unique_ptr<Mix_Music, SDLDeleter>;
using TexturePtr = std::unique_ptr<SDL_Texture, SDLDeleter>;
using FontPtr = std::unique_ptr<TTF_Font, SDLDeleter>;
using SoundPtr = std::unique_ptr<Mix_Chunk, SDLDeleter>;

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
    std::shared_ptr<SDL_Texture> GetTexture(short id);

    void LoadFont(short id, const std::string &path, int size);
    TTF_Font *GetFont(short id);

    std::future<void> LoadMusicAsync(short id, const std::string &path);
    std::future<void> LoadTextureAsync(short id, const std::string &path);
    std::future<void> LoadFontAsync(short id, const std::string &path, int size);
    std::future<void> LoadSoundAsync(short id, const std::string &path);

    void ClearAll();
    // 新增：主线程任务处理
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

    void StartWorker();
    void StopWorker();

    template <typename F>
    std::future<void> EnqueueTask(F &&f);

    // 主线程任务队列
    std::mutex mainThreadQueueMutex_;
    std::queue<std::function<void()>> mainThreadTaskQueue_;

    // 新增测试：加载表面和纹理转换,这个正常不应该在此类中实现
    SDL_Surface *LoadSurface(const std::string &path);
    void ConvertSurfaceToTexture(short id, SDL_Surface *surface);

    // 资源缓存
    std::mutex musicMutex_;
    std::unordered_map<short, MusicPtr> musicCache_;

    std::mutex soundMutex_;
    std::unordered_map<short, SoundPtr> soundCache_;

    std::mutex textureMutex_;
    std::unordered_map<short, TexturePtr> textureCache_;

    std::mutex fontMutex_;
    std::unordered_map<short, FontPtr> fontCache_;

    // 工作线程
    std::thread worker_;
    std::atomic<int> activeTasks_{0};
    std::mutex queueMutex_;
    std::condition_variable queueCV_;
    std::queue<std::function<void()>> taskQueue_;
    std::atomic<bool> shouldStop_{false};
};

template <typename F>
std::future<void> ResourceManager::EnqueueTask(F &&f)
{
    auto task = std::make_shared<std::packaged_task<void()>>(std::forward<F>(f));

    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        if (!worker_.joinable())
            StartWorker();
        taskQueue_.emplace([task]()
                           { (*task)(); });
    }

    queueCV_.notify_one();
    return task->get_future();
};

#endif //_RESOURCE_MANAGER_H_