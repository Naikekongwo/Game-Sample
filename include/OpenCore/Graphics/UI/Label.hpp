#ifndef _LABEL_H_
#define _LABEL_H_

// Label.hpp
// 文字显示的控件

#include "OpenCore/Graphics/IDrawableObject/UIElement.hpp"
#include <memory>

using std::string;
using std::unique_ptr;

class Label : public UIElement
{
    public:

    // 构造函数
    Label(const string& id, uint8_t layer, uint8_t fontWeight);

    // 父类的实现

    void handlEvents(SDL_Event &event, float totalTime) override;
    int setBakedTexture(bool isBaked) override {return false; }
    bool preRenderTexture(SDL_Texture* texture) override;

    void onUpdate(float totalTime) override;

    // 文本框特有功能

    void setText(const string& sentence, uint8_t fontWeight);

    private:

    uint8_t fontWeight;
    string sentence;

    uint8_t fontGrid[2];
    std::unique_ptr<Texture> TextBuffer;
};

#endif //_LABEL_H_