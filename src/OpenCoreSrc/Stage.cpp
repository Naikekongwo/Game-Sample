
#include "OpenCore/OpenCore.hpp"
#include <string>

bool Stage::transferElementFrom(Stage *srcStage, const std::string &id)
{
    return srcStage->transferElementTo(this, id);
}

bool Stage::transferElementTo(Stage *destStage, const std::string &id)
{
    auto srcElements = this->getElementManager();
    auto dstElements = destStage->getElementManager();
    if (srcElements->find(id) == nullptr)
    {
        // 本场景不存在该元素
        LOG("Stage::transferE... failed to transfer a element to certain "
            "stage because it didn't exist.");
        return false;
    }
    if (dstElements->find(id) == nullptr)
    {
        // 对方不存在该元素
        auto element = srcElements->getElement(id);
        return dstElements->PushElement(std::move(element));
    }
    return true;
}

void Stage::onDestroy()
{
    LOG("Stage::onDestroy() The {} stage is going to destroy.",
        (uint8_t)stageType);
    Elements.reset();
}

void Stage::onUpdate()
{
    if (Elements == nullptr || timer == nullptr || sController == nullptr)
    {
        return;
    }
    else
    {
        Elements->onUpdate(timer->getTotalTime());
    }
}

void Stage::onRender()
{
    if (Elements == nullptr || timer == nullptr || sController == nullptr)
    {
        return;
    }
    else
    {
        Elements->onRender();
    }
}