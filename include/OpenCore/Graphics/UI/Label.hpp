#ifndef _LABEL_H_
#define _LABEL_H_

#include "UIElement.hpp"
#include "TextAnimation.hpp"
#include "TypewriterAnimation.hpp"
#include <string>
#include <vector>
#include <memory>

class Label : public UIElement
{
public:
    Label(const std::string& text = "");
    virtual ~Label(){
    for (auto& cache : lineCaches) {
        if (cache.fullTexture) {
            SDL_DestroyTexture(cache.fullTexture);
        }
    }
};
    
    // 文字内容操作
    void setText(const std::string& text);
    const std::string& getText() const { return fullText; }
    
    void appendText(const std::string& text);
    void clearText();
    
    // 文字样式设置
    void setTextColor(const SDL_Color& color);
    void setFont(const std::string& fontName,short fontID);
    void setMaxLineWidth(uint16_t width);
    void setLineHeight(uint16_t height);
    void setCharacterSpacing(uint16_t spacing);
    void setFont(TTF_Font* font);
    void setFontById(int fontId);
    TTF_Font* getCurrentFont() const { return currentFont; }

    
    // 动画控制
    void startTypewriterAnimation(float charsPerSecond = 20.0f);
    void pause();
    void resume();
    void skip();
    bool isTypewriterFinished() const;
    
    // 换行控制
    void enableAutoWrap(bool enable) { autoWrap = enable; textDirty = true; }
    bool isAutoWrapEnabled() const { return autoWrap; }
    
    // 重写UIElement方法
    virtual void onUpdate(float totalTime) override;
    virtual void onRender() override;
    virtual SDL_Rect getBounds() override;
    virtual SDL_Rect getRenderedBounds() override;
    virtual bool onDestroy() override;
    
    // 文本测量
    uint16_t getTextWidth(const std::string& text) const;
    uint16_t getTextHeight() const;

private:
    std::string fullText;                          // 完整文本
    std::vector<std::string> lines;                // 分行文本
    bool autoWrap = true;                           // 自动换行
    bool textDirty = true;                          // 文本脏标记
    TTF_Font* currentFont = nullptr;                // 当前字体
    struct LineCache {
        SDL_Texture* fullTexture = nullptr;    // 整行完整纹理
        std::string text;                       // 缓存的文本内容
        int lastRenderedLength = -1;           // 上次渲染的字符长度
        bool dirty = true;                      // 脏标记
    };
    
    std::vector<LineCache> lineCaches;          // 每行的缓存
    size_t lastTotalLength = 0;                 // 上次的总字符长度
    
    // 文本处理
    void processText();
    void splitLines();
    void splitIntoWords(const std::string& text, std::vector<std::string>& words) const;
    std::string splitWordToFit(const std::string& word, uint16_t maxWidth) const;
    static size_t getUTF8CharLength(char firstByte);
    static bool isCJKUTF8(const std::string& text, size_t pos);
    void handleOversizedWord(const std::string& word, uint16_t maxWidth, std::vector<std::string>& lines) const;
    bool containsUTF8(const std::string& text) const;
    
    // 渲染相关
    SDL_Texture* createTextTexture(const std::string& text) const;
    void renderTextLines();
    uint16_t calculateTextWidth(const std::string& text) const;
    uint16_t getCharWidth(char c) const;
    void updateLineCaches(size_t currentTotalLength);
    void markCachesDirty();
    
    // 工具函数
    //bool isCJKChar(char32_t c) const;
    void cleanText();
};

#endif // _LABEL_H_