// TextureFactory.inl
// 纹理的工厂方法

#include "OpenCore/OpenCore.hpp"

inline std::unique_ptr<Texture> MakeTexture(uint8_t xCount, uint8_t yCount, short texId)
{
    return std::make_unique<Texture>(xCount, yCount, OpenCoreManagers::ResManager.GetTexture(texId));
}