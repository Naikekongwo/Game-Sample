#include "Eclipsea/Gameplay/TextureMeta.hpp"
#include "OpenCore.hpp"
#include <memory>
#include <optional>

EclipseaTextureMetaManager &EclipseaTextureMetaManager::getInstance() {
  static EclipseaTextureMetaManager instance;
  return instance;
}

EclipseaTextureMetaManager::EclipseaTextureMetaManager() {
  // 初始化的相关操作
  _metaRegistry.clear();
  _textureCache.clear();
}

bool EclipseaTextureMetaManager::registerTextureMeta(EclipseaTextureMeta meta) {
  if (meta.textureName.empty())
    return false;
  _metaRegistry[meta.textureName] = meta;
  return true;
}

std::optional<shared_ptr<Texture>>
EclipseaTextureMetaManager::getTexture(std::string_view textureName) {
  auto key = std::string(textureName);
  auto it = _textureCache.find(key);
  if (it != _textureCache.end())
    return it->second;

  auto metaIt = _metaRegistry.find(key);
  if (metaIt == _metaRegistry.end())
    return std::nullopt;

  const EclipseaTextureMeta &meta = metaIt->second;
  auto *package = OpenEngine::getInstance().getPackageManager();
  auto texture =
      package
          ? package->getTextureObject({meta.textureName, meta.cols, meta.rows})
          : nullptr;
  if (!texture)
    return std::nullopt;

  _textureCache[key] = texture;
  return texture;
}