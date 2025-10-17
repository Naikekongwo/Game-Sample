// UIFactory.inl
// UI的工厂方法
#include "OpenCore/OpenCore.hpp"

template<typename T>
inline std::unique_ptr<T> UI(std::string id, short layer, short texID, short frameX, short frameY)
{
    return std::make_unique<T>(id, layer, new Texture(frameX, frameY, OpenCoreManagers::ResManager.GetTexture(texID)));
}