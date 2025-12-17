#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <memory>



void Label::handlEvents(SDL_Event& event, float totalTime)
{
    // 处理事件的部分
    return;
}

void Label::onUpdate(float totalTime)
{
    // 处理文字效果
    AnimeManager->onUpdate(totalTime, *AnimeState.get());
}


Label::Label(const string& id, uint8_t layer, uint8_t fontWeight)
{
    this->layer = layer;
    this->id = id;
    this->texture = nullptr;

    this->AnimeManager = std::make_unique<AnimationManager>();
    this->AnimeState = std::make_unique<AnimationState>();

    this->fontWeight = fontWeight;

    this->directRender = false;
}

bool Label::preRenderTexture(SDL_Texture* texture)
{
    auto& ResMgr = OpenCoreManagers::ResManager;
    auto& GFXMgr = OpenCoreManagers::GFXManager;

    TTF_Font* font = ResMgr.GetFont(9001);
    if (!font || !TextBuffer) return false;

    SDL_Rect rect = getBounds();

    // 这里传入的必定是nullptr，即以屏幕空间进行渲染
    if (texture != nullptr) {
        return false;
    }

    // 计算字符尺寸
    int charWidth, charHeight;
    TTF_SizeText(font, "A", &charWidth, &charHeight);

    if (fontGrid[0] == 0 || fontGrid[1] == 0) return false;

    // 渲染到当前帧索引的所有字符
    for (int i = 0; i <= AnimeState->frameIndex; i++) {
        int currentRow = i / fontGrid[0];  // 使用 i 而不是 frameIndex
        int currentCol = i % fontGrid[0];
        
        // 检查是否超出文本范围
        if (i >= static_cast<int>(sentence.length())) {
            break;
        }

        SDL_Rect dstRect = {
            rect.x + currentCol * charWidth, 
            rect.y + currentRow * charHeight, 
            charWidth, 
            charHeight
        };

        // 渲染单个字符
        char charStr[2] = {sentence[i], '\0'};
        SDL_Color textColor = {255, 255, 255, 255};

        SDL_Surface* textSurface = TTF_RenderText_Blended(font, charStr, textColor);
        if (textSurface) {
            SDL_Texture* charTexture = SDL_CreateTextureFromSurface(GFXMgr.getRenderer(), textSurface);
            if (charTexture) {
                GFXMgr.RenderCopy(charTexture, NULL, &dstRect, 0, NULL, SDL_FLIP_NONE);
                SDL_DestroyTexture(charTexture);  // 销毁临时纹理
            }
            SDL_FreeSurface(textSurface);
        }
    }
    return true;
}

void Label::setText(const string& sentence, uint8_t fontWeight)
{
    this->fontWeight = fontWeight;
    this->sentence = sentence;  // 保存文本

    auto& ResMgr = OpenCoreManagers::ResManager;
    auto& GFXMgr = OpenCoreManagers::GFXManager;

    TTF_Font* font = ResMgr.GetFont(9001);
    if (!font) return;

    SDL_Rect rect = getBounds();

    // 计算字符尺寸
    int charWidth, charHeight;
    TTF_SizeText(font, "A", &charWidth, &charHeight);

    fontGrid[0] = rect.w / charWidth;
    fontGrid[1] = rect.h / charHeight;

    // 创建 SDL_Texture
    SDL_Texture* rawTexture = GFXMgr.createTexture(rect.w, rect.h);
    if (!rawTexture) return;

    // 包装成 shared_ptr
    auto sdlTexturePtr = std::shared_ptr<SDL_Texture>(
        rawTexture, 
        [](SDL_Texture* texture) { 
            if (texture) SDL_DestroyTexture(texture); 
        }
    );

    // 初始化 TextBuffer
    TextBuffer.reset(new Texture(fontGrid[0], fontGrid[1], sdlTexturePtr));
    if (!TextBuffer) return;

    // 设置离屏渲染
    GFXMgr.setOffScreenRender(TextBuffer->texture.get());

    // 清除背景
    SDL_SetRenderDrawColor(GFXMgr.getRenderer(), 0, 0, 0, 0);
    SDL_RenderClear(GFXMgr.getRenderer());

    // 渲染所有文字到纹理
    int totalChars = sentence.length();
    for (int i = 0; i < fontGrid[1]; i++) {
        for (int j = 0; j < fontGrid[0]; j++) {
            int charIndex = i * fontGrid[0] + j;
            if (charIndex >= totalChars) break;

            SDL_Color textColor = {255, 255, 255, 255};
            char charStr[2] = {sentence[charIndex], '\0'};  // 修复：创建以null结尾的字符串

            SDL_Surface* textSurface = TTF_RenderText_Blended(font, charStr, textColor);
            if (textSurface) {
                SDL_Texture* charTexture = SDL_CreateTextureFromSurface(GFXMgr.getRenderer(), textSurface);
                if (charTexture) {
                    SDL_Rect dstRect = {j * charWidth, i * charHeight, charWidth, charHeight};
                    GFXMgr.RenderCopy(charTexture, NULL, &dstRect, 0, NULL, SDL_FLIP_NONE);
                    SDL_DestroyTexture(charTexture);  // 销毁临时纹理
                }
                SDL_FreeSurface(textSurface);
            }
        }
    }

    GFXMgr.setOffScreenRender(nullptr);
    
    // 设置动画总帧数为总字符数
    AnimeState->frameIndex = 0;  // 从0开始
    AnimeState->frameCapacity = totalChars;
}


