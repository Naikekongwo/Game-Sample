#pragma once

// Scrollbar.hpp
// 滑动调节控件

#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"
#include <memory>

using std::make_shared;
using std::shared_ptr;
using std::string;

enum class ScrollStatus
{
    Creating,
    Ready,
    Following,
};

class ImageBoard;
class BaseBackground;

class Scrollbar : public UIElement
{
  public:
    Scrollbar(const string &id, short layer, short backTexID, short buttTexID);

    void handlEvents(SDL_Event &event, float totalTime) override;
    void onUpdate(float totalTime) override;
    void onRender() override;
    bool onDestroy() override;
    void onEnter() override;
    void onExit() override;

    // 获取子部件
    ImageBoard *getSlideBar() const { return slideBar.get(); }
    BaseBackground *getBase() const { return baseBack.get(); }

    // 绑定数值
    void bindVariable(shared_ptr<float> value) { this->value = value; }

    // 更新bar
    void UpdateBar();

  private:
    short backgroundTexture = 2009;
    short buttonTexture = 2026;
    unique_ptr<ImageBoard> slideBar;
    unique_ptr<BaseBackground> baseBack;
    shared_ptr<float> value;
    ScrollStatus status = ScrollStatus::Creating;
};