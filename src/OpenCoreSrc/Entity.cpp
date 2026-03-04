#include "OpenCore/OpenCore.hpp"
#include <memory>

void Entity::createRenderer()
{
    // 创建一个角色绘制器
    if (!renderer)
    {
        renderer = std::make_unique<Mob>()
    }
}

void Entity::Draw()
{
    if (drawable)
    {
        if (!renderer)
        {
            // 创建renderer
            createRenderer();
        }
    }
    else if (renderer)
    {
        // 既然不需要渲染，那就不要让renderer占有内存
        renderer.reset();
    }
}