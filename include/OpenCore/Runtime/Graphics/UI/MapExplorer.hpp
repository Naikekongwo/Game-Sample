/**
 * @file MapExplorer.hpp
 * @brief 地图探索窗口 UI 控件。
 * 
 * 提供游戏世界地图的交互式浏览功能，允许通过键盘（W/A/S/D/空格）移动视角，
 * 并委托 WorldController 进行底层地图渲染。
 */

#ifndef _MAP_EXPLORER_HPP_
#define _MAP_EXPLORER_HPP_

#include "OpenCore/Runtime/Gameplay/Physics/PhysicalProperties.h"
#include "OpenCore/Runtime/Gameplay/WorldController/IWorldController.hpp"
#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"

/**
 * @enum MapExpStatus
 * @brief 地图探索器的内部状态。
 */
enum class MapExpStatus
{
    Creating,   ///< 创建中，尚未完成初始化
    Ready       ///< 就绪，可正常绘制与交互
};

class ClassicMap;   
class ImageBoard;   

/**
 * @class MapExplorer
 * @brief 地图浏览控件。
 * 
 * 继承自 UIElement，在就绪后显示由 WorldController 管理的地图，
 * 并响应键盘输入以移动观察点（通过修改物理属性中的速度）。
 */
class MapExplorer : public UIElement
{
public:
    /**
     * @brief 构造地图探索器对象。
     * @param id    唯一标识符。
     * @param layer 渲染图层。
     */
    MapExplorer(const string &id, short layer);
    void onEnter() override;
    void Draw() override;
    void onUpdate(float totalTime) override;
    void onExit() override;
    void handlEvents(SDL_Event &event, float totalTime) override;

private:
    float widthFactor = 1.0f;          ///< 宽度缩放因子（当前未使用）
    float heightFactor = 1.0f;         ///< 高度缩放因子（当前未使用）
    MapExpStatus status = MapExpStatus::Creating; ///< 当前状态
};

#endif //_MAP_EXPLORER_HPP_