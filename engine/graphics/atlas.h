#ifndef _ATLAS_H
#define _ATLAS_H

// atlas.h 纹理图集类

#include <vector>
#include <SDL2/SDL.h>
#include "modules.h"

class AtlasElement
{
    public:
    AtlasElement(SDL_Texture* texture)
    {
        this->texture = texture;
        SDL_QueryTexture(texture, NULL, NULL, &width, &height); // 获取纹理的宽高
    }

    ~AtlasElement()
    {
        if(texture) SDL_DestroyTexture(texture); // 销毁纹理
    }

    SDL_Texture* getTexture() {return texture;} // 获取纹理

    private:
    SDL_Texture* texture; // 纹理
    int width; // 宽度
    int height; // 高度
};

class Atlas
{
    public:
    Atlas() = default;
    ~Atlas()
    {
        for(auto element : elements) // 遍历纹理图集元素列表
        {
            element->~AtlasElement(); // 删除元素
            delete element; // 释放内存
        }
    }

    void pushElement(AtlasElement* element) {elements.push_back(element);} // 添加元素
    
    AtlasElement* getElement(int index) {return elements.at(index);} // 获取元素

    private:
    std::vector<AtlasElement*> elements; // 纹理图集元素列表

};


#endif // _ATLAS_H