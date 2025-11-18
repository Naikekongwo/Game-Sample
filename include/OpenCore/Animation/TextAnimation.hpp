// TextAnimation.hpp
#ifndef _TEXTANIMATION_H_
#define _TEXTANIMATION_H_

#include "IAnimation.hpp"
#include <functional>
#include <string>
#include <vector>

struct TextAnimationState : public AnimationState
{
    std::string fullText;           // 完整文本内容
    size_t currentLength = 0;       // 当前显示的文字长度
    float charDisplayTime = 0.0f;   // 每个字符显示的时间累加
    bool soundEnabled = true;       // 是否启用打字音效
    
    // 文本格式控制
    uint16_t maxLineWidth = 0;      // 最大行宽（自动换行）
    uint16_t lineHeight = 30;       // 行高
    uint16_t charSpacing = 2;       // 字符间距
    
    // 文字样式
    SDL_Color textColor = {255, 255, 255, 255}; // 文字颜色
    std::string fontName = "default"; // 字体名称
    short fontID = -1; // 字体ID
    
    TextAnimationState() = default;
    virtual ~TextAnimationState() = default;
};

class TextAnimation : public IAnimation
{
public:
    TextAnimation(const std::string& text = "");
    virtual ~TextAnimation() = default;
    
    // 设置/获取文本内容
    void setText(const std::string& text);
    const std::string& getText() const { return targetText; }
    
    // 文本样式设置
    void setTextColor(const SDL_Color& color);
    void setFont(const std::string& fontName);
    void setMaxLineWidth(uint16_t width);
    
    // 音效控制
    void enableSound(bool enable) { soundEnabled = enable; }
    
    // 重写基类方法 - 将processText声明为纯虚函数
    virtual void onUpdate(float totalTime, AnimationState& state) override;
    virtual bool isFinished() const override { return finished; }
    virtual void reset(float totalTime, AnimationState& state) override = 0; // 改为纯虚函数
    virtual bool isLoop() override { return loop; }
    
    void setLoop(bool loop) { this->loop = loop; }

protected:
    std::string targetText;
    bool finished = false;
    bool loop = false;
    bool soundEnabled = true;
    
    // 音效回调（可由具体实现类重写）
    virtual void playTypeSound() const = 0; // 改为纯虚函数
    
    // 文本处理 - 声明为纯虚函数，强制派生类实现
    virtual void processText(TextAnimationState& state) = 0; // 纯虚函数
    
    // 添加一个保护型的工具函数，供派生类使用
    void updateTextState(TextAnimationState& state, size_t newLength);
};

#endif // _TEXTANIMATION_H_