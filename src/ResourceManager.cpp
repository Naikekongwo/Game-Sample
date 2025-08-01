#include "OpenCore/ResourceManager.h"
#include "OpenCore/GfxCore.h"
#include <iostream>

void SDLDeleter::operator()(Mix_Music* music) const {
    if (music) Mix_FreeMusic(music);
}

void SDLDeleter::operator()(Mix_Chunk* chunk) const {
    if (chunk) Mix_FreeChunk(chunk);
}

void SDLDeleter::operator()(SDL_Texture* texture) const {
    if (texture) SDL_DestroyTexture(texture);
}

ResourceManager::ResourceManager() {
    Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
}
ResourceManager::~ResourceManager() {
    ClearAll();
    Mix_Quit();
}

ResourceManager& ResourceManager::Get() {
    static ResourceManager instance;
    return instance;
}

void ResourceManager::LoadMusic(short id, const std::string& path) {
    std::lock_guard<std::mutex> lock(musicMutex_);
    if (musicCache_.count(id)) return;

    MusicPtr music(Mix_LoadMUS(path.c_str()));
    if (!music) {
        LogError("Mix_LoadMUS failed: " + std::string(Mix_GetError()));
        return;
    }

    musicCache_[id] = std::move(music);
}

Mix_Music* ResourceManager::GetMusic(short id) {
    std::lock_guard<std::mutex> lock(musicMutex_);
    auto it = musicCache_.find(id);
    return (it != musicCache_.end()) ? it->second.get() : nullptr;
}

void ResourceManager::LoadSound(short id, const std::string& path) {
    std::lock_guard<std::mutex> lock(soundMutex_);
    if (soundCache_.count(id)) return;

    SoundPtr sound(Mix_LoadWAV(path.c_str()));
    if (!sound) {
        LogError("Mix_LoadWAV failed: " + std::string(Mix_GetError()));
        return;
    }

    soundCache_[id] = std::move(sound);
}

Mix_Chunk* ResourceManager::GetSound(short id) {
    std::lock_guard<std::mutex> lock(soundMutex_);
    auto it = soundCache_.find(id);
    return (it != soundCache_.end()) ? it->second.get() : nullptr;
}


void ResourceManager::LoadTexture(short id, const std::string &path) {
    // Placeholder for texture loading logic
    // SDL_Texture* texture = LoadTextureFromFile(path);
    // if (texture) textureCache_[id] = texture;
    std::lock_guard<std::mutex> lock(textureMutex_);
    if(textureCache_.count(id)) return;

    TexturePtr texture(Algorithms::STB_SDL_LOAD(path, renderer));
    if(!texture)
    {
        LogError("STB_SDL_LOAD failed for path: " + path);
        return;
    }

    textureCache_[id] = std::move(texture);
}

SDL_Texture* ResourceManager::GetTexture(short id) {
    std::lock_guard<std::mutex> lock(textureMutex_);
    auto it = textureCache_.find(id);
    return (it != textureCache_.end()) ? it->second.get() : nullptr;
}



std::future<void> ResourceManager::LoadMusicAsync(short id, const std::string& path) {
    return EnqueueTask([this, id, path] {
        LoadMusic(id, path);
    });
}

std::future<void> ResourceManager::LoadSoundAsync(short id, const std::string& path) {
    return EnqueueTask([this, id, path] {
        LoadSound(id, path);
    });
}

std::future<void> ResourceManager::LoadTextureAsync(short id, const std::string &path) {
    return EnqueueTask([this, id, path] {
        LoadTexture(id, path);
    });
}

void ResourceManager::ClearAll() {
    StopQueue();
    {
        std::lock_guard<std::mutex> lock(musicMutex_);
        musicCache_.clear();
    }
    {
        std::lock_guard<std::mutex> lock(soundMutex_);
        soundCache_.clear();
    }
    {
        std::lock_guard<std::mutex> lock(textureMutex_);
        textureCache_.clear();
    }
    std::cout << "All resources cleared.\n";
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

void ResourceManager::LogError(const std::string& msg) {
    std::cerr << "[ResourceManager] ERROR: " << msg << "\n";
}

void ResourceManager::SetRenderer(SDL_Renderer* render) {
    renderer = render;
}
