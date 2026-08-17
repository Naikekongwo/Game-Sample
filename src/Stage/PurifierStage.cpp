#include "Core/Macros.hpp"
#include "Eclipsea/Core/EclipseaTextures.hpp"
#include "Eclipsea/Core/GameSettings.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "Eclipsea/Gameplay/Gameplay.hpp"
#include "Eclipsea/Gameplay/Sprite/ItemSprite.hpp"
#include "Eclipsea/Gameplay/UI/ItemContainer.hpp"
#include "OpenCore.hpp"
#include "Runtime/Animation/IAnimation.hpp"
#include "Runtime/Graphics/UI/ImageBoard.hpp"
#include "Runtime/Graphics/UI/TextArea.hpp"
#include <SDL3/SDL.h>
#include <format>
#include <optional>

// PurifierStage（新 OpenCore）：timer / sController 由 StageManager 自动注入
PurifierStage::PurifierStage()
{
    // OverlayStage 基类已设置 stageType 与 Elements
}

bool PurifierStage::parseEvents(Event *event)
{
    const SDL_Event &sdlEvent = event->GetSDLEvent();
    if (sdlEvent.type == SDL_EVENT_KEY_DOWN)
    {
        if (sdlEvent.key.key == SDLK_ESCAPE)
        {
            if (phase != PurifierPhase::Normal)
                return true; // 防止多次点击
            phase = PurifierPhase::Exiting;
        }
    }

    Elements->parseEvents(event, timer->getTotalTime());

    if (ItemPickedUp)
    {
        ItemPickedUp->getVisualState()->Position[0] = sdlEvent.motion.x;
        ItemPickedUp->getVisualState()->Position[1] = sdlEvent.motion.y;
    }
    return true;
}

void PurifierStage::onUpdate()
{
    Elements->onUpdate(timer->getTotalTime());

    auto  itemName      = Elements->find("item_name");
    auto &settings      = Eclipsea::GameSettings::getInstance();
    auto  wrdController = &getServerWorldController();

    if (wrdController->queryHomelessItemInfo() != std::nullopt)
    {
        if (itemName)
        {
            auto itemNamePtr = dynamic_cast<TextArea *>(itemName);
            switch (wrdController->queryHomelessItemInfo()->typeID)
            {
            case 1:
                itemNamePtr->setText("空瓶");
                break;
            case 2:
                itemNamePtr->setText("满瓶");
                break;
            case 3:
                itemNamePtr->setText("基础控制单元");
                break;
            case 4:
                itemNamePtr->setText("均衡控制单元");
                break;
            case 5:
                itemNamePtr->setText("高级控制单元");
                break;
            case 6:
                itemNamePtr->setText("基础反应堆");
                break;
            case 7:
                itemNamePtr->setText("均衡反应堆");
                break;
            case 8:
                itemNamePtr->setText("高级反应堆");
                break;
            case 9:
                itemNamePtr->setText("基础储水罐");
                break;
            case 10:
                itemNamePtr->setText("均衡储水罐");
                break;
            case 11:
                itemNamePtr->setText("高级储水罐");
                break;
            }
        }
    }
    else if (itemName)
    {
        auto  itemNamePtr = dynamic_cast<TextArea *>(itemName);
        float water       = *settings.getWaterStorage();
        float maxCap      = *settings.getWaterStorageMax();
        float purified    = *settings.getPurifiedWaterStorage();
        itemNamePtr->setText(
            std::format("水箱容量：{:.0f}/{:.0f} 纯净水储量：{:.1f}", water,
                        maxCap, purified));
    }

    // 根据纯净水量切换字体颜色: >250 天蓝, 否则白色
    if (itemName)
    {
        auto  itemNamePtr = dynamic_cast<TextArea *>(itemName);
        float purified    = *settings.getPurifiedWaterStorage();
        if (purified > 250.0f)
            itemNamePtr->setTextColor(135, 206, 235);
        else
            itemNamePtr->setTextColor(255, 255, 255);
    }

    // 根据容器中的物品更新净水器参数
    {
        auto purifierBackpack = wrdController->getBackpackByEntityID(100);

        auto &purifyRate = *settings.getWaterPurifyRate();
        auto &storageMax = *settings.getWaterStorageMax();

        // 核反应堆 (nuclear_item, index 0, typeID 6~8) → waterPurifyRate
        auto nuclearSlot = purifierBackpack->getItem(0);
        if (nuclearSlot.has_value() && nuclearSlot->item.has_value())
        {
            short typeID = nuclearSlot->item->getTypeID();
            if (typeID >= 6 && typeID <= 8)
                purifyRate =
                    static_cast<float>(typeID - 5); // 6→1.0, 7→2.0, 8→3.0
            else
                purifyRate = 1.0f;
        }
        else
        {
            purifyRate = 1.0f;
        }

        // 储水罐/出水管 (storage_item, index 2, typeID 9~11) → waterStorageMax
        auto storageSlot = purifierBackpack->getItem(2);
        if (storageSlot.has_value() && storageSlot->item.has_value())
        {
            short typeID = storageSlot->item->getTypeID();
            if (typeID == 9)
                storageMax = 1000.0f;
            else if (typeID == 10)
                storageMax = 2000.0f;
            else if (typeID == 11)
                storageMax = 3000.0f;
            else
                storageMax = 1.0f;
        }
        else
        {
            storageMax = 1.0f;
        }

        // 出水口 (filter_item, index 3):
        //   空瓶(typeID=1) → 消耗250纯净水 → 满瓶直接放入玩家背包
        //   满瓶(typeID=2) → waterStorage+250 → 空瓶直接放入玩家背包
        auto filterSlot = purifierBackpack->getItem(3);
        if (filterSlot.has_value() && filterSlot->item.has_value())
        {
            short typeID         = filterSlot->item->getTypeID();
            auto  playerBackpack = wrdController->getBackpackByEntityID(1);

            if (typeID == 1)
            {
                auto &purified = *settings.getPurifiedWaterStorage();
                if (purified >= 250.0f)
                {
                    purified -= 250.0f;
                    purifierBackpack->removeItem(3); // 清空出水口
                    playerBackpack->addItem(2, 1);   // 满瓶放入玩家背包
                    LOG("出水口: 空瓶接满, 满瓶已放入背包");
                }
            }
            else if (typeID == 2)
            {
                auto &water = *settings.getWaterStorage();
                water += 250.0f;
                purifierBackpack->removeItem(3); // 清空出水口
                playerBackpack->addItem(1, 1);   // 空瓶放回玩家背包
                LOG("出水口: 满瓶放水, 空瓶已放回背包");
            }
        }
    }

    // 净水逻辑: 消耗 waterStorage → 产出 purifiedWaterStorage
    {
        float &water    = *settings.getWaterStorage();
        float &purified = *settings.getPurifiedWaterStorage();
        float  rate     = *settings.getWaterPurifyRate();

        float amount = 5.0f * rate * static_cast<float>(timer->getDeltaTime());
        if (water >= amount)
        {
            water -= amount;
            purified += amount;
        }
        else if (water > 0.0f)
        {
            purified += water;
            water = 0.0f;
        }
    }

    if (phase == PurifierPhase::Exiting)
    {
        auto purifier = Elements->find("purifier_body");
        if (purifier && purifier->isAnimeFinished())
        {
            phase = PurifierPhase::Finished;
        }
    }

    if (phase == PurifierPhase::Finished)
    {
        if (sController)
            sController->removeStage(this->stageType);
    }
}

void PurifierStage::onRender()
{
    Elements->onRender();

    auto item = getServerWorldController().queryHomelessItemInfo();

    if (item.has_value())
    {
        if (!ItemPickedUp)
        {
            ItemPickedUp = std::make_unique<ItemSprite>();

            ItemPickedUp->Configure()
                .Parent(nullptr)
                .Anchor(AnchorPoint::Center)
                .PositeR(0.5f, 0.5f)
                .ScaleR(0.1f, 0.1f * widthheight);
        }

        auto meta = Gameplay::ItemMgr.getTextureMeta(item->textureMetaName);

        if (!meta.has_value())
            return;

        ItemPickedUp->changeTexture(MakeTexture(
            meta->texture_cols, meta->texture_rows, meta->textureName));
        ItemPickedUp->setSubTexture(item->texturePosID);
        ItemPickedUp->Draw();
    }
}

void PurifierStage::onEnter()
{
    LOG("PurifierStage::onEnter");
    getServerWorldController().giveUpHomelessItem();
    initializeComponents();
}

void PurifierStage::onExit()
{
    Elements->onDestroy();
    getServerWorldController().giveUpHomelessItem();
    LOG("PurifierStage::onExit - cleared elements");
}

void PurifierStage::onDestroy() { LOG("PurifierStage::onDestroy"); }

void PurifierStage::initializeComponents()
{
    auto wrdController = &getServerWorldController();
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
            if (phase != PurifierPhase::Normal)
                return; // 防止多次点击
            phase = PurifierPhase::Exiting;
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

    auto nuclear_text = UI<TextArea>("nuclear_text", 30, "9002", 0, 0);
    auto control_text = UI<TextArea>("control_text", 30, "9002", 0, 0);
    auto storage_text = UI<TextArea>("storage_text", 30, "9002", 0, 0);
    auto filter_text  = UI<TextArea>("filter_text", 30, "9002", 0, 0);

    nuclear_text->Configure()
        .Parent(nullptr)
        .ScaleR(0.1f, 0.04f)
        .Anchor(AnchorPoint::TopLeft)
        .PositeR(0.617f, 0.124f)
        .Sequence(true);

    nuclear_text->setText("反应堆");
    nuclear_text->setFontSize(
        Eclipsea::GameSettings::getInstance().designFontSize(45));
    nuclear_text->alignCenter(false);

    storage_text->Configure()
        .Parent(nullptr)
        .ScaleR(0.1f, 0.04f)
        .Anchor(AnchorPoint::TopLeft)
        .PositeR(0.617f, 0.36f)
        .Sequence(true);

    storage_text->setText("储水罐");
    storage_text->setFontSize(
        Eclipsea::GameSettings::getInstance().designFontSize(45));
    storage_text->alignCenter(false);

    control_text->Configure()
        .Parent(nullptr)
        .ScaleR(0.1f, 0.04f)
        .Anchor(AnchorPoint::TopLeft)
        .PositeR(0.521f, 0.237f)
        .Sequence(true);
    control_text->setText("控制单元");
    control_text->setFontSize(
        Eclipsea::GameSettings::getInstance().designFontSize(45));
    control_text->alignCenter(false);

    filter_text->Configure()
        .Parent(nullptr)
        .ScaleR(0.1f, 0.04f)
        .Anchor(AnchorPoint::TopLeft)
        .PositeR(0.71f, 0.237f)
        .Sequence(true);
    filter_text->setText("出水口");
    filter_text->setFontSize(
        Eclipsea::GameSettings::getInstance().designFontSize(45));
    filter_text->alignCenter(false);

    Elements->PushElement(std::move(nuclear_text));
    Elements->PushElement(std::move(control_text));
    Elements->PushElement(std::move(storage_text));
    Elements->PushElement(std::move(filter_text));

    auto nuclear_item = std::make_unique<ItemContainer>(
        "nuclear_item", 30, MakeTexture(1, 1, "img_itemcontain"), 1, 1);
    auto control_item = std::make_unique<ItemContainer>(
        "control_item", 30, MakeTexture(1, 1, "img_itemcontain"), 1, 1);
    auto storage_item = std::make_unique<ItemContainer>(
        "storage_item", 30, MakeTexture(1, 1, "img_itemcontain"), 1, 1);
    auto filter_item = std::make_unique<ItemContainer>(
        "filter_item", 30, MakeTexture(1, 1, "img_itemcontain"), 1, 1);

    nuclear_item->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::TopLeft)
        .ScaleR(0.07f, 0.0f)
        .PositeR(0.617f, 0.17f);

    control_item->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::TopLeft)
        .ScaleR(0.07f, 0.0f)
        .PositeR(0.521f, 0.285f);

    storage_item->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::TopLeft)
        .ScaleR(0.07f, 0.0f)
        .PositeR(0.617f, 0.4f);

    filter_item->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::TopLeft)
        .ScaleR(0.07f, 0.0f)
        .PositeR(0.71f, 0.285f);

    Elements->PushElement(std::move(nuclear_item));
    Elements->PushElement(std::move(control_item));
    Elements->PushElement(std::move(storage_item));
    Elements->PushElement(std::move(filter_item));
}
