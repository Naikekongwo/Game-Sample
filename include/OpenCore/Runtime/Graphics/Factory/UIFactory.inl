// UIFactory.inl
// UI的工厂方法
#pragma once
#include "OpenCore/OpenCore.hpp"

// 通用版本
template <typename T>
inline unique_ptr<T> UI(const std::string &id, uint8_t layer, short texID,
                        short frameX, short frameY)
{
    return std::make_unique<T>(
        id, layer,
        std::move(std::make_unique<Texture>(
            frameX, frameY, OpenCoreManagers::ResManager.GetTexture(texID))));
}

template <>
inline unique_ptr<FrameCounter> UI<FrameCounter>(const std::string &id,
                                                 uint8_t layer, short texID,
                                                 short reserve0, short reserve1)
{
    return std::make_unique<FrameCounter>(id, layer, nullptr);
}

template <>
inline unique_ptr<BaseBackground>
UI<BaseBackground>(const std::string &id, uint8_t layer, short texID,
                   short reserve0, short reserve1)
{
    return std::make_unique<BaseBackground>(
        id, layer,
        std::move(std::make_unique<Texture>(
            3, 3, OpenCoreManagers::ResManager.GetTexture(texID))));
}

template <>
inline unique_ptr<Scrollbar> UI<Scrollbar>(const std::string &id, uint8_t layer,
                                           short backTexID, short buttTexID,
                                           short reserve0)
{
    return std::make_unique<Scrollbar>(id, layer, backTexID, buttTexID);
}

template <>
inline unique_ptr<CheckBox> UI<CheckBox>(const std::string &id, uint8_t layer,
                                         short TexID, short reserve1,
                                         short reserve0)
{
    return std::make_unique<CheckBox>(
        id, layer,
        std::move(std::make_unique<Texture>(
            1, 2, OpenCoreManagers::ResManager.GetTexture(TexID))));
}

// 特化版本：MultiImageBoard
template <>
inline unique_ptr<MultiImageBoard>
UI<MultiImageBoard>(const std::string &id, uint8_t layer, short texID,
                    short reserve0, short reserve1)
{
    return std::make_unique<MultiImageBoard>(id, layer, texID);
}