// UIFactory.inl
// UI的工厂方法
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

// 特化版本：FrameCounter <id,layer,X,X,X>
template <>
inline unique_ptr<FrameCounter> UI<FrameCounter>(const std::string &id,
                                                 uint8_t layer, short texID,
                                                 short frameX, short frameY)
{
    return std::make_unique<FrameCounter>(id, layer, nullptr);
}

template <>
inline unique_ptr<Scrollbar> UI<Scrollbar>(const std::string &id, uint8_t layer,
                                           short backTexID, short buttTexID,
                                           short frameY)
{
    return std::make_unique<Scrollbar>(id, layer, backTexID, buttTexID);
}

// 特化版本：MultiImageBoard
template <>
inline unique_ptr<MultiImageBoard>
UI<MultiImageBoard>(const std::string &id, uint8_t layer, short texID,
                    short frameX, short frameY)
{
    return std::make_unique<MultiImageBoard>(id, layer, texID);
}