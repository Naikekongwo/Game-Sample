/**
 * @file MapExplorer.hpp
 * @brief 地图探索窗口 UI 控件。
 *
 * 提供游戏世界地图的交互式浏览功能，允许通过键盘（W/A/S/D/空格）移动视角，
 * 并委托 WorldController 进行底层地图渲染。
 */

#ifndef _MAP_EXPLORER_HPP_
#define _MAP_EXPLORER_HPP_

#include "ItemContainer.hpp"
#include "OpenCore/Runtime/Gameplay/Physics/PhysicalProperties.h"
#include "OpenCore/Runtime/Gameplay/WorldController/WorldController.hpp"
#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"
#include "OpenCore/Runtime/Graphics/Sprite/HealthBar.hpp"
#include <memory>

/**
 * @enum MapExpStatus
 * @brief 地图探索器的内部状态。
 */
enum class MapExpStatus
{
    Creating, ///< 创建中，尚未完成初始化
    Ready     ///< 就绪，可正常绘制与交互
};

enum class ViewportType
{
    Fullscreen,
    LeftHalf,
    RightHalf,
    Free
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

    /**
     * @brief 设置地图窗口的世界绑定
     *
     * @param wrdController
     * @return true
     * @return false
     */
    bool setWorldController(WorldController *wrdController);

    /**
     * @brief 设置地图管理器的视口
     *
     * @param vType
     */
    void setExplorerViewPort(ViewportType vType);

    bool setIndex(short ID)
    {
        m_focusEntityIndex = ID;
        return true;
    }

  private:
    float widthFactor = 1.0f;  ///< 宽度缩放因子（当前未使用）
    float heightFactor = 1.0f; ///< 高度缩放因子（当前未使用）
    MapExpStatus status = MapExpStatus::Creating; ///< 当前状态

    WorldController *m_wrdController = nullptr;  // 世界控制器
    short m_focusEntityIndex = ENTITY_PLAYER_ID; // 焦点角色ID

    ViewportType vType = ViewportType::Fullscreen; // 视口类型

    uint8_t renderRangeX = RENDER_RANGE_X; // 渲染范围X
    uint8_t renderRangeY = RENDER_RANGE_Y; // 渲染范围Y

    unique_ptr<Tile> tileRenderer;
    unique_ptr<ItemContainer> m_itemContainer;
    unique_ptr<HealthBar> m_healthbar;

    bool m_moveUp    = false;
    bool m_moveDown  = false;
    bool m_moveLeft  = false;
    bool m_moveRight = false;

    void initComponents();
};

#endif //_MAP_EXPLORER_HPP_