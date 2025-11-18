#ifndef _TYPEWRITERANIMATION_H_
#define _TYPEWRITERANIMATION_H_

#include "TextAnimation.hpp"
#include <unordered_map>

class TypewriterAnimation : public TextAnimation
{
public:
    TypewriterAnimation(const std::string& text = "", float charsPerSecond = 20.0f);
    virtual ~TypewriterAnimation() = default;

    // 回调函数类型
    using AnimationEndCallback = std::function<void()>;
    using CharacterDisplayedCallback = std::function<void(char, int, int)>;
    
    // 设置回调函数
    void setOnAnimationEnd(AnimationEndCallback callback);
    void setOnCharacterDisplayed(CharacterDisplayedCallback callback);
    
    // 清除回调函数
    void clearCallbacks();

    // 设置打字速度（字符/秒）
    void setSpeed(float charsPerSecond) { this->charsPerSecond = charsPerSecond; }
    float getSpeed() const { return charsPerSecond; }
    
    // 动画控制
    void pause() { paused = true; }
    void resume() { paused = false; }
    bool isPaused() const { return paused; }
    
    // 跳过动画，立即显示全部文字
    void skip() { skipRequested = true; }
    
    // 重写基类方法
    virtual void onUpdate(float totalTime, AnimationState& state) override;
    virtual void reset(float totalTime, AnimationState& state) override;
    virtual bool isFinished() const override { return finished; }
    
    // 设置特殊字符停顿（如标点符号延长显示时间）
    void setPunctuationDelay(char punctuation, float delaySeconds);
    void clearPunctuationDelays() { punctuationDelays.clear(); }
    
    // 文本处理（重写纯虚函数）
    virtual void processText(TextAnimationState& state) override;

    // 设置音效ID
    void setTypeSoundId(int id) { typeSoundId = static_cast<uint16_t>(id); }
    void enableSound(bool enable) { soundEnabled = enable; }

private:
    float charsPerSecond;          // 打字速度（字符/秒）
    float elapsedTime = 0.0f;      // 累计时间
    bool paused = false;           // 暂停状态
    bool skipRequested = false;    // 跳过请求

    // 音效相关成员
    uint16_t typeSoundId = static_cast<uint16_t>(1); // 默认打字音效ID
    bool soundEnabled = true; // 是否启用音效
    
    std::unordered_map<char, float> punctuationDelays; // 标点符号延迟映射
    
    // 计算字符显示时间（考虑标点停顿）
    float getCharDisplayTime(char c) const;
    
    // 播放打字音效
    virtual void playTypeSound() const ;

    // 新增回调成员
    AnimationEndCallback onAnimationEnd = nullptr;
    CharacterDisplayedCallback onCharacterDisplayed = nullptr;
};

#endif // _TYPEWRITERANIMATION_H_