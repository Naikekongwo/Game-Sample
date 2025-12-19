#ifndef _UI_FACTORY_INL_
#define _UI_FACTORY_INL_

// UIFactory.inl
// UI的工厂方法
#include "OpenCore/OpenCore.hpp"

#include "Eclipsea/UI/MultiImageBoard.hpp"

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


// 特化版本：MultiImageBoard
template<>
inline std::unique_ptr<MultiImageBoard> UI<MultiImageBoard>(const std::string &id, uint8_t layer, short texID, short frameX, short frameY)
{
    return std::make_unique<MultiImageBoard>(id, layer, texID);
}

#endif //_UI_FACTORY_INL_