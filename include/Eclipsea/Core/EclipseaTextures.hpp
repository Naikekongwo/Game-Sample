#pragma once

// EclipseaTextures.hpp
// 旧引擎「数字纹理ID」→ 新引擎「资源名」的桥接层
//
// 新版 OpenCore 使用 PackageManager 以资源名注册/获取纹理，
// 而旧 Game-Project 的玩法代码全部使用数字 ID。此模块在启动时
// 建立 ID → 名称 的映射，向玩法层提供与旧 ResManager 兼容的接口。

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <SDL3/SDL.h>

struct Texture;

// 旧引擎 MakeTexture 的等价物：按数字纹理 ID 创建带网格分割的纹理包装对象
std::unique_ptr<Texture> MakeTexture(uint8_t xCount, uint8_t yCount,
                                     short texId);

class EclipseaTextures final
{
  public:
    static EclipseaTextures &getInstance();

    // 数字 ID → 资源名绑定（在 StartUp 注册资源时调用）
    void bindTexture(short textureID, std::string_view name);

    // 查询资源名
    std::string nameOf(short textureID) const;

    // 兼容旧接口：按数字 ID 获取 SDL_Texture（缓存由 PackageManager 管理）
    std::shared_ptr<SDL_Texture> getTexture(short textureID);

    // 便捷：按数字 ID 获取引擎 Texture 包装对象（含网格分割）
    std::shared_ptr<Texture> getTextureObject(short textureID, uint8_t cols,
                                              uint8_t rows);

  private:
    EclipseaTextures() = default;
    ~EclipseaTextures() = default;

    std::unordered_map<short, std::string> bindings_;
};
