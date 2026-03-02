
#include "OpenCore/OpenCore.hpp"

// 初始化
#include <SDL2/SDL_image.h>

bool ResourceManager::Init()
{
    // 确保GraphicsManager 一定在其之前进行初始化
    renderer = OpenCoreManagers::GFXManager.getRenderer();

    if (!renderer)
    {
        Console_Log("ResourceManager::Init() encountered a null renderer.");
        return false;
    }

    int result = Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
    if (!result)
    {
        Console_Log("ResourceManager::ResourceManager() failed to initialize "
                    "SDL_Mixer: %s",
                    Mix_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) <
        0)
    {
        Console_Log(
            "ResourceManager::ResourceManager() failed to open audio: %s",
            Mix_GetError());
        return false;
    }

    Console_Log("ResourceManager::ResourceManager() SDL_Mixer initialized "
                "successfully.");

    return true;
}

// 清理
void ResourceManager::CleanUp()
{
    ClearAll();
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();
    renderer = nullptr;
}

// 单例
ResourceManager &ResourceManager::getInstance()
{
    static ResourceManager instance;
    return instance;
}

// 音乐加载同步
void ResourceManager::LoadMusic(short id, const std::string &path)
{
    std::lock_guard<std::mutex> lock(musicMutex_);
    if (musicCache_.count(id))
        return;

    MusicPtr music = LoadMusicFromFile(path);
    if (!music)
    {
        Console_Log("Mix_LoadMUS failed: %s", Mix_GetError());
        return;
    }

    Console_Log("ResourceManager::LoadMusic music id %d loaded successfully.",
                id);
    musicCache_[id] = std::move(music);
}

// 获取音乐
Mix_Music *ResourceManager::GetMusic(short id)
{
    std::lock_guard<std::mutex> lock(musicMutex_);
    auto it = musicCache_.find(id);

    if (it == musicCache_.end())
    {
        Console_Log("ResourceManager::GetMusic failed to get music id %d", id);
        return nullptr;
    }
    return it->second.get();
}

// 音效加载同步
void ResourceManager::LoadSound(short id, const std::string &path)
{

    std::lock_guard<std::mutex> lock(soundMutex_);
    if (soundCache_.count(id))
        return;

    SoundPtr sound = std::move(LoadSoundFromFile(path));
    if (!sound)
    {
        Console_Log("Mix_LoadWAV failed: %s", Mix_GetError());
        return;
    }

    Console_Log("ResourceManager::LoadSound sound id %d loaded successfully.",
                id);

    soundCache_[id] = std::move(sound);
}

// 获取音效
Mix_Chunk *ResourceManager::GetSound(short id)
{
    std::lock_guard<std::mutex> lock(soundMutex_);
    auto it = soundCache_.find(id);

    if (it == soundCache_.end())
    {
        Console_Log("ResourceManager::GetSound failed to get sound id %d", id);
        return nullptr;
    }
    return it->second.get();
}

// 纹理加载同步
void ResourceManager::LoadTexture(short id, const std::string &path)
{
    // 检查是否已加载
    {
        std::lock_guard<std::mutex> lock(textureMutex_);
        if (textureCache_.count(id))
            return;
    }

    // 加载表面
    SDL_Surface *surface = LoadSurface(path);
    if (!surface)
    {
        Console_Log(
            "ResourceManager::LoadTexture failed to load surface for id: %d",
            id);
        return;
    }

    // 转换纹理
    ConvertToTexture(id, surface);
}

// 获取纹理
shared_ptr<SDL_Texture> ResourceManager::GetTexture(short id)
{
    std::lock_guard<std::mutex> lock(textureMutex_);
    auto it = textureCache_.find(id);

    if (it == textureCache_.end())
    {
        Console_Log("ResourceManager::GetTexture failed to get texture id %d",
                    id);
        return nullptr;
    }
    return shared_ptr<SDL_Texture>(
        it->second.get(),
        [](SDL_Texture *) { /* do nothing, managed by unique_ptr */ });
}

// 异步加载音乐
std::future<void> ResourceManager::LoadMusicAsync(short id,
                                                  const std::string &path)
{
    return EnqueueTask(
        [this, id, path]
        {
            TaskGuard guard(activeTasks_);
            LoadMusic(id, path);
        });
}
// 异步加载音效
std::future<void> ResourceManager::LoadSoundAsync(short id,
                                                  const std::string &path)
{

    return EnqueueTask(
        [this, id, path]
        {
            TaskGuard guard(activeTasks_);
            LoadSound(id, path);
        });
}
// 字体加载
void ResourceManager::LoadFont(short id, const std::string &path, int size)
{
    std::lock_guard<std::mutex> lock(fontMutex_);
    if (fontCache_.count(id))
        return;

    FontPtr font(TTF_OpenFont(path.c_str(), size));
    if (!font)
    {
        Console_Log("TTF_OpenFont failed: %s", TTF_GetError());
        return;
    }

    Console_Log("ResourceManager::LoadFont font id %d loaded successfully.",
                id);
    fontCache_[id] = std::move(font);
}
// 异步字体加载
std::future<void>
ResourceManager::LoadFontAsync(short id, const std::string &path, int size)
{
    return EnqueueTask(
        [this, id, path, size]
        {
            TaskGuard guard(activeTasks_);
            LoadFont(id, path, size);
        });
}
// 获取字体
TTF_Font *ResourceManager::GetFont(short id)
{
    std::lock_guard<std::mutex> lock(fontMutex_);
    auto it = fontCache_.find(id);

    if (it == fontCache_.end())
    {
        Console_Log("ResourceManager::GetFont failed to get font id %d", id);
        return nullptr;
    }
    return it->second.get();
}
// 异步加载纹理
std::future<void> ResourceManager::LoadTextureAsync(short id,
                                                    const std::string &path)
{
    auto promise = std::make_shared<std::promise<void>>();
    std::future<void> future = promise->get_future();

    EnqueueTask(
        [this, id, path, promise]
        {
            // 在这里不递增 activeTasks_，等到主线程真正执行纹理转换时再递增
            SDL_Surface *surface = nullptr;
            try
            {
                // 1. 在后台线程加载表面
                surface = LoadSurface(path);
                if (!surface)
                {
                    throw std::runtime_error("LoadSurface failed");
                }
            }
            catch (...)
            {
                promise->set_exception(std::current_exception());
                return;
            }

            // 2. 将实际纹理创建任务放到主线程队列
            std::lock_guard<std::mutex> lock(mainThreadQueueMutex_);
            mainThreadTaskQueue_.push(
                [this, id, surface, promise]
                {
                    // activeTasks_ 在实际执行点递增/递减，保证异常安全
                    struct TaskGuard
                    {
                        std::atomic<int> &counter;
                        TaskGuard(std::atomic<int> &c) : counter(c)
                        {
                            counter++;
                        }
                        ~TaskGuard() { counter--; }
                    } guard(activeTasks_);

                    try
                    {
                        ConvertToTexture(id, surface);
                        promise->set_value();
                    }
                    catch (...)
                    {
                        promise->set_exception(std::current_exception());
                    }
                });
        });

    return future;
}

// 清理所有资源
void ResourceManager::ClearAll()
{
    Console_Log("ResourceManager::ClearAll() started");
    StopWorker();

    // 处理剩余的主线程任务
    ProcessMainThreadTasks();
    // 等待所有任务完成
    while (activeTasks_.load() > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    Console_Log("ResourceManager::ClearAll() stopped task queue successfully");

    {
        std::lock_guard<std::mutex> lock(musicMutex_);
        Console_Log(
            "ResourceManager::ClearAll() clearing music cache, count=%d",
            static_cast<int>(musicCache_.size()));
        musicCache_.clear();
        Console_Log("ResourceManager::ClearAll() cleared music cache");
    }

    {
        std::lock_guard<std::mutex> lock(textureMutex_);
        Console_Log(
            "ResourceManager::ClearAll() clearing texture cache, count=%d",
            static_cast<int>(textureCache_.size()));
        textureCache_.clear();
        Console_Log("ResourceManager::ClearAll() cleared texture cache");
    }

    Console_Log("ResourceManager::ClearAll() finished");
}

// 处理主线程任务
void ResourceManager::ProcessMainThreadTasks()
{
    std::queue<std::function<void()>> tasks;
    {
        std::lock_guard<std::mutex> lock(mainThreadQueueMutex_);
        if (mainThreadTaskQueue_.empty())
            return;
        tasks = std::move(mainThreadTaskQueue_);
        // 清空原队列
        while (!mainThreadTaskQueue_.empty())
            mainThreadTaskQueue_.pop();
    }

    while (!tasks.empty())
    {
        tasks.front()();
        tasks.pop();
    }
}

// 启动工作线程
void ResourceManager::StartWorker()
{
    worker_ = std::thread(
        [this]
        {
            while (true)
            {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(queueMutex_);
                    queueCV_.wait(
                        lock,
                        [this] { return shouldStop_ || !taskQueue_.empty(); });

                    if (shouldStop_ && taskQueue_.empty())
                    {
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
void ResourceManager::StopWorker()
{
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        shouldStop_ = true;
    }

    queueCV_.notify_all();

    if (worker_.joinable())
    {
        worker_.join();
    }
}

// 转换表面为纹理（必须在主线程）
void ResourceManager::ConvertToTexture(short id, SDL_Surface *surface)
{
    TexturePtr texture = std::move(ConvertSurfaceToTexture(renderer, surface));

    if (!texture)
    {
        Console_Log("Failed to convert surface!");
        return;
    }
    textureCache_[id] = std::move(texture);
    Console_Log("ResourceManager: texture id %d converted and stored", id);
}

std::future<void> ResourceManager::LoadResourcesFromJson(short id)
{
    // 1. 构建文件名
    std::string filename =
        "assets/script/STAGE_" + std::to_string(id) + ".json";

    // 2. 检查文件是否存在
    FILE *file = fopen(filename.c_str(), "rb");
    if (!file)
    {
        Console_Log("Error: JSON file %s does not exist", filename.c_str());
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
    if (doc.HasParseError())
    {
        Console_Log("JSON parse error (%s): %s", filename.c_str(),
                    rapidjson::GetParseError_En(doc.GetParseError()));
        std::promise<void> p;
        p.set_value();
        return p.get_future();
    }

    // 5. 验证JSON根对象
    if (!doc.IsObject())
    {
        Console_Log("Error: JSON root is not an object in %s",
                    filename.c_str());
        std::promise<void> p;
        p.set_value();
        return p.get_future();
    }

    // 6. 存储异步任务future
    vector<std::future<void>> futures;

    // 7. 遍历所有资源对象
    for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it)
    {
        short resourceId;
        try
        {
            resourceId = static_cast<short>(std::stoi(it->name.GetString()));
        }
        catch (...)
        {
            Console_Log("Invalid resource ID format: %s", it->name.GetString());
            continue;
        }

        const rapidjson::Value &resObj = it->value;
        if (!resObj.IsObject() || !resObj.HasMember("path") ||
            !resObj["path"].IsString() || !resObj.HasMember("category") ||
            !resObj["category"].IsString())
        {
            Console_Log("Invalid resource object for ID: %d", resourceId);
            continue;
        }

        std::string path = resObj["path"].GetString();
        std::string category = resObj["category"].GetString();

        if (category == "music")
        {
            futures.push_back(LoadMusicAsync(resourceId, path));
        }
        else if (category == "sound")
        {
            futures.push_back(LoadSoundAsync(resourceId, path));
        }
        else if (category == "texture")
        {
            futures.push_back(LoadTextureAsync(resourceId, path));
        }
        else if (category == "font")
        {
            if (!resObj.HasMember("size") || !resObj["size"].IsInt())
            {
                Console_Log("Font resource missing or invalid font size for "
                            "resource ID: %d",
                            resourceId);
                continue;
            }
            int size = resObj["size"].GetInt();
            futures.push_back(LoadFontAsync(resourceId, path, size));
        }
        else
        {
            Console_Log("Unknown category '%s' for resource ID: %d",
                        category.c_str(), resourceId);
        }
    }

    // 8. 返回一个 future 表示所有任务完成
    return std::async(std::launch::async,
                      [futures = std::move(futures), filename]() mutable
                      {
                          for (auto &future : futures)
                          {
                              future.wait();
                          }

                          Console_Log(
                              "ResourceManager: Resources loaded from JSON "
                              "file %s was finished",
                              filename.c_str());
                      });
}

// 释放音乐
void ResourceManager::FreeMusic(short id)
{
    std::lock_guard<std::mutex> lock(musicMutex_);
    if (musicCache_.count(id))
    {
        musicCache_.erase(id);
    }
}

// 释放音效
void ResourceManager::FreeSound(short id)
{
    std::lock_guard<std::mutex> lock(soundMutex_);
    if (soundCache_.count(id))
    {
        soundCache_.erase(id);
    }
}

// 释放纹理
void ResourceManager::FreeTexture(short id)
{
    std::lock_guard<std::mutex> lock(textureMutex_);
    if (textureCache_.count(id))
    {
        textureCache_.erase(id);
    }
}
// 释放字体
void ResourceManager::FreeFont(short id)
{
    std::lock_guard<std::mutex> lock(fontMutex_);
    if (fontCache_.count(id))
    {
        fontCache_.erase(id);
    }
}

// 异步释放资源
std::future<void> ResourceManager::FreeMusicAsync(short id)
{
    return EnqueueTask(
        [this, id]
        {
            TaskGuard guard(activeTasks_);
            FreeMusic(id);
        });
}

std::future<void> ResourceManager::FreeTextureAsync(short id)
{
    return EnqueueTask(
        [this, id]
        {
            TaskGuard guard(activeTasks_);
            FreeTexture(id);
        });
}

std::future<void> ResourceManager::FreeFontAsync(short id)
{
    return EnqueueTask(
        [this, id]
        {
            TaskGuard guard(activeTasks_);
            FreeFont(id);
        });
}

std::future<void> ResourceManager::FreeSoundAsync(short id)
{
    return EnqueueTask(
        [this, id]
        {
            TaskGuard guard(activeTasks_);
            FreeSound(id);
        });
}
