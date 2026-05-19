#pragma once

#include <cstdint>
#include <optional>
#include <unordered_map>

using std::optional;
using std::shared_ptr;
using std::unordered_map;

struct TextureMeta
{
    short textureID;
    uint8_t cols;
    uint8_t rows;

    TextureMeta(short textureID, uint8_t cols, uint8_t rows)
        : textureID(textureID), cols(cols), rows(rows)
    {
    }

    TextureMeta(short textureID) : textureID(textureID), cols(1), rows(1) {}
    TextureMeta() : textureID(-1), cols(1), rows(1) {}

    bool operator==(const TextureMeta &other) const
    {
        return textureID == other.textureID && cols == other.cols &&
               rows == other.rows;
    }
};

struct Texture;

class TextureMetaManager final
{
  public:
    static TextureMetaManager &getInstance();

    // 初始化
    TextureMetaManager();

    bool registerTextureMeta(TextureMeta meta);

    // 支持缓存机制的纹理获取函数
    optional<shared_ptr<Texture>> getTexture(short textureID);

  private:
    unordered_map<short, TextureMeta> _metaRegistry;
    unordered_map<short, shared_ptr<Texture>> _textureCache;
};