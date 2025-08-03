#include "OpenCOre/OpenCore.h"


bool ResourceManager::Init(SDL_Renderer* render)
{
    if(!render) 
    {
        SDL_Log("ResourceManager::Init() encountered a null renderer.");
        return false;
    }
    this->renderer = render;

    bool result = Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
    
    // 输出初始化结果
    if(!result) SDL_Log("ResourceManager::ResourceManager() failed to initialize SDL_Mixer");
    else SDL_Log("ResourceManager::ResourceManager() SDL_Mixer was initialized successfully.");

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    return true;
}

void SDLDeleter::operator()(Mix_Music* music) const {
    if (music) Mix_FreeMusic(music);
}

void SDLDeleter::operator()(SDL_Texture* texture) const {
    if (texture) SDL_DestroyTexture(texture);
}

void ResourceManager::CleanUp() { 
    ClearAll();
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();

    renderer = nullptr;

    const char* errr = Mix_GetError();
    
    if(errr && *errr)
        SDL_Log("ResourceManager::~ResourceManager() failed to clear memory or quit the SDL_Mixer %s", errr);    
}

ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}


// LoadMusic 加载音乐的同步函数
void ResourceManager::LoadMusic(short id, const std::string& path) {
    std::lock_guard<std::mutex> lock(musicMutex_);
    if (musicCache_.count(id)) return;

    MusicPtr music(Mix_LoadMUS(path.c_str()));
    if (!music) {
        SDL_Log("Mix_LoadMUS failed: %s", std::string(Mix_GetError()));
        return;
    }

    // SDL_Log 输出相关日志
    SDL_Log("ResourceManager::LoadMusic music id %d loaded successfully.", id);
    musicCache_[id] = std::move(music);
}

// GetMusic 从资源库中获取音乐
Mix_Music* ResourceManager::GetMusic(short id) {
    std::lock_guard<std::mutex> lock(musicMutex_);
    auto it = musicCache_.find(id);

    // 报错输出日志
    if(it == musicCache_.end()) SDL_Log("ResourceManger::GetMusic failed to get music id %d", id);
    return (it != musicCache_.end()) ? it->second.get() : nullptr;
}

// LoadTexture 加载纹理的同步函数
void ResourceManager::LoadTexture(short id, const std::string &path) {
    std::lock_guard<std::mutex> lock(textureMutex_);
    if(textureCache_.count(id)) return;

    TexturePtr texture(Algorithms::STB_SDL_LOAD(path, renderer));
    if(!texture)
    {
        SDL_Log("ResourceManager::LoadTexture failed to load texture id: %d", id);
        return;
    }

    // SDL_Log 输出相关日志
    SDL_Log("ResourceManager::LoadTexture loaded texture id %d", id);
    textureCache_[id] = std::move(texture);
}

// GetTexture 从资源库中获取纹理
SDL_Texture* ResourceManager::GetTexture(short id) {
    std::lock_guard<std::mutex> lock(textureMutex_);
    auto it = textureCache_.find(id);

    if(it == textureCache_.end()) SDL_Log("ResourceManager::GetTexture failed to get texture id %d", id);
    return (it != textureCache_.end()) ? it->second.get() : nullptr;
}


// LoadMusicAsync 加载音乐的异步函数
std::future<void> ResourceManager::LoadMusicAsync(short id, const std::string& path) {
    return EnqueueTask([this, id, path] {
        ActiveTask_++;
        LoadMusic(id, path);
        ActiveTask_--;
    });
}

// LoadTextureAsync 加载纹理的异步函数
std::future<void> ResourceManager::LoadTextureAsync(short id, const std::string &path) {
    return EnqueueTask([this, id, path] {
        ActiveTask_++;
        LoadTexture(id, path);
        ActiveTask_--;
    });
}

void ResourceManager::ClearAll() {
    SDL_Log("ResourceManager::ClearAll() the clear process started.");
    StopQueue();
    while (ActiveTask_.load() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    SDL_Log("ResourceManager::CLearAll() Stopped the task queue successfully.");

    {
        std::lock_guard<std::mutex> lock(musicMutex_);
        SDL_Log("ResourceManager::CLearAll() Clearing music cache, count=%d", (int)musicCache_.size());
        musicCache_.clear();
        SDL_Log("ResourceManager::ClearAll() successfully cleared all the music.");
    }
    {
        std::lock_guard<std::mutex> lock(textureMutex_);
        SDL_Log("ResourceManager::ClearAll() Clearing texture cache, count=%d", (int)textureCache_.size());
        textureCache_.clear();
        SDL_Log("ResourceManager::ClearAll() Texture cache cleared");
    }
    SDL_Log("ResourceManager::ClearAll() the clear process has finished");
}


void ResourceManager::StartWorker() {
    worker_ = std::thread([this] {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queueMutex_);
                queueCV_.wait(lock, [this] {
                    return !taskQueue_.empty() || shouldStop_;
                });

                if (shouldStop_ && taskQueue_.empty()) return;

                task = std::move(taskQueue_.front());
                taskQueue_.pop();
            }
            task();
        }
    });
}

void ResourceManager::StopQueue() {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        shouldStop_ = true;
    }
    queueCV_.notify_all();
    if (worker_.joinable()) worker_.join();
}