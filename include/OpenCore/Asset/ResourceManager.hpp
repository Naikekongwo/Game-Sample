/**
 * @file ResourceManager.hpp
 * @brief 资源管理器模块，负责游戏资源的加载、缓存、异步管理和释放。
 * 
 * 该模块提供了音乐、音效、纹理、字体的同步/异步加载与释放接口，
 * 通过单例模式全局访问，并利用 ThreadManager 执行异步任务。
 */
#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

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

#include "OpenCore/Asset/MapLoader.hpp"
#include "OpenCore/Asset/SoundLoader.hpp"
#include "OpenCore/Asset/TextureLoader.hpp"
#include "OpenCore/Core/Thread/ThreadManager.hpp" // 新增

using std::shared_ptr;
using std::unique_ptr;

// 任务保护（已不再需要，可以移除，但为保持兼容可保留注释）
// struct TaskGuard { ... };

/**
 * @class ResourceManager
 * @brief 资源管理单例类，提供资源加载、缓存、异步操作和释放功能。
 * 
 * 该类管理四种资源类型：音乐(Mix_Music*)、音效(Mix_Chunk*)、纹理(SDL_Texture*)、字体(TTF_Font*)。
 * 所有资源通过 short 类型的唯一标识符进行存取。支持同步和异步加载/释放，
 * 异步任务通过 ThreadManager 提交到工作线程，并返回 std::future 以便同步。
 * 资源内部使用智能指针封装，自动管理生命周期，但提供了显式的释放接口。
 */

class ResourceManager
{
  public:
    /**
     * @brief 获取 ResourceManager 单例实例。
     * @return ResourceManager 的引用。
     */
    static ResourceManager &getInstance();
    /**
     * @brief 初始化资源管理器，设置内部状态（例如获取渲染器指针）。
     * @return 初始化成功返回 true，否则返回 false。
     */
    bool Init();
    /**
     * @brief 清理资源管理器，释放所有已加载的资源。
     */
    void CleanUp();
    /**
     * @brief 同步加载音乐资源。
     * @param id 资源的唯一标识符。
     * @param path 音乐文件的路径。
     */
    void LoadMusic(short id, const std::string &path);
    /**
     * @brief 获取已加载的音乐资源。
     * @param id 资源的标识符。
     * @return Mix_Music* 指针，若未加载则返回 nullptr。
     */
    Mix_Music *GetMusic(short id);
    /**
     * @brief 同步加载音效资源。
     * @param id 资源的唯一标识符。
     * @param path 音效文件的路径。
     */
    void LoadSound(short id, const std::string &path);
    /**
     * @brief 获取已加载的音效资源。
     * @param id 资源的标识符。
     * @return Mix_Chunk* 指针，若未加载则返回 nullptr。
     */
    Mix_Chunk *GetSound(short id);
    /**
     * @brief 同步加载纹理资源。
     * @param id 资源的唯一标识符。
     * @param path 纹理文件的路径。
     */
    void LoadTexture(short id, const std::string &path);
    /**
     * @brief 获取已加载的纹理资源。
     * @param id 资源的标识符。
     * @return shared_ptr<SDL_Texture> 智能指针，若未加载则为空。
     */
    shared_ptr<SDL_Texture> GetTexture(short id);
    /**
     * @brief 同步加载字体资源。
     * @param id 资源的唯一标识符。
     * @param path 字体文件的路径。
     * @param size 字体大小（磅值）。
     */
    void LoadFont(short id, const std::string &path, int size);
    /**
     * @brief 获取已加载的字体资源。
     * @param id 资源的标识符。
     * @return TTF_Font* 指针，若未加载则返回 nullptr。
     */
    TTF_Font *GetFont(short id);
    
     /**
     * @brief 异步加载音乐资源。
     * @param id 资源的唯一标识符。
     * @param path 音乐文件的路径。
     * @return std::future<void> 可用于等待加载完成。
     */
    std::future<void> LoadMusicAsync(short id, const std::string &path);
    /**
     * @brief 异步加载纹理资源。
     * @param id 资源的唯一标识符。
     * @param path 纹理文件的路径。
     * @return std::future<void> 可用于等待加载完成。
     */
    std::future<void> LoadTextureAsync(short id, const std::string &path);
    /**
     * @brief 异步加载字体资源。
     * @param id 资源的唯一标识符。
     * @param path 字体文件的路径。
     * @param size 字体大小（磅值）。
     * @return std::future<void> 可用于等待加载完成。
     */
    std::future<void> LoadFontAsync(short id, const std::string &path,
                                    int size);
    /**
     * @brief 异步加载音效资源。
     * @param id 资源的唯一标识符。
     * @param path 音效文件的路径。
     * @return std::future<void> 可用于等待加载完成。
     */
    std::future<void> LoadSoundAsync(short id, const std::string &path);
    /**
     * @brief 清除所有已加载的资源（清空所有缓存）。
     */
    void ClearAll();
    /**
     * @brief 处理主线程任务（现委托给 ThreadManager）。
     */
    void ProcessMainThreadTasks();

    /**
     * @brief 通过 JSON 配置文件加载整个场景的资源。
     * @param id 场景或配置资源的标识符。
     * @return std::future<void> 可用于等待加载完成。
     */
    std::future<void> LoadResourcesFromJson(short id);

    /**
     * @brief 释放指定标识符的音乐资源。
     * @param id 资源的标识符。
     */
    void FreeMusic(short id);
    /**
     * @brief 释放指定标识符的纹理资源。
     * @param id 资源的标识符。
     */
    void FreeTexture(short id);
    /**
     * @brief 释放指定标识符的字体资源。
     * @param id 资源的标识符。
     */
    void FreeFont(short id);
    /**
     * @brief 释放指定标识符的音效资源。
     * @param id 资源的标识符。
     */
    void FreeSound(short id);

    /**
     * @brief 异步释放音乐资源。
     * @param id 资源的标识符。
     * @return std::future<void> 可用于等待释放完成。
     */
    std::future<void> FreeMusicAsync(short id);
    /**
     * @brief 异步释放纹理资源。
     * @param id 资源的标识符。
     * @return std::future<void> 可用于等待释放完成。
     */
    std::future<void> FreeTextureAsync(short id);
    /**
     * @brief 异步释放字体资源。
     * @param id 资源的标识符。
     * @return std::future<void> 可用于等待释放完成。
     */
    std::future<void> FreeFontAsync(short id);
    /**
     * @brief 异步释放音效资源。
     * @param id 资源的标识符。
     * @return std::future<void> 可用于等待释放完成。
     */
    std::future<void> FreeSoundAsync(short id);

  private:
    SDL_Renderer *renderer = nullptr;///< SDL 渲染器指针，用于纹理转换

    /**
     * @brief 将任意可调用对象打包为异步任务并提交到线程池。将任务推入 ThreadManager 的线程池执行。
     * @tparam F 可调用对象类型。
     * @param f 要执行的函数或可调用对象。
     * @return std::future<void> 关联的任务 future。
     */
    template <typename F> std::future<void> EnqueueTask(F &&f);
    /**
     * @brief 将 SDL_Surface 转换为纹理并存入缓存。
     * @param id 纹理标识符。
     * @param surface 源表面，函数内部将负责释放。
     */
    void ConvertToTexture(short id, SDL_Surface *surface);

    // 资源缓存
    std::mutex musicMutex_;///< 音乐缓存互斥锁
    std::unordered_map<short, MusicPtr> musicCache_;///< 音乐缓存

    std::mutex soundMutex_;///< 音效缓存互斥锁
    std::unordered_map<short, SoundPtr> soundCache_;///< 音效缓存

    std::mutex textureMutex_;///< 纹理缓存互斥锁
    std::unordered_map<short, TexturePtr> textureCache_;///< 纹理缓存

    std::mutex fontMutex_;///< 字体缓存互斥锁
    std::unordered_map<short, FontPtr> fontCache_;///< 字体缓存
};

template <typename F> std::future<void> ResourceManager::EnqueueTask(F &&f)
{
    ///< 现在直接使用 ThreadManager 提交任务
    return ThreadManager::getInstance().submit(std::forward<F>(f));
}

#endif //_RESOURCE_MANAGER_H_