#include "Eclipsea/Stage/Story/StoryStage.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Graphics/Manager/ElementManager.hpp"
#include "OpenCore/Runtime/Graphics/UI/ImageBoard.hpp"
#include "OpenCore/Runtime/Graphics/UI/TypeWriter.hpp"
#include "OpenCore/World/Stage/Stage.hpp"

#define firstLine                                                              \
    "在遥远未来的某一天，由于人类过度的消耗地球资源，生态圈有限的恢复能力终于" \
    "枯竭了。\n江河湖海几近于枯竭，空气中的湿度极具下降，地表的液态水资源正在" \
    "以异常的速度流失。"

#define secondLine                                                             \
    "起初，人类的科学家们尚对事态抱乐观态度，认为地表水资源中液态比例下降只是" \
    "暂时性的气候问题，只要假以时日，水循环一定能够重回正轨。\n然而，随着日子" \
    "一天一天过去，液态水资源愈发减少，大量生物和人类死去，期待的复苏并未到来" \
    "，恐惧随着死亡的气息在人群中传播。"

#define thirdLine                                                              \
    "更糟糕的是，一些国家的顶级富豪开始筹备逃离地球的计划，并且陆续发射了五艘" \
    "载人飞船，前往生态环境已经得到良好改善的火星。\n先前的发射任务大获成功，" \
    "越来越多人像抓住救命稻草一般拼命地争抢登船资格。\n在这样的氛围下，同时对" \
    "水资源枯竭的事态进入了停滞，先前那些抱有乐观态度的科学家已经十不存一，甚" \
    "至他们中的大多数都已经做好了前往火星的准备。"

#define fourthLine                                                             \
    "今晚将要发射的，则是这个"                                                 \
    "逃离航班的第13次发射\n——诺亚13号飞船。"

StoryStage::StoryStage(Timer *timer, StageManager *sController,
                       StoryStatus sStatus)
{
    this->timer = timer;
    this->sController = sController;

    this->stageType = overlayStage;

    Elements = std::make_unique<ElementManager>();

    this->targetStatus = sStatus;
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

bool StoryStage::parseEvents(Event *event)
{
    SDL_Event sEvent = *event;
    Elements->handlEvents(sEvent, timer->getTotalTime());
    return true;
}

void StoryStage::initializeComponents()
{
    switch (targetStatus)
    {
    case StoryStatus::Intro:
        func_intro();
        break;
    case StoryStatus::Dialog:
    {
        break;
    }
    default:
        break;
    }
}

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
        handleIntroUpdate();
        break;
    }
    case StoryStatus::Launching:
    {
        handleLaunchUpdate();
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

    auto frontpage = UI<ImageBoard>("frontpage", 70, 3002, 1, 1);

    frontpage->Configure()
        .Sequence(true)
        .Parent(nullptr)
        .Anchor(AnchorPoint::BottomCenter)
        .Posite(0.5f, 2.0f)
        .Alpha(1.0f)
        .Scale(1.0f, 0.0f);

    frontpage->Animate()
        .Timer(15.0f)
        .Move(960, 2160, 960, 1080, 10.0f)
        .Commit();

    Elements->PushElement(std::move(frontpage));

    auto tpwt = UI<TypeWriter>("typeWriter", 99, 9002, NULL, NULL);

    tpwt->Configure()
        .Sequence(true)
        .Parent(nullptr)
        .Alpha(1.0f)
        .Anchor(AnchorPoint::TopLeft)
        .Posite(2.5f, 0.55f)
        .Scale(0.6f, 0.5f);

    tpwt->Animate().Timer(27.0f).Move(1920, 560, -1152, 560, 20.0f).Commit();

    tpwt->setFontSize(48);
    tpwt->setText(firstLine);
    tpwt->alignCenter(false);

    Elements->PushElement(std::move(tpwt));

    sStatus = StoryStatus::Intro;
}

void StoryStage::handleIntroUpdate()
{
    auto element = Elements->find("typeWriter");
    if (!element)
    {
        LOG("错误！没有找到TYPEWRITER元素");
        return;
    }

    auto typeWriter = dynamic_cast<TypeWriter *>(element);

    if (!typeWriter->isAnimeFinished())
        return;

    switch (stageIndex)
    {
    case 0:
        handleIntroScrollText(typeWriter, secondLine);
        break;
    case 1:
        handleIntroScrollText(typeWriter, thirdLine);
        break;
    case 2:
        handleIntroCenterText(typeWriter);
        break;
    case 3:
        handleIntroVisualScene(typeWriter);
        break;
    case 4:
        handleIntroFadeAudio(typeWriter);
        break;
    case 5:
        handleIntroLaunch(typeWriter);
        break;
    case 6:
        handleLaunchMove(typeWriter);
        break;
    default:
        break;
    }
}

void StoryStage::handleIntroScrollText(TypeWriter *typeWriter,
                                       const std::string &text)
{
    typeWriter->setText(text);
    typeWriter->Animate()
        .Timer(0.5f)
        .Move(1920, 560, -1152, 560, 20.0f)
        .Commit();
    ++stageIndex;
}

void StoryStage::handleIntroCenterText(TypeWriter *typeWriter)
{
    typeWriter->setText(fourthLine);
    typeWriter->Animate()
        .Timer(0.5f)
        .Move(1920, 560, 384, 560, 10.0f)
        .Timer(10.0f)
        .Commit();
    ++stageIndex;
}

void StoryStage::handleIntroVisualScene(TypeWriter *typeWriter)
{
    typeWriter->Animate()
        .Timer(5.0f)
        .Move(384, 560, 384, 1080, 8.0f)
        .Timer(6.0f)
        .Commit();

    auto house = UI<ImageBoard>("house", 30, 2047, 1, 1);

    house->Configure()
        .Parent(nullptr)
        .Sequence(true)
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.5f)
        .Alpha(0.0f)
        .Scale(1.0f, 1.0f);

    house->Animate().Timer(5.0f).Fade(0.0f, 1.0f, 8.0f).Commit();

    Elements->PushElement(std::move(house));

    auto ele = Elements->find("frontpage");
    if (ele)
    {
        auto eleI = dynamic_cast<ImageBoard *>(ele);

        eleI->Configure().Anchor(AnchorPoint::TopCenter).Posite(0.5f, 0.2f);
        eleI->Animate()
            .SubStart(true)
            ->Fade(1.0f, 0.0f, 10.0f)
            .Scale(1.0f, 5.0f, 10.0f)
            .SubEnd()
            .Commit();
    }

    auto rocket = UI<ImageBoard>("rocket", 2, 3001, 1, 1);

    rocket->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::BottomCenter)
        .Posite(0.46f, 0.47f)
        .Scale(0.0f, 0.2f)
        .Alpha(0.0f)
        .Sequence(true);

    rocket->Animate().Timer(5.0f).Fade(0.0f, 1.0f, 8.0f).Commit();

    Elements->PushElement(std::move(rocket));
    ++stageIndex;
}

void StoryStage::handleIntroFadeAudio(TypeWriter *typeWriter)
{
    auto &sfx = OpenCoreManagers::SFXManager.getInstance();
    if (sfx.getVolume() > 0)
    {
        sfx.setVolume((sfx.getVolume() - 1 >= 0) ? sfx.getVolume() - 1 : 0);
    }
    else
    {
        typeWriter->Animate().Timer(5.0f).Commit();
        ++stageIndex;
    }
}

void StoryStage::handleIntroLaunch(TypeWriter *typeWriter)
{

    auto explosion = UI<ImageBoard>("flames", 1, 2051, 11, 11);

    explosion->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::Center)
        .Posite(0.46f, 0.47f)
        .Scale(0.2f, 0.0f);

    explosion->Animate().Frame(121, 15, true).Commit();

    Elements->PushElement(std::move(explosion));

    auto house = Elements->find("house");
    if (house)
    {
        auto houseE = dynamic_cast<ImageBoard *>(house);

        houseE->Animate().Fade(1.0f, 0.0f, 5.0f).Commit();
    }

    ++stageIndex;

    typeWriter->Animate().Timer(8.0f).Commit();
    // 开始播放倒计时音效
}

void StoryStage::handleLaunchUpdate()
{
    // 开始处理
    auto element = Elements->find("typeWriter");

    if (!element)
    {
        return;
    }

    if (element->isAnimeFinished())
    {
        auto typeWriter = dynamic_cast<TypeWriter *>(element);
        switch (stageIndex)
        {
        case 0:
        {
            handleExplosion(typeWriter);
            break;
        }
        case 1:
        {
            Elements->removeElement("rocket");
            Elements->removeElement("flames");
            Elements->removeElement("house");
            Elements->removeElement("exp");
            Elements->removeElement("background");
            typeWriter->Animate().Timer(12.0f);
            auto centerText = UI<TextArea>("text", 99, 9002, NULL, NULL);

            centerText->Configure()
                .Parent(nullptr)
                .Anchor(AnchorPoint::Center)
                .Posite(0.5f, 0.5f)
                .Scale(0.8f, 0.2f)
                .Alpha(0.0f);
            centerText->alignCenter(true);
            centerText->setText("两年后....");
            centerText->setFontSize(50);

            centerText->Animate().Fade(0.0f, 1.0f, 4.0f).Commit();

            Elements->PushElement(std::move(centerText));

            ++stageIndex;
            break;
        }
        default:
            break;
        }
    }
}

void StoryStage::handleLaunchMove(TypeWriter *typeWriter)
{
    sStatus = StoryStatus::Launching;
    stageIndex = 0;

    auto orocket = Elements->find("rocket");
    auto oflames = Elements->find("flames");

    if (orocket)
    {
        auto rocket = dynamic_cast<ImageBoard *>(orocket);
        auto flames = dynamic_cast<ImageBoard *>(oflames);

        rocket->Animate().Move(883.2f, 507.6f, 960, 540, 5.0f).Commit();
        flames->Animate().Move(883.2f, 507.6f, 960, 540, 5.0f).Commit();
    }

    typeWriter->Animate().Timer(5.0f);
}

void StoryStage::handleExplosion(TypeWriter *typeWriter)
{
    auto explosion = UI<ImageBoard>("exp", 99, 2050, 11, 11);

    explosion->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.5f)
        .Scale(0.5f, 0.0f);

    explosion->Animate().Frame(121, 11).Commit();

    Elements->PushElement(std::move(explosion));

    typeWriter->Animate().Timer(6.0f);
    ++stageIndex;
}

#pragma endregion