#include "Eclipsea/Core/EclipseaTextures.hpp"
#include "OpenCore.hpp"

#include <cstdint>

std::unique_ptr<Texture> MakeTexture(uint8_t xCount, uint8_t yCount,
                                     short texId)
{
    auto obj = EclipseaTextures::getInstance().getTextureObject(texId, xCount,
                                                                yCount);
    if (!obj)
    {
        LOG("MakeTexture: 纹理ID {} 加载失败", texId);
        return nullptr;
    }
    return std::make_unique<Texture>(*obj);
}

EclipseaTextures &EclipseaTextures::getInstance()
{
    static EclipseaTextures instance;
    return instance;
}

void EclipseaTextures::bindTexture(short textureID, std::string_view name)
{
    bindings_[textureID] = std::string(name);
}

std::string EclipseaTextures::nameOf(short textureID) const
{
    auto it = bindings_.find(textureID);
    return it == bindings_.end() ? std::string() : it->second;
}

std::shared_ptr<SDL_Texture> EclipseaTextures::getTexture(short textureID)
{
    auto it = bindings_.find(textureID);
    if (it == bindings_.end())
    {
        LOG("EclipseaTextures::getTexture: 未绑定的纹理ID {}", textureID);
        return nullptr;
    }
    auto *package = OpenEngine::getInstance().getPackageManager();
    return package ? package->getTexture(it->second) : nullptr;
}

std::shared_ptr<Texture>
EclipseaTextures::getTextureObject(short textureID, uint8_t cols,
                                   uint8_t rows)
{
    auto it = bindings_.find(textureID);
    if (it == bindings_.end())
    {
        LOG("EclipseaTextures::getTextureObject: 未绑定的纹理ID {}", textureID);
        return nullptr;
    }
    auto *package = OpenEngine::getInstance().getPackageManager();
    return package ? package->getTextureObject({it->second, cols, rows})
                   : nullptr;
}
