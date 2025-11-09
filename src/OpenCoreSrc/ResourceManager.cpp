#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"
// 初始化
#include <SDL2/SDL_image.h>

bool ResourceManager::Init() {
    // 确保GraphicsManager 一定在其之前进行初始化
    renderer = OpenCoreManagers::GFXManager.getRenderer();

    if (!renderer) {
        SDL_Log("ResourceManager::Init() encountered a null renderer.");
        return false;
    }

    int result = Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
    if (!result) {
        SDL_Log("ResourceManager::ResourceManager() failed to initialize SDL_Mixer: %s", Mix_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) < 0)
    {
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
std::shared_ptr<SDL_Texture> ResourceManager::GetTexture(short id) {
    std::lock_guard<std::mutex> lock(textureMutex_);
    auto it = textureCache_.find(id);

    if (it == textureCache_.end()) {
        SDL_Log("ResourceManager::GetTexture failed to get texture id %d", id);
       return nullptr;
    }
    return std::shared_ptr<SDL_Texture>(it->second.get(), [](SDL_Texture*){ /* do nothing, managed by unique_ptr */ });
}

// 异步加载音乐
std::future<void> ResourceManager::LoadMusicAsync(short id, const std::string& path) {
    return EnqueueTask([this, id, path] {
        activeTasks_++;
        LoadMusic(id, path);
        activeTasks_--;
    });
}
//字体加载
void ResourceManager::LoadFont(short id, const std::string& path,int size) {
    std::lock_guard<std::mutex> lock(fontMutex_);
    if (fontCache_.count(id)) return;

    FontPtr font(TTF_OpenFont(path.c_str(), size));
    if (!font) {
        SDL_Log("TTF_OpenFont failed: %s", TTF_GetError());
        return;
    }

    SDL_Log("ResourceManager::LoadFont font id %d loaded successfully.", id);
    fontCache_[id] = std::move(font);
}
//异步字体加载
std::future<void> ResourceManager::LoadFontAsync(short id, const std::string& path,int size) {
    return EnqueueTask([this, id, path,size] {
        activeTasks_++;
        LoadFont(id, path,size);
        activeTasks_--;
    });
}
//获取字体
TTF_Font* ResourceManager::GetFont(short id) {
    std::lock_guard<std::mutex> lock(fontMutex_);
    auto it = fontCache_.find(id);

    if (it == fontCache_.end()) {
        SDL_Log("ResourceManager::GetFont failed to get font id %d", id);
        return nullptr;
    }
    return it->second.get();
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
    SDL_Surface* surface = IMG_Load(path.c_str());
    
    if (!surface) {
        SDL_Log("IMG_Load failed: %s", IMG_GetError());
        return nullptr;
    }
    SDL_Surface* convertedSurface = SDL_ConvertSurfaceFormat(
        surface, 
        SDL_PIXELFORMAT_ABGR8888,  // 或其他需要的格式
        0
    );
    
    SDL_FreeSurface(surface); 
    
    if (!convertedSurface) {
        SDL_Log("SDL_ConvertSurfaceFormat failed: %s", SDL_GetError());
        return nullptr;
    }

    SDL_Log("ResourceManager::LoadSurface() surface loaded successfully.");
    return convertedSurface;
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

void SDLDeleter::operator()(TTF_Font* font) const {
    if (font) TTF_CloseFont(font);
}

//通过json进行整个场景的资源加载
//新增一些说明，大体上结构是这样的，首先有一个main.json,用于储存场景名称与id的对应关系，当然这个main.json在一定程度上是可有可无的
//只是为了方便记忆，即使不用短id修改起来也很方便，只需要修改输入即可，文件的位置在第1步，自行修改即可。对于各个场景的json文件，大致结构如下
//{
//    "1001": {
//        "path": "background.png",
//        "category": "texture"
//    }
//}
//第一行是资源的id，值是一个对象包含路径和种类，种类暂时为texture和music，如需增加chunk等，只需在7.4加入对应分支即可
//如需单独加载某一类型，只需在7.4去掉其余分支即可
std::future<void> ResourceManager::LoadResourcesFromJson(short id) {
    // 1. 构建文件名
    std::string filename = "assets/script/STAGE_" + std::to_string(id) + ".json";
    
    // 2. 检查文件是否存在
    FILE* file = fopen(filename.c_str(), "rb");
    if (!file) {
        SDL_Log("Error: JSON file %s does not exist", filename.c_str());
        // 返回一个已经完成的 future（防止外部阻塞）
        std::promise<void> p;
        p.set_value();
        return p.get_future();
    }

    // 3. 解析JSON文件
    char readBuffer[65536];
    rapidjson::FileReadStream stream(file, readBuffer, sizeof(readBuffer));
    rapidjson::Document doc;
    doc.ParseStream(stream);
    fclose(file);

    // 4. 检查解析错误
    if (doc.HasParseError()) {
        SDL_Log("JSON parse error (%s): %s", filename.c_str(), rapidjson::GetParseError_En(doc.GetParseError()));
        std::promise<void> p;
        p.set_value();
        return p.get_future();
    }

    // 5. 验证JSON根对象
    if (!doc.IsObject()) {
        SDL_Log("Error: JSON root is not an object in %s", filename.c_str());
        std::promise<void> p;
        p.set_value();
        return p.get_future();
    }

    // 6. 存储异步任务future
    std::vector<std::future<void>> futures;

    // 7. 遍历所有资源对象
    for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
        short resourceId;
        try {
            resourceId = static_cast<short>(std::stoi(it->name.GetString()));
        } catch (...) {
            SDL_Log("Invalid resource ID format: %s", it->name.GetString());
            continue;
        }

        const rapidjson::Value& resObj = it->value;
        if (!resObj.IsObject() || 
            !resObj.HasMember("path") || !resObj["path"].IsString() ||
            !resObj.HasMember("category") || !resObj["category"].IsString()) {
            SDL_Log("Invalid resource object for ID: %d", resourceId);
            continue;
        }

        std::string path = resObj["path"].GetString();
        std::string category = resObj["category"].GetString();

        if (category == "music") {
            futures.push_back(LoadMusicAsync(resourceId, path));
        } else if (category == "texture") {
            futures.push_back(LoadTextureAsync(resourceId, path));
        } else if (category == "font") {
            if(!resObj.HasMember("size") || !resObj["size"].IsInt()) {
                SDL_Log("Font resource missing or invalid font size for resource ID: %d", resourceId);
                continue;
            }
            int size = resObj["size"].GetInt();
            futures.push_back(LoadFontAsync(resourceId, path, size));
        } else {
            SDL_Log("Unknown category '%s' for resource ID: %d", category.c_str(), resourceId);
        }
    }

    // 8. 返回一个 future 表示所有任务完成
    return std::async(std::launch::async, [futures = std::move(futures), filename]() mutable {
        for (auto& future : futures) {
            future.wait();
        }
        SDL_Log("ResourceManager: Resources loaded from JSON file %s was finished", filename.c_str());
    });
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
//释放字体
void ResourceManager::FreeFont(short id) {
    std::lock_guard<std::mutex> lock(fontMutex_);
    if (fontCache_.count(id)) {
        TTF_CloseFont(fontCache_[id].get());
        fontCache_.erase(id);
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

std::future<void> ResourceManager::FreeFontAsync(short id) {
    return EnqueueTask([this, id] {
        activeTasks_++;
        FreeFont(id);
        activeTasks_--;
    }); 
}
