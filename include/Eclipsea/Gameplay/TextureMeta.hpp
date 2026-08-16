#pragma once

// 注意：新版 OpenCore 的 PackageManager 也定义了名为 TextureMeta 的类型
// （基于资源名）。为避免冲突，本项目将旧玩法系统的版本重命名为
// EclipseaTextureMeta，与 PackageManager 一致，改用「资源名 + 网格」描述，
// 不再使用旧引擎的数字纹理 ID。

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

using std::optional;
using std::shared_ptr;
using std::string;
using std::string_view;
using std::unordered_map;

struct EclipseaTextureMeta {
  std::string textureName;
  uint8_t cols;
  uint8_t rows;

  EclipseaTextureMeta(std::string_view name, uint8_t cols, uint8_t rows)
      : textureName(name), cols(cols), rows(rows) {}

  EclipseaTextureMeta(std::string_view name)
      : textureName(name), cols(1), rows(1) {}
  EclipseaTextureMeta() : cols(1), rows(1) {}

  bool operator==(const EclipseaTextureMeta &other) const {
    return textureName == other.textureName && cols == other.cols &&
           rows == other.rows;
  }
};

struct Texture;

class EclipseaTextureMetaManager final {
public:
  static EclipseaTextureMetaManager &getInstance();

  // 初始化
  EclipseaTextureMetaManager();

  bool registerTextureMeta(EclipseaTextureMeta meta);

  optional<EclipseaTextureMeta>
  queryTextureMeta(string_view textureName) const {
    auto it = _metaRegistry.find(std::string(textureName));
    if (it != _metaRegistry.end())
      return it->second;
    return std::nullopt;
  }

  // 支持缓存机制的纹理获取函数（直接走 PackageManager）
  optional<shared_ptr<Texture>> getTexture(string_view textureName);

private:
  unordered_map<string, EclipseaTextureMeta> _metaRegistry;
  unordered_map<string, shared_ptr<Texture>> _textureCache;
};