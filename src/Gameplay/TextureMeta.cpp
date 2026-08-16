#include "Eclipsea/Gameplay/TextureMeta.hpp"
#include "Eclipsea/Core/EclipseaTextures.hpp"
#include "OpenCore.hpp"
#include <memory>
#include <optional>

EclipseaTextureMetaManager &EclipseaTextureMetaManager::getInstance()
{
    static EclipseaTextureMetaManager instance;
    return instance;
}

EclipseaTextureMetaManager::EclipseaTextureMetaManager()
{
    // 初始化的相关操作
    _metaRegistry.clear();
    _textureCache.clear();
}

bool EclipseaTextureMetaManager::registerTextureMeta(EclipseaTextureMeta meta)
{
    if (meta.textureID < 0)
        return false;
    _metaRegistry[meta.textureID] = meta;
    return true;
}

std::optional<shared_ptr<Texture>>
EclipseaTextureMetaManager::getTexture(short textureID)
{
    auto it = _textureCache.find(textureID);
    if (it != _textureCache.end())
        return it->second;

    auto metaIt = _metaRegistry.find(textureID);
    if (metaIt == _metaRegistry.end())
        return std::nullopt;

    const EclipseaTextureMeta &meta = metaIt->second;
    auto texture = EclipseaTextures::getInstance().getTextureObject(
        meta.textureID, meta.cols, meta.rows);
    if (!texture)
        return std::nullopt;

    return texture;
}