#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"

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
    auto stageBg =
        UI<BaseBackground>("purSbg", 0, background_purifier, NULL, NULL);
    stageBg->setNativeScale(60);
    stageBg->Configure()
        .Scale(0.82f, 0.5f)
        .Anchor(AnchorPoint::Center)
        .Parent(nullptr)
        .Posite(0.5f, 0.28f)
        .Sequence(true);
    Elements->PushElement(std::move(stageBg));

    // 返回按钮
    auto backButton = UI<Button>("backButton", 1, img_BackButton, 1, 3);
    backButton->Configure()
        .Scale(0.03f, 0.03f)
        .Posite(0.88f, 0.05f)
        .Anchor(AnchorPoint::Center)
        .Sequence(false)
        .Parent(nullptr);

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
                        .Fade(state->transparency, 0.0f, 5.0f)
                        .Commit();
                });
        });

    Elements->PushElement(std::move(backButton));

    // 净水器状态
    auto purifierbody = UI<ImageBoard>("purifier_body", 1, item_purifier, 1, 1);

    purifierbody->Configure()
        .Scale(0.146f, 0.182f)
        .Anchor(AnchorPoint::TopLeft)
        .Posite(0.2f, 0.08f)
        .Sequence(false)
        .Parent(nullptr);

    Elements->PushElement(std::move(purifierbody));

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