#include "Eclipsea/Core/EclipseaTextures.hpp"
#include "OpenCore.hpp"

#include <cstdint>

std::unique_ptr<Texture> MakeTexture(uint8_t xCount, uint8_t yCount,
                                     std::string_view textureName)
{
    auto *package = OpenEngine::getInstance().getPackageManager();
    auto obj =
        package ? package->getTextureObject({textureName, xCount, yCount})
                : nullptr;
    if (!obj)
    {
        LOG("MakeTexture: 纹理资源 {} 加载失败", textureName);
        return nullptr;
    }
    return std::make_unique<Texture>(*obj);
}
