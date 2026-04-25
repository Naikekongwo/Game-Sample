#include "OpenCore/OpenCore.hpp"

#include <SDL2/SDL_image.h>

bool ResourceManager::Init()
{
    ///< 确保GraphicsManager 一定在其之前进行初始化
    renderer = OpenCoreManagers::GFXManager.getRenderer();

    if (!renderer)
    {
        LOG("ResourceManager::Init() encountered a null renderer.");
        return false;
    }

    int result = Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
    if (!result)
    {
        LOG("ResourceManager::ResourceManager() failed to initialize "
            "SDL_Mixer: {}",
            Mix_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) <
        0)
    {
        LOG("ResourceManager::ResourceManager() failed to open audio: {}",
            Mix_GetError());
        return false;
    }

    LOG("ResourceManager::ResourceManager() SDL_Mixer initialized "
        "successfully.");

    return true;
}

void ResourceManager::CleanUp()
{
    ClearAll();
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();
    renderer = nullptr;
}

ResourceManager &ResourceManager::getInstance()
{
    static ResourceManager instance;
    return instance;
}

void ResourceManager::LoadMusic(short id, const std::string &path)
{
    std::lock_guard<std::mutex> lock(musicMutex_);
    if (musicCache_.count(id))
        return;

    MusicPtr music = LoadMusicFromFile(path);
    if (!music)
    {
        LOG("Mix_LoadMUS failed: {}", Mix_GetError());
        return;
    }

    LOG("ResourceManager::LoadMusic music id {} loaded successfully.", id);
    musicCache_[id] = std::move(music);
}

Mix_Music *ResourceManager::GetMusic(short id)
{
    std::lock_guard<std::mutex> lock(musicMutex_);
    auto it = musicCache_.find(id);

    if (it == musicCache_.end())
    {
        LOG("ResourceManager::GetMusic failed to get music id {}", id);
        return nullptr;
    }
    return it->second.get();
}

void ResourceManager::LoadSound(short id, const std::string &path)
{
    std::lock_guard<std::mutex> lock(soundMutex_);
    if (soundCache_.count(id))
        return;

    SoundPtr sound = std::move(LoadSoundFromFile(path));
    if (!sound)
    {
        LOG("Mix_LoadWAV failed: {}", Mix_GetError());
        return;
    }

    LOG("ResourceManager::LoadSound sound id {} loaded successfully.", id);

    soundCache_[id] = std::move(sound);
}

Mix_Chunk *ResourceManager::GetSound(short id)
{
    std::lock_guard<std::mutex> lock(soundMutex_);
    auto it = soundCache_.find(id);

    if (it == soundCache_.end())
    {
        LOG("ResourceManager::GetSound failed to get sound id {}", id);
        return nullptr;
    }
    return it->second.get();
}

void ResourceManager::LoadTexture(short id, const std::string &path)
{

    {
        std::lock_guard<std::mutex> lock(textureMutex_);
        if (textureCache_.count(id))
            return;
    }

    SDL_Surface *surface = LoadSurface(path);
    if (!surface)
    {
        LOG("ResourceManager::LoadTexture failed to load surface for id: {}",
            id);
        return;
    }

    ConvertToTexture(id, surface);
}

shared_ptr<SDL_Texture> ResourceManager::GetTexture(short id)
{
    std::lock_guard<std::mutex> lock(textureMutex_);
    auto it = textureCache_.find(id);

    if (it == textureCache_.end())
    {
        LOG("ResourceManager::GetTexture failed to get texture id {}", id);
        return nullptr;
    }
    return shared_ptr<SDL_Texture>(
        it->second.get(),
        [](SDL_Texture *) { /* do nothing, managed by unique_ptr */ });
}

std::future<void> ResourceManager::LoadMusicAsync(short id,
                                                  const std::string &path)
{
    return EnqueueTask([this, id, path] { LoadMusic(id, path); });
}

std::future<void> ResourceManager::LoadSoundAsync(short id,
                                                  const std::string &path)
{
    return EnqueueTask([this, id, path] { LoadSound(id, path); });
}

void ResourceManager::LoadFont(short id, const std::string &path, int size)
{
    std::lock_guard<std::mutex> lock(fontMutex_);
    if (fontCache_.count(id))
        return;

    FontPtr font(TTF_OpenFont(path.c_str(), size));
    if (!font)
    {
        LOG("TTF_OpenFont failed: {}", TTF_GetError());
        return;
    }

    LOG("ResourceManager::LoadFont font id {} loaded successfully.", id);
    fontCache_[id] = std::move(font);
}

std::future<void>
ResourceManager::LoadFontAsync(short id, const std::string &path, int size)
{
    return EnqueueTask([this, id, path, size] { LoadFont(id, path, size); });
}

TTF_Font *ResourceManager::GetFont(short id)
{
    std::lock_guard<std::mutex> lock(fontMutex_);
    auto it = fontCache_.find(id);

    if (it == fontCache_.end())
    {
        LOG("ResourceManager::GetFont failed to get font id {}", id);
        return nullptr;
    }
    return it->second.get();
}

std::future<void> ResourceManager::LoadTextureAsync(short id,
                                                    const std::string &path)
{
    auto promise = std::make_shared<std::promise<void>>();
    std::future<void> future = promise->get_future();

    EnqueueTask(
        [this, id, path, promise]
        {
            SDL_Surface *surface = nullptr;
            try
            {
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

            ///< 将纹理创建任务提交到主线程队列（现通过 ThreadManager）
            ThreadManager::getInstance().submit_to_main_thread(
                [this, id, surface, promise]
                {
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

void ResourceManager::ClearAll()
{
    LOG("ResourceManager::ClearAll() started");

    ThreadManager::getInstance().wait_for_all_tasks();

    ThreadManager::getInstance().process_main_thread_tasks();

    LOG("ResourceManager::ClearAll() stopped task queue successfully");

    {
        std::lock_guard<std::mutex> lock(musicMutex_);
        LOG("ResourceManager::ClearAll() clearing music cache, count={}",
            static_cast<int>(musicCache_.size()));
        musicCache_.clear();
        LOG("ResourceManager::ClearAll() cleared music cache");
    }

    {
        std::lock_guard<std::mutex> lock(soundMutex_);
        soundCache_.clear();
    }

    {
        std::lock_guard<std::mutex> lock(textureMutex_);
        LOG("ResourceManager::ClearAll() clearing texture cache, count={}",
            static_cast<int>(textureCache_.size()));
        textureCache_.clear();
        LOG("ResourceManager::ClearAll() cleared texture cache");
    }

    {
        std::lock_guard<std::mutex> lock(fontMutex_);
        fontCache_.clear();
    }

    LOG("ResourceManager::ClearAll() finished");
}

void ResourceManager::ProcessMainThreadTasks()
{
    ThreadManager::getInstance().process_main_thread_tasks();
}

void ResourceManager::ConvertToTexture(short id, SDL_Surface *surface)
{
    TexturePtr texture = std::move(ConvertSurfaceToTexture(renderer, surface));

    if (!texture)
    {
        LOG("Failed to convert surface!");
        return;
    }
    textureCache_[id] = std::move(texture);
    LOG("ResourceManager: texture id {} converted and stored", id);
}

std::future<void> ResourceManager::LoadResourcesFromJson(short id)
{
    std::string filename =
        "assets/script/STAGE_" + std::to_string(id) + ".json";

    FILE *file = fopen(filename.c_str(), "rb");
    if (!file)
    {
        LOG("Error: JSON file {} does not exist", filename.c_str());
        std::promise<void> p;
        p.set_value();
        return p.get_future();
    }

    char readBuffer[65536];
    rapidjson::FileReadStream stream(file, readBuffer, sizeof(readBuffer));
    rapidjson::Document doc;
    doc.ParseStream(stream);
    fclose(file);

    if (doc.HasParseError())
    {
        LOG("JSON parse error ({}): {}", filename.c_str(),
            rapidjson::GetParseError_En(doc.GetParseError()));
        std::promise<void> p;
        p.set_value();
        return p.get_future();
    }

    if (!doc.IsObject())
    {
        LOG("Error: JSON root is not an object in {}", filename.c_str());
        std::promise<void> p;
        p.set_value();
        return p.get_future();
    }

    vector<std::future<void>> futures;

    for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it)
    {
        short resourceId;
        try
        {
            resourceId = static_cast<short>(std::stoi(it->name.GetString()));
        }
        catch (...)
        {
            LOG("Invalid resource ID format: {}", it->name.GetString());
            continue;
        }

        const rapidjson::Value &resObj = it->value;
        if (!resObj.IsObject() || !resObj.HasMember("path") ||
            !resObj["path"].IsString() || !resObj.HasMember("category") ||
            !resObj["category"].IsString())
        {
            LOG("Invalid resource object for ID: {}", resourceId);
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
                LOG("Font resource missing or invalid font size for "
                    "resource ID: {}",
                    resourceId);
                continue;
            }
            int size = resObj["size"].GetInt();
            futures.push_back(LoadFontAsync(resourceId, path, size));
        }
        else
        {
            LOG("Unknown category '{}' for resource ID: {}", category.c_str(),
                resourceId);
        }
    }

    return std::async(std::launch::async,
                      [futures = std::move(futures), filename]() mutable
                      {
                          for (auto &future : futures)
                          {
                              future.wait();
                          }
                          LOG("ResourceManager: Resources loaded from JSON "
                              "file {} was finished",
                              filename.c_str());
                      });
}

void ResourceManager::FreeMusic(short id)
{
    std::lock_guard<std::mutex> lock(musicMutex_);
    if (musicCache_.count(id))
    {
        musicCache_.erase(id);
    }
}

void ResourceManager::FreeSound(short id)
{
    std::lock_guard<std::mutex> lock(soundMutex_);
    if (soundCache_.count(id))
    {
        soundCache_.erase(id);
    }
}

void ResourceManager::FreeTexture(short id)
{
    std::lock_guard<std::mutex> lock(textureMutex_);
    if (textureCache_.count(id))
    {
        textureCache_.erase(id);
    }
}
void ResourceManager::FreeFont(short id)
{
    std::lock_guard<std::mutex> lock(fontMutex_);
    if (fontCache_.count(id))
    {
        fontCache_.erase(id);
    }
}

std::future<void> ResourceManager::FreeMusicAsync(short id)
{
    return EnqueueTask([this, id] { FreeMusic(id); });
}

std::future<void> ResourceManager::FreeTextureAsync(short id)
{
    return EnqueueTask([this, id] { FreeTexture(id); });
}

std::future<void> ResourceManager::FreeFontAsync(short id)
{
    return EnqueueTask([this, id] { FreeFont(id); });
}

std::future<void> ResourceManager::FreeSoundAsync(short id)
{
    return EnqueueTask([this, id] { FreeSound(id); });
}