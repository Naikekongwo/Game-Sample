// TextAnimation.cpp
#include "TextAnimation.hpp"

TextAnimation::TextAnimation(const std::string& text) : targetText(text)
{
}

void TextAnimation::setText(const std::string& text)
{
    targetText = text;
    finished = false;
}

void TextAnimation::onUpdate(float totalTime, AnimationState& state)
{
    // 将基类状态转换为文本动画状态
    TextAnimationState& textState = static_cast<TextAnimationState&>(state);
    
    // 调用纯虚函数，由派生类实现具体逻辑
    processText(textState);
}

void TextAnimation::updateTextState(TextAnimationState& state, size_t newLength)
{
    // 工具函数实现
    state.currentLength = std::min(newLength, targetText.length());
    finished = (state.currentLength >= targetText.length());
}

// 注意：纯虚函数reset、playTypeSound、processText不在基类中实现
// 它们必须在派生类中重写