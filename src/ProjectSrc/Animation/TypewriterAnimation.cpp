#include "OpenCore/OpenCore.hpp"
#include "TypewriterAnimation.hpp"
#include <algorithm>
#include <cmath>

TypewriterAnimation::TypewriterAnimation(const std::string& text, float charsPerSecond)
    : TextAnimation(text), charsPerSecond(charsPerSecond)
{
    // 设置常用标点符号的默认延迟时间
    setPunctuationDelay('.', 0.3f);
    setPunctuationDelay('。', 0.3f);
    setPunctuationDelay('!', 0.2f);
    setPunctuationDelay('！', 0.2f);
    setPunctuationDelay('?', 0.2f);
    setPunctuationDelay('？', 0.2f);
    setPunctuationDelay(',', 0.1f);
    setPunctuationDelay('，', 0.1f);
    setPunctuationDelay(';', 0.15f);
    setPunctuationDelay('；', 0.15f);
    setPunctuationDelay(':', 0.15f);
    setPunctuationDelay('：', 0.15f);
}

// 设置动画结束回调
void TypewriterAnimation::setOnAnimationEnd(AnimationEndCallback callback)
{
    onAnimationEnd = callback;
}

// 设置字符显示回调
void TypewriterAnimation::setOnCharacterDisplayed(CharacterDisplayedCallback callback)
{
    onCharacterDisplayed = callback;
}

// 清除所有回调
void TypewriterAnimation::clearCallbacks()
{
    onAnimationEnd = nullptr;
    onCharacterDisplayed = nullptr;
}

void TypewriterAnimation::onUpdate(float totalTime, AnimationState& state)
{
    // 将基类状态转换为文本动画状态
    TextAnimationState& textState = static_cast<TextAnimationState&>(state);
    
    // 处理跳过请求
    if (skipRequested) {
        textState.currentLength = textState.fullText.length();
        finished = true;
        skipRequested = false;
        
        // 触发完成回调（如果有）
        if (onAnimationEnd) {
            onAnimationEnd();
        }
        return;
    }
    
    // 如果动画已完成或暂停，直接返回
    if (finished || paused) {
        return;
    }
    
    // 初始化elapsedTime（第一次调用时）
    if (elapsedTime == 0.0f) {
        elapsedTime = totalTime;
        
        // 动画开始时，如果有字符显示回调，触发第一个字符（如果立即显示）
        if (charsPerSecond > 0 && textState.currentLength == 0 && onCharacterDisplayed) {
            if (textState.fullText.length() > 0) {
                onCharacterDisplayed(textState.fullText[0], 0, textState.fullText.length());
            }
        }
    }
    
    // 计算时间增量
    float deltaTime = totalTime - elapsedTime;
    elapsedTime = totalTime;
    
    // 如果打字速度为0或负数，立即完成
    if (charsPerSecond <= 0.0f) {
        textState.currentLength = textState.fullText.length();
        finished = true;
        
        // 触发完成回调
        if (onAnimationEnd) {
            onAnimationEnd();
        }
        return;
    }
    
    // 基础时间计算：每个字符的标准显示时间
    float baseTimePerChar = 1.0f / charsPerSecond;
    
    // 累积时间用于字符显示
    static float accumulatedTime = 0.0f;
    accumulatedTime += deltaTime;
    
    // 处理字符显示
    while (accumulatedTime >= baseTimePerChar && textState.currentLength < textState.fullText.length()) {
        // 获取当前要显示的字符
        char currentChar = textState.fullText[textState.currentLength];
        
        // 计算当前字符的实际显示时间（考虑标点延迟）
        float actualTimePerChar = getCharDisplayTime(currentChar);
        
        // 如果累积时间足够显示这个字符
        if (accumulatedTime >= actualTimePerChar) {
            // 增加显示长度
            textState.currentLength++;
            accumulatedTime -= actualTimePerChar;
            
            // 播放打字音效
            if (soundEnabled) {
                playTypeSound();
            }
            
            // 触发字符显示回调（如果有）
            if (onCharacterDisplayed) {
                onCharacterDisplayed(currentChar, textState.currentLength, textState.fullText.length());
            }
        } else {
            // 时间不够显示这个字符，等待下一帧
            break;
        }
    }
    
    // 检查动画是否完成
    if (textState.currentLength >= textState.fullText.length() && !finished) {
        finished = true;
        
        // 触发完成回调
        if (onAnimationEnd) {
            onAnimationEnd();
        }
    }
}

void TypewriterAnimation::reset(float totalTime, AnimationState& state)
{
    TextAnimationState& textState = static_cast<TextAnimationState&>(state);
    
    // 重置动画状态
    textState.currentLength = 0;
    textState.charDisplayTime = 0.0f;
    elapsedTime = totalTime;
    finished = false;
    paused = false;
    skipRequested = false;
    
    // 重置文本处理状态
    processText(textState);
}

void TypewriterAnimation::setPunctuationDelay(char punctuation, float delaySeconds)
{
    if (delaySeconds > 0) {
        punctuationDelays[punctuation] = delaySeconds;
    } else {
        // 如果延迟时间为0或负数，从映射中移除该字符
        punctuationDelays.erase(punctuation);
    }
}

float TypewriterAnimation::getCharDisplayTime(char c) const
{
    // 基础时间（每个字符的标准显示时间）
    float baseTime = 1.0f / charsPerSecond;
    
    // 检查是否为标点符号，如果有特殊延迟则使用延迟时间
    auto it = punctuationDelays.find(c);
    if (it != punctuationDelays.end()) {
        return it->second; // 返回标点符号的特殊延迟时间
    }
    
    // 检查是否为空白字符（空格、制表符等），可以稍微加快速度
    if (std::isspace(static_cast<unsigned char>(c))) {
        return baseTime * 0.5f; // 空白字符显示快一些
    }
    
    // 默认返回基础时间
    return baseTime;
}

void TypewriterAnimation::processText(TextAnimationState& state)
{
    // 对于打字机动画，这里可以处理文本的样式和格式
    // 例如：根据当前显示长度应用不同的颜色或样式
    
    // 基础实现：确保文本状态正确
    if (state.currentLength > state.fullText.length()) {
        state.currentLength = state.fullText.length();
    }
    
    // 可以在这里添加富文本处理逻辑
    // 例如：为已显示部分和未显示部分设置不同的样式
    
    // 调用基类的文本处理（如果有公共逻辑）
    // TextAnimation::processText(state);
}

void TypewriterAnimation::playTypeSound() const {
        if (!soundEnabled) return;
        
        int soundId = typeSoundId; // 默认使用普通打字音效
        // 通过音效管理器播放
        OpenCoreManagers::SFXManager.getInstance().playSE(soundId);
    }