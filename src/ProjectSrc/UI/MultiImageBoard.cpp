#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"

bool MultiImageBoard::onDestroy()
{
    auto result = UIElement::onDestroy();
    // 先执行父类的方法
    Layers.clear();

    return result;
}

void MultiImageBoard::onUpdate(float totalTime)
{
    if(async == true)
    {
        for( auto& entry : Layers)
        {
            // 遍历vector
            entry.get()->onUpdate(totalTime);
        }
    }
}

bool MultiImageBoard::preRenderTexture(SDL_Texture* texture)
{
    // 多重图片框并不支持离屏渲染
    for(int i = 0; i < Layers.size() ; i++)
    {
        Layers[i].get()->onRender();
    }
    
    return true;
}

MultiImageBoard::MultiImageBoard(const std::string& id, uint8_t layer, uint8_t size)
{
    this->id = id;
    this->layer = layer;

    this->directRender = false;

    this->size = static_cast<size_t>(size);
}

