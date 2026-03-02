#include "OpenCore/OpenCore.hpp"
#include <memory>

MapExplorer::MapExplorer(const string &id, short layer)
    : UIElement(id, layer, nullptr)
{
    // 初始化函数
}

void MapExplorer::onEnter()
{
    // 正式开始渲染准备
    if (status == MapExpStatus::Creating)
    {
        // 加载地图
        map = std::make_unique<ClassicMap>(1, mapPath);
        map->onEnter();

        // 渲染 worker 初始化
        renderWorker = UI<ImageBoard>("worker", 0, 2035, 16, 16);
        renderWorker->Configure()
            .Parent(this)
            .Anchor(AnchorPoint::Center)
            .Alpha(0.0f);
        // 设置缩放，Y=0 表示保持比例
        uint8_t renderWidth = *OpenCoreManagers::SetManager.getRenderWidth();
        uint8_t renderHeight = *OpenCoreManagers::SetManager.getRenderHeight();

        widthFactor = 1.0f / renderWidth;
        heightFactor = 1.0f / renderHeight;

        renderWorker->setScale(widthFactor, 0.0f);

        status = MapExpStatus::Ready;
    }
}

void MapExplorer::onRender()
{
    if (status != MapExpStatus::Ready)
        return;

    // 遍历屏幕渲染格子
    for (int y = -up_border; y <= up_border; ++y)
    {
        for (int x = -left_border; x <= left_border; ++x)
        {
            int gx = x + px; // 地图坐标
            int gy = y + py;

            // 获取 BlockInfo，越界自动返回 (0,0)
            BlockInfo &info = map->getBlockInfo(gx, gy);

            // 设置渲染位置
            renderWorker->setPosition(x * widthFactor + 0.5f,
                                      0.5f + y * heightFactor);
            renderWorker->setTransparency(1.0f);

            renderWorker->getAnimationState()->frameIndex = info.Terrain;

            // 绘制格子
            renderWorker->onRender();
        }
    }
}