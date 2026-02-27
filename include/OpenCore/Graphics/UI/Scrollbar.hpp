#pragma once

// Scrollbar.hpp
// 滑动调节控件

#include "OpenCore/Graphics/IDrawableObject/UIElement.hpp"
#include <memory>

using std::shared_ptr;
using std::string;

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

  private:
    shared_ptr<float> value;
};