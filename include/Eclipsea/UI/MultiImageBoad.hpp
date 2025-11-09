#ifndef _MULTI_IMAGEBOARD_H_
#define _MULTI_IMAGEBOARD_H_

// MultiImageBoard.hpp
// 多层图片的基类

#include "OpenCore/Graphics/IDrawableObject/UIElement.hpp"


#include <memory>
#include <vector>
#include <string>

using std::vector;
using std::unique_ptr;


class MultiImageBoard : public UIElement
{
    public:

    MultiImageBoard(const std::string& id, uint8_t layer, uint8_t size);

    // 多层级ImageBoard不存在实时渲染的问题，所以可以直接忽略
    bool preRenderTexture(SDL_Texture* texture) override;
    bool onDestroy() override;

    // 更新算法
    void onUpdate(float totalTime) override;

    // 配置贴图算法
    bool pushImageBoard(vector<unique_ptr<Texture>> list);
    
    // 配置算法
    MyAnimationPipeline AnimateAt();
    DrawableConfigurator ConfigureAt();
    
    private:
    bool async = true;
    bool ready = false;
    size_t size = 0;
    vector<unique_ptr<ImageBoard>> Layers; 
};


#endif //_MULTI_IMAGEBOARD_H_