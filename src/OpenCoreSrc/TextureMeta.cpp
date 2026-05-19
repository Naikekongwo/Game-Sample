#include "OpenCore/Runtime/Graphics/Manager/TextureMeta.hpp"
#include "OpenCore/OpenCore.hpp"
#include <memory>
#include <optional>

TextureMetaManager &TextureMetaManager::getInstance()
{
    static TextureMetaManager instance;
    return instance;
}

TextureMetaManager::TextureMetaManager()
{
    // 初始化的相关操作
    _metaRegistry.clear();
    _textureCache.clear();
}

bool TextureMetaManager::registerTextureMeta(TextureMeta meta)
{
    if (meta.textureID < 0)
        return false;
    _metaRegistry[meta.textureID] = meta;
    return true;
}

std::optional<shared_ptr<Texture>>
TextureMetaManager::getTexture(short textureID)
{
    auto it = _textureCache.find(textureID);
    if (it != _textureCache.end())
        return it->second;

    auto metaIt = _metaRegistry.find(textureID);
    if (metaIt == _metaRegistry.end())
        return std::nullopt;

    const TextureMeta &meta = metaIt->second;
    auto texture = make_shared<Texture>(
        meta.cols, meta.rows,
        OpenCoreManagers::ResManager.GetTexture(meta.textureID));
    if (!texture)
        return std::nullopt;

    return texture;
}