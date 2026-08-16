#pragma once

// 注意：新版 OpenCore 的 PackageManager 也定义了名为 TextureMeta 的类型
// （基于资源名）。为避免冲突，本项目将旧玩法系统的数字ID版重命名为
// EclipseaTextureMeta，其纹理获取统一走 EclipseaTextures 桥接。

#include <cstdint>
#include <optional>
#include <unordered_map>

using std::optional;
using std::shared_ptr;
using std::unordered_map;

struct EclipseaTextureMeta
{
    short textureID;
    uint8_t cols;
    uint8_t rows;

    EclipseaTextureMeta(short textureID, uint8_t cols, uint8_t rows)
        : textureID(textureID), cols(cols), rows(rows)
    {
    }

    EclipseaTextureMeta(short textureID)
        : textureID(textureID), cols(1), rows(1)
    {
    }
    EclipseaTextureMeta() : textureID(-1), cols(1), rows(1) {}

    bool operator==(const EclipseaTextureMeta &other) const
    {
        return textureID == other.textureID && cols == other.cols &&
               rows == other.rows;
    }
};

struct Texture;

class EclipseaTextureMetaManager final
{
  public:
    static EclipseaTextureMetaManager &getInstance();

    // 初始化
    EclipseaTextureMetaManager();

    bool registerTextureMeta(EclipseaTextureMeta meta);

    optional<EclipseaTextureMeta> queryTextureMeta(short textureID) const
    {
        auto it = _metaRegistry.find(textureID);
        if (it != _metaRegistry.end())
            return it->second;
        return std::nullopt;
    }

    // 支持缓存机制的纹理获取函数（委托给 EclipseaTextures 桥接）
    optional<shared_ptr<Texture>> getTexture(short textureID);

  private:
    unordered_map<short, EclipseaTextureMeta> _metaRegistry;
    unordered_map<short, shared_ptr<Texture>> _textureCache;
};