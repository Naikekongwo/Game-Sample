#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>

#include <memory>
#include <unordered_map>
#include <string>
#include <future>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>


struct TextureLoadTask
{
    bool isFinished = false;
    std::string path;
    std::future<void> future;
    short id;
};



// 事关储存资源的对应的智能指针的对应销毁方法
struct SDLDeleter {
    void operator()(Mix_Music* music) const;
    void operator()(SDL_Texture* texture) const;
};

// 使用智能指针来储存对应的指针，并且都自定义了对应的析构函数（Deleter）
using MusicPtr = std::unique_ptr<Mix_Music, SDLDeleter>;
using TexturePtr = std::unique_ptr<SDL_Texture, SDLDeleter>;


// 资源管理器类
class ResourceManager {
public:
    // 单例
    static ResourceManager& getInstance();

    bool Init(SDL_Renderer* render);

    void CleanUp();

    void LoadMusic(short id, const std::string& path);
    Mix_Music* GetMusic(short id);

    void LoadTexture(short id, const std::string &path);
    SDL_Texture* GetTexture(short id);

    std::future<void> LoadMusicAsync(short id, const std::string& path);
    std::future<void> LoadTextureAsync(short id, const std::string &path);

    void ClearAll();    

private:

    SDL_Renderer* renderer; // 渲染器指针

    void StartWorker();
    void StopQueue();

    // 开始任务的模板
    template<typename F>
    std::future<void> EnqueueTask(F&& f);

    // 音乐锁和储存音乐的无序图
    std::mutex musicMutex_;
    std::unordered_map<short, MusicPtr> musicCache_;

    // 纹理锁和储存纹理的无序图
    std::mutex textureMutex_;
    std::unordered_map<short, TexturePtr> textureCache_;

    std::thread worker_;
    std::atomic<int> ActiveTask_{0};
    std::mutex queueMutex_;
    std::condition_variable queueCV_;
    std::queue<std::function<void()>> taskQueue_;
    bool shouldStop_ = false;
};

template<typename F>
std::future<void> ResourceManager::EnqueueTask(F&& f) {
    auto task = std::make_shared<std::packaged_task<void()>>(std::forward<F>(f));

    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        if (!worker_.joinable()) StartWorker();
        taskQueue_.emplace([task]() { (*task)(); });
    }

    queueCV_.notify_one();
    return task->get_future();
}

#endif // _RESOURCE_MANAGER_H_
