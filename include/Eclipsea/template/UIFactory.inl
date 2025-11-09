// UIFactory.inl
// UI的工厂方法
#include "OpenCore/OpenCore.hpp"

// 通用版本
template <typename T>
inline std::unique_ptr<T> UI(const std::string &id, uint8_t layer, short texID, short frameX, short frameY)
{
    return std::make_unique<T>(id, layer, std::move(std::make_unique<Texture>(frameX, frameY, OpenCoreManagers::ResManager.GetTexture(texID))));
}

// 特化版本：FrameCounter
template <>
inline std::unique_ptr<FrameCounter> UI<FrameCounter>(const std::string &id, uint8_t layer, short texID, short frameX, short frameY)
{
    return std::make_unique<FrameCounter>(id, layer, nullptr);
}