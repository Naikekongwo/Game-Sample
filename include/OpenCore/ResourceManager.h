#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include <SDL2/SDL_mixer.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <future>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>

struct SDLDeleter {
    void operator()(Mix_Music* music) const;
    void operator()(Mix_Chunk* chunk) const;
};

using MusicPtr = std::unique_ptr<Mix_Music, SDLDeleter>;
using SoundPtr = std::unique_ptr<Mix_Chunk, SDLDeleter>;

class ResourceManager {
public:
    static ResourceManager& Get();

    void LoadMusic(short id, const std::string& path);
    Mix_Music* GetMusic(short id);

    void LoadSound(short id, const std::string& path);
    Mix_Chunk* GetSound(short id);

    std::future<void> LoadMusicAsync(short id, const std::string& path);
    std::future<void> LoadSoundAsync(short id, const std::string& path);

    void ClearAll();

private:
    ResourceManager();
    ~ResourceManager();

    void LogError(const std::string& msg);
    void StartWorker();
    void StopQueue();

    template<typename F>
    std::future<void> EnqueueTask(F&& f);

    std::mutex musicMutex_;
    std::unordered_map<short, MusicPtr> musicCache_;

    std::mutex soundMutex_;
    std::unordered_map<short, SoundPtr> soundCache_;

    std::thread worker_;
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
