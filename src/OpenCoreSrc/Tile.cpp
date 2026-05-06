#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Graphics/IDrawableObject/Texture.hpp"
#include <memory>

Tile::Tile()
{
    this->id = "Tile-Regular";
    this->layer = 0;
    this->VState = std::make_unique<VisualState>();
    this->AnimeManager = std::make_unique<AnimationManager>();

    texture.reset();
}

void Tile::onEnter()
{
    // 初始化纹理库
    // TerrainTiles = std::make_unique<Texture>(
    //     16, 16, OpenCoreManagers::ResManager.GetTexture(2035));

    TerrainTiles = std::make_unique<Texture>(
        8, 8, OpenCoreManagers::ResManager.GetTexture(2044));
}

void Tile::Draw()
{
    auto info = OpenEngine::getInstance().getGameInfo();

    Rect dstRect = getLogicalBounds();
    Rect windowRect{0, 0, static_cast<float>(info->TargetResolutionWidth),
                    static_cast<float>(info->TargetResolutionHeight)};

    if (visible(dstRect, windowRect) && VState->getAlpha() > 0.0f)
    {
        auto GFX = OpenCoreManagers::GFXManager.getInstance();
        // 可见才绘制
        switch (type)
        {
        case TileType::Terrain:
        {
            Rect srcRect = TerrainTiles->getSubRect(TileID);
            GFX.Draw(TerrainTiles->get(), &srcRect, &dstRect, 0.0f, nullptr);
            break;
        }
        default:
            break;
        }
    }
}

bool Tile::onDestroy() { return true; }