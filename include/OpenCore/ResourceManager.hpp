#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "Graphics/Renderer/IRenderer.hpp"
#include "OpenCore/Graphics/Renderer/IRenderer.hpp"

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



class ResourceManager
{
  public:
    static ResourceManager &getInstance();

    bool Init();
    void CleanUp();

    Mix_Chunk *GetSound(short id);
    Mix_Music *GetMusic(short id);
    TTF_Font *GetFont(short id);

    std::shared_ptr<OpenCore_Tilemap> GetTexture(short id);

    void PushTexture(short id, TexturePtr texPtr);

  protected:

    std::future<void> LoadMusicAsync(short id, const std::string &path);
    std::future<void> LoadTextureAsync(short id, const std::string &path);
    std::future<void> LoadFontAsync(short id, const std::string &path,
                                    int size);
    std::future<void> LoadSoundAsync(short id, const std::string &path);

    // 通过json进行整个场景的资源加载
    std::future<void> LoadManifest(short id);

    // 异步释放资源
    std::future<void> FreeMusicAsync(short id);
    std::future<void> FreeTextureAsync(short id);
    std::future<void> FreeFontAsync(short id);
    std::future<void> FreeSoundAsync(short id);

  private:

  

};

#endif //_RESOURCE_MANAGER_H_