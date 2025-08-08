#include "OpenCore/OpenCore.hpp"
// 初始化
#define STB_IMAGE_IMPLEMENTATION
#include "STB-IMAGE/stb_image.h"

bool ResourceManager::Init(SDL_Renderer* render) {
    // 确保GraphicsManager 一定在其之前进行初始化
    renderer = OpenCoreManagers::GFXManager.getRenderer();

    if (!render) {
        SDL_Log("ResourceManager::Init() encountered a null renderer.");
        return false;
    }
    this->renderer = render;

    int result = Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
    if (!result) {
        SDL_Log("ResourceManager::ResourceManager() failed to initialize SDL_Mixer: %s", Mix_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("ResourceManager::ResourceManager() failed to open audio: %s", Mix_GetError());
        return false;
    }

    SDL_Log("ResourceManager::ResourceManager() SDL_Mixer initialized successfully.");
    return true;
}

// 清理
void ResourceManager::CleanUp() { 
    ClearAll();
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();
    renderer = nullptr;
}

// 单例
ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

// 音乐加载同步
void ResourceManager::LoadMusic(short id, const std::string& path) {
    std::lock_guard<std::mutex> lock(musicMutex_);
    if (musicCache_.count(id)) return;

    MusicPtr music(Mix_LoadMUS(path.c_str()));
    if (!music) {
        SDL_Log("Mix_LoadMUS failed: %s", Mix_GetError());
        return;
    }

    SDL_Log("ResourceManager::LoadMusic music id %d loaded successfully.", id);
    musicCache_[id] = std::move(music);
}

// 获取音乐
Mix_Music* ResourceManager::GetMusic(short id) {
    std::lock_guard<std::mutex> lock(musicMutex_);
    auto it = musicCache_.find(id);

    if (it == musicCache_.end()) {
        SDL_Log("ResourceManager::GetMusic failed to get music id %d", id);
        return nullptr;
    }
    return it->second.get();
}

// 纹理加载同步
void ResourceManager::LoadTexture(short id, const std::string &path) {
    // 检查是否已加载
    {
        std::lock_guard<std::mutex> lock(textureMutex_);
        if (textureCache_.count(id)) return;
    }

    // 加载表面
    SDL_Surface* surface = LoadSurface(path);
    if (!surface) {
        SDL_Log("ResourceManager::LoadTexture failed to load surface for id: %d", id);
        return;
    }

    // 转换纹理
    ConvertSurfaceToTexture(id, surface);
}

// 获取纹理
SDL_Texture* ResourceManager::GetTexture(short id) {
    std::lock_guard<std::mutex> lock(textureMutex_);
    auto it = textureCache_.find(id);

    if (it == textureCache_.end()) {
        SDL_Log("ResourceManager::GetTexture failed to get texture id %d", id);
       return nullptr;
    }
    return it->second.get();
}

// 异步加载音乐
std::future<void> ResourceManager::LoadMusicAsync(short id, const std::string& path) {
    return EnqueueTask([this, id, path] {
        activeTasks_++;
        LoadMusic(id, path);
        activeTasks_--;
    });
}

// 异步加载纹理
std::future<void> ResourceManager::LoadTextureAsync(short id, const std::string &path) {
    // 创建promise用于通知任务完成
    auto promise = std::make_shared<std::promise<void>>();
    std::future<void> future = promise->get_future();

    EnqueueTask([this, id, path, promise] {
    activeTasks_++;

        // 1. 在后台线程加载表面
        SDL_Surface* surface = LoadSurface(path);
        if (!surface) {
            activeTasks_--;
            promise->set_exception(std::make_exception_ptr(std::runtime_error("LoadSurface failed")));
            return;
        }

        // 2. 创建主线程转换任务
        {
            std::lock_guard<std::mutex> lock(mainThreadQueueMutex_);
            mainThreadTaskQueue_.push([this, id, surface, promise] {
                ConvertSurfaceToTexture(id, surface);   
                promise->set_value(); // 通知任务完成
                activeTasks_--;
            });
        }
    });

    return future;
}

// 清理所有资源
void ResourceManager::ClearAll() {
    SDL_Log("ResourceManager::ClearAll() started");
    StopWorker();
        
    // 处理剩余的主线程任务
    ProcessMainThreadTasks();  
    // 等待所有任务完成
    while (activeTasks_.load() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    SDL_Log("ResourceManager::ClearAll() stopped task queue successfully");
        
    {
        std::lock_guard<std::mutex> lock(musicMutex_);
        SDL_Log("ResourceManager::ClearAll() clearing music cache, count=%d", 
                static_cast<int>(musicCache_.size()));
        musicCache_.clear();
        SDL_Log("ResourceManager::ClearAll() cleared music cache");
    }
                
    {
        std::lock_guard<std::mutex> lock(textureMutex_);
        SDL_Log("ResourceManager::ClearAll() clearing texture cache, count=%d", 
                static_cast<int>(textureCache_.size()));
        textureCache_.clear();
        SDL_Log("ResourceManager::ClearAll() cleared texture cache");
    }
            
    SDL_Log("ResourceManager::ClearAll() finished");
}

// 处理主线程任务
void ResourceManager::ProcessMainThreadTasks() {
    std::queue<std::function<void()>> tasks;
    {
        std::lock_guard<std::mutex> lock(mainThreadQueueMutex_);
        if (mainThreadTaskQueue_.empty()) return;
        tasks = std::move(mainThreadTaskQueue_);
        // 清空原队列
        while (!mainThreadTaskQueue_.empty()) mainThreadTaskQueue_.pop();
    }

    while (!tasks.empty()) {
        tasks.front()();
        tasks.pop();
    }
}

// 启动工作线程
void ResourceManager::StartWorker() {
    worker_ = std::thread([this] {
        while (true) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(queueMutex_);
                queueCV_.wait(lock, [this] {
                    return shouldStop_ || !taskQueue_.empty();
                });

                if (shouldStop_ && taskQueue_.empty()) {
                    return;
                }

                task = std::move(taskQueue_.front());
                taskQueue_.pop();
            }

            task();
        }
    });
}

// 停止工作线程
void ResourceManager::StopWorker() {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        shouldStop_ = true;
    }
    
    queueCV_.notify_all();

    if (worker_.joinable()) {
        worker_.join();
    }
}

// 加载表面（线程安全）
SDL_Surface* ResourceManager::LoadSurface(const std::string& path) {
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (!data) {
        SDL_Log("stbi_load failed: %s", stbi_failure_reason());
        return nullptr;
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
        data, width, height, 32, width * 4,
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
    );

    if (!surface) {
        stbi_image_free(data);
        return nullptr;
    }

    SDL_Log("ResourceManager::LoadSurface() surface loaded.");
    return surface;
}

// 转换表面为纹理（必须在主线程）
void ResourceManager::ConvertSurfaceToTexture(short id, SDL_Surface* surface) {
    if (!renderer) {
        SDL_Log("ResourceManager::ConvertSurfaceToTexture: renderer is null");
        SDL_FreeSurface(surface);
        return;
    }
    
    // 创建纹理
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        return;
    }

    // 存储纹理
    std::lock_guard<std::mutex> lock(textureMutex_);
    if (textureCache_.count(id)) {
        SDL_DestroyTexture(texture);
        SDL_Log("ResourceManager::ConvertSurfaceToTexture: texture id %d already exists", id);
        return;
    }

    textureCache_[id] = TexturePtr(texture);
    SDL_Log("ResourceManager: texture id %d converted and stored", id);
}

// SDL对象删除器
void SDLDeleter::operator()(Mix_Music* music) const {
    if (music) Mix_FreeMusic(music);
}

void SDLDeleter::operator()(SDL_Texture* texture) const {
    if (texture) SDL_DestroyTexture(texture);
}

//通过json进行整个场景的资源加载
void ResourceManager::LoadResourcesFromJson(short id) {
    // 1. 构建文件名
    std::string filename = "../json" + std::to_string(id) + ".json";
    
    // 2. 检查文件是否存在
    FILE* file = fopen(filename.c_str(), "rb");
    if (!file) {
        std::cerr << "Error: JSON file " << filename << " does not exist" << std::endl;
        return;
    }

    // 3. 解析JSON文件
    char readBuffer[65536];
    rapidjson::FileReadStream stream(file, readBuffer, sizeof(readBuffer));
    rapidjson::Document doc;
    doc.ParseStream(stream);
    fclose(file);  // 关闭文件

    // 4. 检查解析错误
    if (doc.HasParseError()) {
        std::cerr << "JSON parse error (" << filename << "): " 
                  << rapidjson::GetParseError_En(doc.GetParseError()) << std::endl;
        return;
    }

    // 5. 验证JSON根对象
    if (!doc.IsObject()) {
        std::cerr << "Error: JSON root is not an object in " << filename << std::endl;
        return;
    }

    // 6. 存储异步任务future
    std::vector<std::future<void>> futures;

    // 7. 遍历所有资源对象
    for (rapidjson::Value::ConstMemberIterator it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
        // 7.1 获取资源ID
        short resourceId;
        try {
            resourceId = static_cast<short>(std::stoi(it->name.GetString()));
        } catch (...) {
            std::cerr << "Invalid resource ID format: " << it->name.GetString() << std::endl;
            continue;
        }

        // 7.2 验证资源对象结构
        const rapidjson::Value& resObj = it->value;
        if (!resObj.IsObject() || 
            !resObj.HasMember("path") || !resObj["path"].IsString() ||
            !resObj.HasMember("category") || !resObj["category"].IsString()) {
            std::cerr << "Invalid resource object for ID: " << resourceId << std::endl;
            continue;
        }

        // 7.3 提取路径和类别
        std::string path = resObj["path"].GetString();
        std::string category = resObj["category"].GetString();

        // 7.4 根据类别调用加载函数
        if (category == "music") {
            futures.push_back(LoadMusicAsync(resourceId, path));
        } else if (category == "texture") {
            futures.push_back(LoadTextureAsync(resourceId, path));
        } else {
            std::cerr << "Unknown category '" << category << "' for resource ID: " << resourceId << std::endl;
        }
    }

    // 8. 异步等待所有加载任务完成
    if (!futures.empty()) {
        std::thread([futures = std::move(futures)]() mutable {
            for (auto& future : futures) {
                future.wait();  // 等待每个异步任务完成
            }
        }).detach();  // 分离线程避免阻塞主线程
    }
}

//释放音乐
void ResourceManager::FreeMusic(short id) {
    std::lock_guard<std::mutex> lock(musicMutex_);
    if (musicCache_.count(id)) {
        Mix_FreeMusic(musicCache_[id].get());
        musicCache_.erase(id);
    }
}

//释放纹理
void ResourceManager::FreeTexture(short id) {
    std::lock_guard<std::mutex> lock(textureMutex_);
    if (textureCache_.count(id)) {
        SDL_DestroyTexture(textureCache_[id].get());
        textureCache_.erase(id);
    }
}

//异步释放资源
std::future<void> ResourceManager::FreeMusicAsync(short id) {
    return EnqueueTask([this, id] {
        activeTasks_++;
        FreeMusic(id);
        activeTasks_--;
    });
}

std::future<void> ResourceManager::FreeTextureAsync(short id) {
    return EnqueueTask([this, id] {
        activeTasks_++;
        FreeTexture(id);
        activeTasks_--;
    });
}







