// UIFactory.inl
// UI的工厂方法
#include "OpenCore/OpenCore.hpp"

// 通用版本
template <typename T>
inline std::unique_ptr<T> UI(const std::string &id, uint8_t layer, short texID, short frameX, short frameY)
{
    return std::make_unique<T>(id, layer, std::move(std::make_unique<Texture>(frameX, frameY, OpenCoreManagers::ResManager.GetTexture(texID))));
}

// 特化版本：FrameCounter <id,layer,X,X,X>
template <>
inline std::unique_ptr<FrameCounter> UI<FrameCounter>(const std::string &id, uint8_t layer, short texID, short frameX, short frameY)
{
    return std::make_unique<FrameCounter>(id, layer, nullptr);
}

template <>
inline std::unique_ptr<Label> UI<Label>(const std::string &id, uint8_t layer, short texID, short frameX, short frameY)
{
    return std::make_unique<Label>(id, layer, texID);
}

// 特化版本：MultiImageBoard
template<>
inline std::unique_ptr<MultiImageBoard> UI<MultiImageBoard>(const std::string &id, uint8_t layer, short texID, short frameX, short frameY)
{
    return std::make_unique<MultiImageBoard>(id, layer, texID);
}