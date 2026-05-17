#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Graphics/UI/ImageBoard.hpp"
#include "OpenCore/Runtime/Graphics/UI/ItemContainer.hpp"

PurifierStage::PurifierStage(Timer *timer, StageManager *sController)
{
    this->timer = timer;
    this->sController = sController;
    stageType = StageType::overlayStage;
    Elements = std::make_unique<ElementManager>();
}

bool PurifierStage::handlEvents(SDL_Event *event)
{
    Elements->handlEvents(*event, timer->getTotalTime());
    return true;
}

void PurifierStage::onUpdate()
{
    Elements->onUpdate(timer->getTotalTime());

    if (phase == PurifierPhase::Exiting)
    {
        auto purifier = Elements->find("purifier_body");
        if (purifier)
        {
            if (purifier->isAnimeFinished())
            {
                phase = PurifierPhase::Finished;
            }
        }
    }

    if (phase == PurifierPhase::Finished)
    {
        if (sController)
            sController->removeStage(this->stageType);
    }
}
void PurifierStage::onRender() { Elements->onRender(); }

bool PurifierStage::buildStage()
{
    // 背景
    auto stageBg = UI<BaseBackground>("purSbg", 0, 2009, NULL, NULL);
    stageBg->setNativeScale(120);
    stageBg->Configure()
        .Parent(nullptr)
        .Scale(0.82f, 0.95f)
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.5f)
        .Sequence(true);
    Elements->PushElement(std::move(stageBg));

    // 返回按钮
    auto backButton = UI<Button>("backButton", 1, img_BackButton, 1, 3);
    backButton->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::Center)
        .Scale(0.03f, 0.0f)
        .Posite(0.87f, 0.1f)
        .Sequence(false);

    // 点击回调
    backButton->setOnClick(
        [this]()
        {
            if (phase != PurifierPhase::Normal)
                return; // 防止多次点击
            phase = PurifierPhase::Exiting;
            Elements->forEachElement(
                [](auto &elem)
                {
                    auto state = elem->getVisualState();
                    LOG("Adding fade out effect to {}, transparency, {}",
                        elem->getID().c_str(), state->transparency);
                    elem->Animate()
                        .Fade(state->transparency, 0.0f, 0.1f)
                        .Commit();
                });
        });

    Elements->PushElement(std::move(backButton));

    // 净水器状态
    auto purifierbody = UI<ImageBoard>("purifier_body", 1, item_purifier, 1, 1);

    purifierbody->Configure()
        .Parent(nullptr)
        .Scale(0.146f, 0.323f)
        .Anchor(AnchorPoint::TopLeft)
        .Posite(0.2f, 0.08f)
        .Sequence(false);

    Elements->PushElement(std::move(purifierbody));

    auto effect =
        UI<ImageBoard>("purifier_effect", 2, effects_water_bubbles, 5, 9);

    effect->Configure()
        .Parent(nullptr)
        .Scale(0.146f, 0.0f)
        .Anchor(AnchorPoint::TopLeft)
        .Posite(0.2f, 0.08f)
        .Sequence(false);

    effect->Animate().Frame(45, 15, true).Commit();

    Elements->PushElement(std::move(effect));

    auto nuclear_text = UI<TextArea>("nuclear_text", 30, 9002, NULL, NULL);
    auto control_text = UI<TextArea>("control_text", 30, 9002, NULL, NULL);
    auto storage_text = UI<TextArea>("storage_text", 30, 9002, NULL, NULL);
    auto filter_text = UI<TextArea>("filter_text", 30, 9002, NULL, NULL);

    nuclear_text->Configure()
        .Parent(nullptr)
        .Scale(0.1f, 0.04f)
        .Anchor(AnchorPoint::TopLeft)
        .Posite(0.617f, 0.124f)
        .Sequence(true);

    nuclear_text->setText("反应堆");
    nuclear_text->setFontSize(45);
    nuclear_text->alignCenter(false);

    storage_text->Configure()
        .Parent(nullptr)
        .Scale(0.1f, 0.04f)
        .Anchor(AnchorPoint::TopLeft)
        .Posite(0.617f, 0.36f)
        .Sequence(true);

    storage_text->setText("储水罐");
    storage_text->setFontSize(45);
    storage_text->alignCenter(false);

    control_text->Configure()
        .Parent(nullptr)
        .Scale(0.1f, 0.04f)
        .Anchor(AnchorPoint::TopLeft)
        .Posite(0.521f, 0.237f)
        .Sequence(true);
    control_text->setText("控制单元");
    control_text->setFontSize(45);
    control_text->alignCenter(false);

    filter_text->Configure()
        .Parent(nullptr)
        .Scale(0.1f, 0.04f)
        .Anchor(AnchorPoint::TopLeft)
        .Posite(0.71f, 0.237f)
        .Sequence(true);
    filter_text->setText("过滤器");
    filter_text->setFontSize(45);
    filter_text->alignCenter(false);

    Elements->PushElement(std::move(nuclear_text));
    Elements->PushElement(std::move(control_text));
    Elements->PushElement(std::move(storage_text));
    Elements->PushElement(std::move(filter_text));

    auto nuclear_item = UI<ItemContainer>("nuclear_item", 30, 2027, 1, 1);
    auto control_item = UI<ItemContainer>("control_item", 30, 2027, 1, 1);
    auto storage_item = UI<ItemContainer>("storage_item", 30, 2027, 1, 1);
    auto filter_item = UI<ItemContainer>("filter_item", 30, 2027, 1, 1);

    nuclear_item->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::TopLeft)
        .Scale(0.07f, 0.0f)
        .Posite(0.617f, 0.17f);

    control_item->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::TopLeft)
        .Scale(0.07f, 0.0f)
        .Posite(0.521f, 0.285f);

    storage_item->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::TopLeft)
        .Scale(0.07f, 0.0f)
        .Posite(0.617f, 0.41f);

    filter_item->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::TopLeft)
        .Scale(0.07f, 0.0f)
        .Posite(0.71f, 0.285f);

    Elements->PushElement(std::move(nuclear_item));
    Elements->PushElement(std::move(control_item));
    Elements->PushElement(std::move(storage_item));
    Elements->PushElement(std::move(filter_item));

    return true;
}
void PurifierStage::onEnter()
{
    LOG("PurifierStage::onEnter");
    buildStage();
}

void PurifierStage::onExit()
{
    Elements->onDestroy();
    LOG("PurifierStage::onExit - cleared elements");
}

void PurifierStage::onDestroy() { LOG("PurifierStage::onDestroy"); }

bool PurifierStage::parseEvents(Event *event) { return true; }

void PurifierStage::initializeComponents() {}