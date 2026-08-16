#pragma once

// Eclipsea.hpp
// 项目实际的头库（基于新 OpenCore 26.1）
//
// 新版 OpenCore 已放弃「script 清单 + 数字 ID」资源加载模式，资源统一
// 以字符串名称注册与访问。旧版用于桥接资源文件与源代码调用的数字 ID
// 宏已全部移除。

// 调试开关：世界编辑器按钮（原 DEBUG_MAP 模式）
#ifndef ECLIPSEA_DEBUG_WORLD
#define ECLIPSEA_DEBUG_WORLD 0
#endif

#include "OpenCore.hpp"

#include "Stage/ContainerStage.hpp"
#include "Stage/DualGameplayStage.hpp"
#include "Stage/GameplayStage.hpp"
#include "Stage/MainStage.hpp"
#include "Stage/PauseStage.hpp"
#include "Stage/PreloadStage.hpp"
#include "Stage/PurifierStage.hpp"
#include "Stage/WorldEditorStage.hpp"

#include "Stage/Story/StoryStage.hpp"

#include "Stage/MainStage/ContinueStage.hpp"
#include "Stage/MainStage/SettingsStage.hpp"

class EclipseaApp
{
  public:
    bool StartUp();
    bool CleanUp();

  private:
};
