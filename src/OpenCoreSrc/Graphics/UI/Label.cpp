#include "Label.hpp"
#include "OpenCore/OpenCore.hpp"
#include <algorithm>
#include <sstream>

Label::Label(const std::string& text) : fullText(text)
{
    // 将基类的AnimeState替换为TextAnimationState
    AnimeState = std::make_unique<TextAnimationState>();
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    
    // 初始化文本状态
    textState.fullText = fullText;
    textState.currentLength = 0;
    textState.charDisplayTime = 0.0f;
    textState.maxLineWidth = 500; // 默认最大行宽
    textState.lineHeight = 30;
    textState.charSpacing = 2;
    textState.textColor = {255, 255, 255, 255};
    
    textDirty = true;
}

void Label::setText(const std::string& text)
{
    fullText = text;
    textDirty = true;
    markCachesDirty();
    
    // 更新动画状态
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    textState.fullText = fullText;
    textState.currentLength = 0;
    textState.charDisplayTime = 0.0f;
}

void Label::appendText(const std::string& text)
{
    fullText += text;
    textDirty = true;
    
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    textState.fullText = fullText;
}

void Label::clearText()
{
    fullText.clear();
    lines.clear();
    textDirty = true;
    
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    textState.fullText.clear();
    textState.currentLength = 0;
}

void Label::setTextColor(const SDL_Color& color)
{
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    textState.textColor = color;
}

void Label::setFont(const std::string& fontName,short fontID)
{
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    textState.fontName = fontName;
    textState.fontID = fontID;
    textDirty = true;
}

void Label::setMaxLineWidth(uint16_t width)
{
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    textState.maxLineWidth = width;
    textDirty = true;
}

void Label::setLineHeight(uint16_t height)
{
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    textState.lineHeight = height;
}

void Label::setCharacterSpacing(uint16_t spacing)
{
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    textState.charSpacing = spacing;
    textDirty = true;
}
void Label::setFont(TTF_Font* font)
{
    if (currentFont == font) {
        return; // 已经是同一个字体，无需更新
    }
    
    currentFont = font;
    
    // 字体改变，标记为需要重新生成纹理
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    textState.dirty = true;
    
    SDL_Log("Label::setFont: Font changed, texture marked as dirty");
}

void Label::setFontById(int fontId)
{
    // 获取字体的unique_ptr
    auto& fontUniquePtr = OpenCoreManagers::ResManager.GetFont(fontId);
    if (fontUniquePtr) {
        setFont(fontUniquePtr.get()); // 获取原始指针
        SDL_Log("Label::setFontById: Successfully set font with ID %d", fontId);
    } else {
        SDL_Log("Label::setFontById: Failed to get font with ID %d", fontId);
        currentFont = nullptr;
    }
}

void Label::onUpdate(float totalTime)
{
    // 先更新文本处理
    if (textDirty) {
        processText();
        textDirty = false;
    }
    
    // 再更新动画
     AnimeManager->onUpdate(totalTime, *AnimeState.get());
}

void Label::onRender()
{
    if (!AnimeState || fullText.empty()){
        SDL_Log("Label::onRender: AnimeState is null or text is empty");
        return;
    }
    
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    
    // 如果没有文字需要显示，直接返回
    if (textState.currentLength == 0){ 
        SDL_Log("Label::onRender: currentLength is 0");
        return;
    }
    
    // 渲染文本行
    renderTextLines();
}

SDL_Rect Label::getBounds()
{
    if (!AnimeState) {
        return SDL_Rect{0, 0, 0, 0};
    }
    
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    const auto& state = *AnimeState;
    
    // 计算基于逻辑坐标系的宽高
    uint16_t logicalWidth = static_cast<uint16_t>(nativeWidth * state.scaleX);
    uint16_t logicalHeight = static_cast<uint16_t>(nativeHeight * state.scaleY);
    
    // 如果没有设置固定大小，则根据文本内容计算
    if (logicalWidth == 0 || logicalHeight == 0) {
        logicalWidth = static_cast<uint16_t>(textState.maxLineWidth*state.scaleX);
        logicalHeight = static_cast<uint16_t>(lines.size() * textState.lineHeight*state.scaleY);
    }
    
    int16_t logicalX = state.PositionX;
    int16_t logicalY = state.PositionY;
    
    // 按锚点偏移位置
    switch (state.Anchor) {
    case AnchorPoint::TopLeft:
        break;
    case AnchorPoint::TopCenter:
        logicalX -= logicalWidth / 2;
        break;
    case AnchorPoint::TopRight:
        logicalX -= logicalWidth;
        break;
    case AnchorPoint::MiddleLeft:
        logicalY -= logicalHeight / 2;
        break;
    case AnchorPoint::Center:
        logicalX -= logicalWidth / 2;
        logicalY -= logicalHeight / 2;
        break;
    case AnchorPoint::MiddleRight:
        logicalX -= logicalWidth;
        logicalY -= logicalHeight / 2;
        break;
    case AnchorPoint::BottomLeft:
        logicalY -= logicalHeight;
        break;
    case AnchorPoint::BottomCenter:
        logicalX -= logicalWidth / 2;
        logicalY -= logicalHeight;
        break;
    case AnchorPoint::BottomRight:
        logicalX -= logicalWidth;
        logicalY -= logicalHeight;
        break;
    }
    
    // 如果是绝对定位，直接返回逻辑坐标
    if (absolutePosite || !parentContainer) {
        return SDL_Rect{logicalX, logicalY, logicalWidth, logicalHeight};
    }
    
    // 相对定位：基于父容器的实际边界进行转换
    SDL_Rect parentBounds = parentContainer->getBounds();
    
    return SDL_Rect{
        parentBounds.x + logicalX,
        parentBounds.y + logicalY,
        logicalWidth,
        logicalHeight
    };
}

SDL_Rect Label::getRenderedBounds()
{
    return OpenCoreManagers::GFXManager.getScale()->ToScreen(getBounds());
}

void Label::processText()
{
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    textState.fullText = fullText;
    
    // 文本清洗和规范化
    cleanText();
    
    // 处理特殊字符
    // 将连续的\r\n、\r、\n统一为\n
    size_t pos = 0;
    while ((pos = textState.fullText.find("\r\n", pos)) != std::string::npos) {
        textState.fullText.replace(pos, 2, "\n");
    }
    while ((pos = textState.fullText.find('\r', pos)) != std::string::npos) {
        textState.fullText.replace(pos, 1, "\n");
    }
    
    // 分割行
    splitLines();
}

void Label::splitLines()
{
    lines.clear();
    
    if (fullText.empty()){ 
        SDL_Log("Label::splitLines: fullText is empty");
        return;
    }
    
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    
    if (!autoWrap) {
        // 不自动换行，使用显式换行符
        size_t start = 0;
        size_t pos = 0;
        while ((pos = textState.fullText.find('\n', start)) != std::string::npos) {
            std::string line = textState.fullText.substr(start, pos - start);
            if (!line.empty()) {
                lines.push_back(line);
            }
            start = pos + 1;
        }
        std::string lastLine = textState.fullText.substr(start);
        if (!lastLine.empty()) {
            lines.push_back(lastLine);
        }
    } else {
        // 自动换行逻辑
        uint16_t maxWidth = textState.maxLineWidth;
        
        if (maxWidth == 0) {
            lines.push_back(textState.fullText);
            return;
        }
        
        std::vector<std::string> words;
        splitIntoWords(textState.fullText, words);
        
        // 预计算单词宽度，避免重复计算
        std::vector<uint16_t> wordWidths;
        wordWidths.reserve(words.size());
        for (const auto& word : words) {
            wordWidths.push_back(calculateTextWidth(word));
        }
        
        std::string currentLine;
        uint16_t currentWidth = 0;
        
        for (size_t i = 0; i < words.size(); ++i) {
            const auto& word = words[i];
            uint16_t wordWidth = wordWidths[i];
            
            // 检查是否能放入当前行（考虑空格）
            bool canFit = currentWidth + wordWidth <= maxWidth;
            
            if (canFit) {
                // 可以放入当前行
                currentLine += word;
                currentWidth += wordWidth;
            } else {
                // 不能放入当前行
                if (!currentLine.empty()) {
                    // 提交当前行
                    lines.push_back(std::move(currentLine));
                    currentLine.clear();
                    currentWidth = 0;
                }
                
                // 处理超长单词
                if (wordWidth > maxWidth) {
                    handleOversizedWord(word, maxWidth, lines);
                } else {
                    // 正常单词，开始新行
                    currentLine = word;
                    currentWidth = wordWidth;
                }
            }
        }
        
        // 处理最后一行
        if (!currentLine.empty()) {
            lines.push_back(std::move(currentLine));
        }
    }
}

void Label::splitIntoWords(const std::string& text, std::vector<std::string>& words) const
{
    words.clear();
    size_t i = 0;
    
    while (i < text.length()) {
        // 跳过连续空格
        if (std::isspace(static_cast<unsigned char>(text[i]))) {
            size_t start = i;
            while (i < text.length() && std::isspace(static_cast<unsigned char>(text[i]))) {
                i++;
            }
            // 将连续空格作为一个"单词"保留，保持原始间距
            if (i > start) {
                words.push_back(text.substr(start, i - start));
            }
            continue;
        }
        
        // 检查字符类型
        bool cjk = isCJKUTF8(text, i);
        
        if (cjk) {
            // CJK字符：每个字符单独成词
            size_t charLen = getUTF8CharLength(text[i]);
            words.push_back(text.substr(i, charLen));
            i += charLen;
        } else {
            // 非CJK字符：按单词分割
            size_t start = i;
            
            // 前进到单词结束（空格或CJK字符）
            while (i < text.length()) {
                if (std::isspace(static_cast<unsigned char>(text[i])) || 
                    isCJKUTF8(text, i)) {
                    break;
                }
                i += getUTF8CharLength(text[i]);
            }
            
            if (i > start) {
                words.push_back(text.substr(start, i - start));
            }
        }
    }
}

std::string Label::splitWordToFit(const std::string& word, uint16_t maxWidth) const
{
    for (size_t i = word.length(); i > 0; i--) {
        std::string segment = word.substr(0, i);
        if (calculateTextWidth(segment) <= maxWidth) {
            if (i < word.length()) {
                // 添加连字符表示单词被分割
                return segment + "-";
            }
            return segment;
        }
    }
    
    // 极端情况：单个字符都超宽，强制分割
    return word.substr(0, 1);
}


static bool Label::isCJKUTF8(const std::string& text, size_t pos){
    if (pos >= text.length()) return false;
    size_t len = getUTF8CharLength(text[pos]);
    if (pos + len > text.length()) return false;    
    // 提取Unicode码点
    uint32_t codepoint = 0;
    switch (len) {
        case 1: codepoint = text[pos] & 0x7F; break;
        case 2: codepoint = ((text[pos] & 0x1F) << 6) | (text[pos+1] & 0x3F); break;
        case 3: codepoint = ((text[pos] & 0x0F) << 12) | ((text[pos+1] & 0x3F) << 6) | (text[pos+2] & 0x3F); break;
        case 4: codepoint = ((text[pos] & 0x07) << 18) | ((text[pos+1] & 0x3F) << 12) | ((text[pos+2] & 0x3F) << 6) | (text[pos+3] & 0x3F); break;
    }
        
    // CJK Unicode范围检测
    return (codepoint >= 0x4E00 && codepoint <= 0x9FFF) ||    // 基本CJK
            (codepoint >= 0x3400 && codepoint <= 0x4DBF) ||    // 扩展A
            (codepoint >= 0x20000 && codepoint <= 0x2A6DF) ||  // 扩展B
            (codepoint >= 0x3040 && codepoint <= 0x309F) ||    // 平假名
            (codepoint >= 0x30A0 && codepoint <= 0x30FF) ||    // 片假名
            (codepoint >= 0xAC00 && codepoint <= 0xD7AF);      // 韩文
}

void Label::renderTextLines()
{
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    SDL_Rect renderRect = getRenderedBounds();
    
    if (lines.empty()){
        SDL_Log("No lines to render.");
        return;
    }
    
    // 检查是否需要更新缓存
    if (textState.currentLength != lastTotalLength || 
        lineCaches.size() != lines.size()) {
        updateLineCaches(textState.currentLength);
        lastTotalLength = textState.currentLength;
    }
    
    // 渲染可见行
    int currentY = renderRect.y;
    for (size_t i = 0; i < lineCaches.size(); i++) {
        LineCache& cache = lineCaches[i];
        
        if (!cache.fullTexture) {
            continue; // 没有纹理可渲染
        }
        
        // 获取纹理尺寸
        int textWidth, textHeight;
        SDL_QueryTexture(cache.fullTexture, NULL, NULL, &textWidth, &textHeight);
        
        // 计算裁剪区域（用于打字机效果）
        SDL_Rect srcRect = {0, 0, textWidth, textHeight};
        
        // 如果该行未完全显示，计算裁剪宽度
        if (cache.lastRenderedLength < cache.text.length()) {
            // 计算部分文本的宽度
            std::string partialText = cache.text.substr(0, cache.lastRenderedLength);
            int partialWidth = calculateTextWidth(partialText);
            srcRect.w = partialWidth;
        }
        
        SDL_Rect destRect = {
            renderRect.x,
            currentY,
            srcRect.w,  // 使用裁剪后的宽度
            textHeight
        };
        
        // 渲染（可能只渲染部分纹理）
        OpenCoreManagers::GFXManager.RenderCopyEx(
            cache.fullTexture, &srcRect, &destRect, 
            AnimeState->angle, nullptr, SDL_FLIP_NONE
        );
        
        currentY += textState.lineHeight;
    }
}
//缓存更新逻辑
void Label::updateLineCaches(size_t currentTotalLength)
{
    // 确保缓存大小与行数匹配
    if (lineCaches.size() != lines.size()) {
        // 清理旧缓存
        for (auto& cache : lineCaches) {
            if (cache.fullTexture) {
                SDL_DestroyTexture(cache.fullTexture);
            }
        }
        lineCaches.clear();
        lineCaches.resize(lines.size());
    }
    
    // 计算每行应显示的字符数
    size_t remainingChars = currentTotalLength;
    
    for (size_t i = 0; i < lines.size(); i++) {
        LineCache& cache = lineCaches[i];
        const std::string& lineText = lines[i];
        
        // 检查是否需要更新缓存
        bool needsUpdate = cache.dirty || 
                          cache.text != lineText || 
                          cache.fullTexture == nullptr;
        
        if (needsUpdate) {
            // 清理旧纹理
            if (cache.fullTexture) {
                SDL_DestroyTexture(cache.fullTexture);
            }
            
            // 创建新纹理
            cache.fullTexture = createTextTexture(lineText);
            cache.text = lineText;
            cache.dirty = false;
        }
        
        // 计算该行当前应显示的字符数
        size_t charsToShow = 0;
        if (remainingChars > 0) {
            charsToShow = std::min(lineText.length(), remainingChars);
            remainingChars -= charsToShow;
        }
        
        cache.lastRenderedLength = charsToShow;
    }
}

void Label::markCachesDirty()
{
    for (auto& cache : lineCaches) {
        cache.dirty = true;
    }
    lastTotalLength = 0; // 强制更新
}

SDL_Texture* Label::createTextTexture(const std::string& text) const
{
    if (text.empty()) return nullptr;
    
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    
    // 检查字体是否可用
    if (!currentFont) {
        SDL_Log("Label::createTextTexture: No font available");
        return nullptr;
    }
    
    SDL_Renderer* renderer = OpenCoreManagers::GFXManager.getRenderer();
    if (!renderer) {
        SDL_Log("Label::createTextTexture: No renderer available");
        return nullptr;
    }
    
    // 设置文本颜色
    SDL_Color textColor = {
        textState.colorR, 
        textState.colorG, 
        textState.colorB, 
        textState.colorA
    };
    
    SDL_Texture* texture = nullptr;
    
    // 使用SDL_ttf渲染文本到表面
    SDL_Surface* textSurface = nullptr;
    
    // 根据文本内容选择合适的渲染函数
    if (containsUTF8(text)) {
        // 包含非ASCII字符，使用UTF-8渲染
        textSurface = TTF_RenderUTF8_Blended(currentFont, text.c_str(), textColor);
    } else {
        // 纯ASCII文本，使用更快的渲染函数
        textSurface = TTF_RenderText_Blended(currentFont, text.c_str(), textColor);
    }
    
    if (!textSurface) {
        SDL_Log("Label::createTextTexture: Failed to create text surface: %s", TTF_GetError());
        return nullptr;
    }
    
    // 从表面创建纹理
    texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    
    if (!texture) {
        SDL_Log("Label::createTextTexture: Failed to create texture from surface: %s", SDL_GetError());
        return nullptr;
    }
    
    // 设置纹理混合模式以实现透明度
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    
    SDL_Log("Label::createTextTexture: Successfully created texture for text: '%s'", text.c_str());
    return texture;
}

bool Label::containsUTF8(const std::string& text) const
{
    for (char c : text) {
        // 如果任何字符的高位被设置，说明是UTF-8字符
        if (static_cast<unsigned char>(c) > 127) {
            return true;
        }
    }
    return false;
}

uint16_t Label::calculateTextWidth(const std::string& text) const
{
    if (text.empty() || !currentFont) {
        return 0;
    }
    
    int width = 0, height = 0;
    
    // 使用TTF直接测量文本尺寸
    if (containsUTF8(text)) {
        TTF_SizeUTF8(currentFont, text.c_str(), &width, &height);
    } else {
        TTF_SizeText(currentFont, text.c_str(), &width, &height);
    }
    
    return static_cast<uint16_t>(width);
}

uint16_t Label::getCharWidth(char c) const
{
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    uint16_t spacing = textState.charSpacing;
    
    // 如果有字体，使用真实测量
    if (currentFont) {
        std::string charStr(1, c);
        int width = 0, height = 0;
        
        if (static_cast<unsigned char>(c) > 127) {
            TTF_SizeUTF8(currentFont, charStr.c_str(), &width, &height);
        } else {
            TTF_SizeText(currentFont, charStr.c_str(), &width, &height);
        }
        
        return static_cast<uint16_t>(width) + spacing;
    }
    
    // 没有字体时的回退估算（保持原有逻辑）
    if (std::isspace(static_cast<unsigned char>(c))) {
        return 3 + spacing;
    }
    else if (isCJKChar(c)) {
        return 12 + spacing;
    }
    else if (std::isalpha(static_cast<unsigned char>(c)) || std::isdigit(static_cast<unsigned char>(c))) {
        return 8 + spacing;
    }
    else {
        return 6 + spacing;
    }
}

void Label::handleOversizedWord(const std::string& word, uint16_t maxWidth, 
                               std::vector<std::string>& lines) const
{
    std::string remaining = word;
    
    while (!remaining.empty()) {
        // 找到能放入的最大前缀
        size_t bestSplit = 0;
        uint16_t bestWidth = 0;
        
        for (size_t splitPos = 1; splitPos <= remaining.length(); ) {
            std::string segment = remaining.substr(0, splitPos);
            uint16_t segmentWidth = calculateTextWidth(segment);
            
            if (segmentWidth <= maxWidth) {
                bestSplit = splitPos;
                bestWidth = segmentWidth;
                
                // 前进到下一个字符边界（UTF-8安全）
                if (splitPos < remaining.length()) {
                    size_t charLen = getUTF8CharLength(remaining[splitPos]);
                    splitPos += charLen;
                } else {
                    splitPos++;
                }
            } else {
                break;
            }
        }
        
        if (bestSplit > 0) {
            std::string segment = remaining.substr(0, bestSplit);
            
            // 如果不是最后一个分段，考虑添加连字符
            if (bestSplit < remaining.length() && 
                !std::isspace(static_cast<unsigned char>(segment.back())) &&
                !isCJKUTF8(segment, segment.length() - 1)) {
                // 只在非CJK单词末尾添加连字符
                segment += "-";
            }
            
            lines.push_back(segment);
            remaining = remaining.substr(bestSplit);
        } else {
            // 极端情况：单个字符都超宽，强制分割第一个字符
            size_t charLen = getUTF8CharLength(remaining[0]);
            lines.push_back(remaining.substr(0, charLen));
            remaining = remaining.substr(charLen);
        }
    }
}

bool Label::isCJKChar(char c) const
{
    // 简单检测：检查字符是否在常见CJK范围内
    unsigned char uc = static_cast<unsigned char>(c);
    
    // 如果字符高位被设置，可能是CJK字符的UTF-8首字节
    if (uc >= 0x80) {
        // 常见CJK UTF-8范围：
        // - 0xE4-0xE9: 常用CJK字符的UTF-8首字节
        // - 其他CJK区块的首字节范围
        return (uc >= 0xE4 && uc <= 0xE9) || 
               (uc >= 0xB0 && uc <= 0xBF); // 一些扩展区块
    }
    
    return false;
}

static size_t Label::getUTF8CharLength(char firstByte) {
    if ((firstByte & 0x80) == 0x00) return 1;      // ASCII
    if ((firstByte & 0xE0) == 0xC0) return 2;      // 2字节
    if ((firstByte & 0xF0) == 0xE0) return 3;      // 3字节
    if ((firstByte & 0xF8) == 0xF0) return 4;      // 4字节
    return 1; // 无效UTF-8，按单字节处理
}

void Label::cleanText()
{
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    
    // 移除不必要的空白字符
    std::string cleaned;
    bool lastWasSpace = false;
    
    for (char c : textState.fullText) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!lastWasSpace) {
                cleaned += ' ';
                lastWasSpace = true;
            }
        } else {
            cleaned += c;
            lastWasSpace = false;
        }
    }
    
    textState.fullText = cleaned;
}

// 动画控制方法
void Label::startTypewriterAnimation(float charsPerSecond)
{
    auto typewriterAnim = std::make_shared<TypewriterAnimation>(fullText, charsPerSecond);
    typewriterAnim->setLoop(false);
    AnimeManager->pushAnimation(typewriterAnim);
}

void Label::pause()
{
    // 遍历所有动画，对TypewriterAnimation调用pause()
    for (auto& anim : AnimeManager.Animations) {
        // 尝试将IAnimation转换为TypewriterAnimation
        auto typewriterAnim = std::dynamic_pointer_cast<TypewriterAnimation>(anim);
        if (typewriterAnim) {
            typewriterAnim->pause();
            SDL_Log("Label::pause: Paused TypewriterAnimation");
        }
        
        // 如果有其他需要暂停的文字动画类型，可以继续添加
        // auto otherTextAnim = std::dynamic_pointer_cast<OtherTextAnimation>(anim);
        // if (otherTextAnim) {
        //     otherTextAnim->pause();
        // }
    }
}

void Label::resume()
{
    // 遍历所有动画，对TypewriterAnimation调用resume()
    for (auto& anim : AnimeManager.Animations) {
        auto typewriterAnim = std::dynamic_pointer_cast<TypewriterAnimation>(anim);
        if (typewriterAnim) {
            typewriterAnim->resume();
            SDL_Log("Label::resume: Resumed TypewriterAnimation");
        }
        
        // 恢复其他文字动画类型
        // auto otherTextAnim = std::dynamic_pointer_cast<OtherTextAnimation>(anim);
        // if (otherTextAnim) {
        //     otherTextAnim->resume();
        // }
    }
}

void Label::skip()
{
    // 遍历所有动画，对TypewriterAnimation调用skip()
    for (auto& anim : AnimeManager.Animations) {
        auto typewriterAnim = std::dynamic_pointer_cast<TypewriterAnimation>(anim);
        if (typewriterAnim) {
            typewriterAnim->skip();
            SDL_Log("Label::skip: Skipped TypewriterAnimation");
        }
        
        // 跳过其他文字动画类型
        // auto otherTextAnim = std::dynamic_pointer_cast<OtherTextAnimation>(anim);
        // if (otherTextAnim) {
        //     otherTextAnim->skip();
        // }
    }
}

bool Label::isTypewriterFinished() const
{
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    return textState.currentLength >= textState.fullText.length();
}

bool Label::onDestroy()
{
    UIElement::onDestroy();
    lines.clear();
    fullText.clear();
    for (auto& cache : lineCaches) {
        if (cache.fullTexture) {
            SDL_DestroyTexture(cache.fullTexture);
        }
    }
    return true;
}

uint16_t Label::getTextWidth(const std::string& text) const
{
    return calculateTextWidth(text);
}

uint16_t Label::getTextHeight() const
{
    TextAnimationState& textState = static_cast<TextAnimationState&>(*AnimeState);
    return static_cast<uint16_t>(lines.size() * textState.lineHeight);
}