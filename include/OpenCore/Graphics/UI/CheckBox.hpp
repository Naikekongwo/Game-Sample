#pragma once

// CheckBox.hpp
// 勾选框的类

#include "OpenCore/Graphics/IDrawableObject/UIElement.hpp"
#include "OpenCore/Graphics/UI/ImageBoard.hpp"
#include <memory>

using std::shared_ptr;
using std::string;

enum class CheckBoxStatus
{
    Creating,
    Ready,
    Checking
};

class CheckBox : public UIElement
{
  public:
    CheckBox(const string &id, short layer, unique_ptr<Texture> texture);

    void handlEvents(SDL_Event &event, float totalTime) override;
    // void onUpdate(float totalTime) override;
    void onRender() override;
    bool onDestroy() override;
    void onEnter() override;
    void onExit() override;

    // Function

    void bindVariable(shared_ptr<bool> value) { Value = value; }
    void refreshStatus();

  private:
    unique_ptr<ImageBoard> Img;
    shared_ptr<bool> Value;
    CheckBoxStatus status = CheckBoxStatus::Creating;
};