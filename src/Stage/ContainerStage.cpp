#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore.hpp"
#include "Runtime/Graphics/UI/ImageBoard.hpp"

// ContainerStage（新 OpenCore）：timer / sController 由 StageManager 自动注入
ContainerStage::ContainerStage(shared_ptr<Backpack> backpack)
{
    // OverlayStage 基类已设置 stageType 与 Elements
    this->m_backpack = backpack;
}

void ContainerStage::onUpdate()
{
    Elements->onUpdate(timer->getTotalTime());

    if (phase == ContainerPhase::Exiting)
    {
        auto purifier = Elements->find("purifier_body");
        if (purifier)
        {
            if (purifier->isAnimeFinished())
            {
                phase = ContainerPhase::Finished;
            }
        }
    }

    if (phase == ContainerPhase::Finished)
    {
        if (sController)
            sController->removeStage(this->stageType);
    }
}
void ContainerStage::onRender() { Elements->onRender(); }

void ContainerStage::onEnter()
{
    LOG("ContainerStage::onEnter");
    initializeComponents();
}

void ContainerStage::onExit()
{
    Elements->onDestroy();
    LOG("ContainerStage::onExit - cleared elements");
}

void ContainerStage::onDestroy() { LOG("ContainerStage::onDestroy"); }

bool ContainerStage::parseEvents(Event *event)
{
    Elements->parseEvents(event, timer->getTotalTime());
    return true;
}

void ContainerStage::initializeComponents()
{
    // 背景
    auto stageBg = UI<BaseBackground>("purSbg", 0, "background_purifier", 0, 0);
    stageBg->setNativeScale(120);
    stageBg->Configure()
        .Parent(nullptr)
        .ScaleR(0.82f, 0.95f)
        .Anchor(AnchorPoint::Center)
        .PositeR(0.5f, 0.5f)
        .Sequence(true);
    Elements->PushElement(std::move(stageBg));

    // 返回按钮
    auto backButton = UI<Button>("backButton", 1, "img_BackButton", 1, 3);
    backButton->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::Center)
        .ScaleR(0.03f, 0.0f)
        .PositeR(0.87f, 0.1f)
        .Sequence(false);

    // 点击回调
    backButton->setOnClick(
        [this]()
        {
            if (phase != ContainerPhase::Normal)
                return; // 防止多次点击
            phase = ContainerPhase::Exiting;
            Elements->forEachElement(
                [](auto &elem)
                {
                    auto state = elem->getVisualState();
                    LOG("向元素 {} 添加渐变效果, 透明度为, {}",
                        elem->getID().c_str(), state->transparency);
                    elem->Animate()
                        .Fade(state->transparency, 0.0f, 0.1f)
                        .Commit();
                });
        });

    Elements->PushElement(std::move(backButton));

    // 净水器状态
    auto purifierbody =
        UI<ImageBoard>("purifier_body", 1, "item_purifier", 1, 1);

    purifierbody->Configure()
        .Parent(nullptr)
        .ScaleR(0.146f, 0.323f)
        .Anchor(AnchorPoint::TopLeft)
        .PositeR(0.2f, 0.08f)
        .Sequence(false);

    Elements->PushElement(std::move(purifierbody));

    auto effect =
        UI<ImageBoard>("purifier_effect", 2, "effects_water_bubbles", 5, 9);

    effect->Configure()
        .Parent(nullptr)
        .ScaleR(0.146f, 0.0f)
        .Anchor(AnchorPoint::TopLeft)
        .PositeR(0.2f, 0.08f)
        .Sequence(false);

    effect->Animate().Frame(45, 15, true).Commit();

    Elements->PushElement(std::move(effect));
}
