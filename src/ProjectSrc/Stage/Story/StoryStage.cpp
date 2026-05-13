#include "Eclipsea/Stage/Story/StoryStage.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Graphics/Manager/ElementManager.hpp"
#include "OpenCore/Runtime/Graphics/UI/ImageBoard.hpp"
#include "OpenCore/Runtime/Graphics/UI/TypeWriter.hpp"
#include "OpenCore/World/Stage/Stage.hpp"

#define firstLine                                                              \
    "公元 2300年 "                                                             \
    "X市\n地球水资源危机已经进入终末期，先前对生态恢复抱有幻想的科学家十不存"  \
    "一，甚至极大多数已经登上了早些时候的诺亚13号飞船。\n地球即将进入无人居住" \
    "的窘态。"

#define secondLine "XXXXXXXXXXX\nXXXXXXXXX"

StoryStage::StoryStage(Timer *timer, StageManager *sController)
{
    this->timer = timer;
    this->sController = sController;

    this->stageType = overlayStage;

    Elements = std::make_unique<ElementManager>();
}

void StoryStage::onEnter()
{
    if (sStatus > StoryStatus::Loading)
        return;
    initializeComponents();
}

bool StoryStage::handlEvents(SDL_Event *event)
{
    Event new_event = Event(*event);
    return parseEvents(&new_event);
}

bool StoryStage::parseEvents(Event *event) { return true; }

void StoryStage::initializeComponents() { func_intro(); }

void StoryStage::onUpdate()
{
    Elements->onUpdate(timer->getTotalTime());

    switch (sStatus)
    {
    case StoryStatus::Loading:
    {
        LOG("Loading");
        break;
    }
    case StoryStatus::Ready:
    {
        LOG("Ready");
        break;
    }
    case StoryStatus::Intro:
    {
        // 等待
        auto element = Elements->find("typeWriter");
        if (!element)
        {
            LOG("错误！没有找到TYPEWRITER元素");
            return;
        }

        auto typeWriter = dynamic_cast<TypeWriter *>(element);

        if (typeWriter->isAnimeFinished())
        {
            // 动画结束

            string buffer;
            buffer = firstLine;
            if (stageIndex == 0)
            {
                typeWriter->setText(secondLine);
                typeWriter->Animate()
                    .Timer(0.5f)
                    .Move(2880, 560, -960, 560, 20.0f)
                    .Commit();
                stageIndex = 1;
                return;
            }
        }
        break;
    }
    default:
        break;
    }
}

void StoryStage::onRender() { Elements->onRender(); }

#pragma region 剧情部分

void StoryStage::func_intro()
{
    if (sStatus > StoryStatus::Ready)
        return;

    OpenCoreManagers::SFXManager.getInstance().changeBGM(1004);

    auto background = UI<ImageBoard>("background", 0, 3003, 1, 1);

    background->Configure()
        .Sequence(true)
        .Parent(nullptr)
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.5f)
        .Alpha(0.0f)
        .Scale(1.0f, 0.0f);

    background->Animate().Timer(5.0f).Fade(0.0f, 1.0f, 5.0f).Commit();

    Elements->PushElement(std::move(background));

    auto frontpage = UI<ImageBoard>("frontpage", 2, 3002, 1, 1);

    frontpage->Configure()
        .Sequence(true)
        .Parent(nullptr)
        .Anchor(AnchorPoint::BottomCenter)
        .Posite(0.5f, 2.0f)
        .Alpha(1.0f)
        .Scale(1.0f, 0.0f);

    frontpage->Animate().Timer(15.0f).Move(960, 2160, 960, 1080, 5.0f).Commit();

    Elements->PushElement(std::move(frontpage));

    auto tpwt = UI<TypeWriter>("typeWriter", 99, 9002, NULL, NULL);

    tpwt->Configure()
        .Sequence(true)
        .Parent(nullptr)
        .Alpha(1.0f)
        .Anchor(AnchorPoint::TopCenter)
        .Posite(2.5f, 0.55f)
        .Scale(0.5f, 0.5f);

    tpwt->Animate().Timer(22.0f).Move(2880, 560, -960, 560, 20.0f).Commit();

    tpwt->setFontSize(48);
    tpwt->setText(firstLine);
    tpwt->alignCenter(true);

    Elements->PushElement(std::move(tpwt));

    sStatus = StoryStatus::Intro;
}

#pragma endregion