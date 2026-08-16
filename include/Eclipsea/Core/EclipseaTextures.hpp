#pragma once

// EclipseaTextures.hpp
// 项目级纹理辅助（新版 OpenCore 已放弃数字 ID，统一以资源名访问）
//
// 新版 OpenCore 使用 PackageManager 以资源名注册/获取纹理，旧引擎的
// 「数字纹理 ID → 资源名」桥接层（EclipseaTextures 类）已废弃移除，
// 玩法层直接使用资源名字符串。

#include <memory>
#include <string_view>

struct Texture;

// 按资源名创建带网格分割的纹理包装对象（等价于旧引擎 MakeTexture）
std::unique_ptr<Texture> MakeTexture(uint8_t xCount, uint8_t yCount,
                                     std::string_view textureName);
