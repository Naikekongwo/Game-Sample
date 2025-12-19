#ifndef _IRENDERER_HPP_
#define _IRENDERER_HPP_

#include <csetjmp>
#include <cstdint>
#include <string>
#include <memory>
#include <vector>

using std::string;
using std::vector;
using std::shared_ptr;

class ResourceManager;

enum class rendererType { SDL, Vulkan};

namespace OpenCore_Renderer
{

    struct OpenCore_Rect
    {
        int32_t x;
        int32_t y;
        int32_t w;
        int32_t h;
    };

    struct OpenCore_Point
    {
        int32_t x;
        int32_t y;
    };

    struct Window_Info
    {  
        string WindowTitle;
        bool Window_Resizable;

        uint32_t WindowGeometry[2];
    };

    struct Tilemap_Info
    {
        double angle;
        OpenCore_Point* center;
        int Flip_Mode;
    };

    class OpenCore_Tilemap
    {
        public:

        virtual int Destroy() = 0;

        virtual bool QueryTile(int &w, int &h) = 0;

        void setAlpha(uint8_t Alpha)
        {
            this->Alpha = Alpha;
        }

        rendererType rType;

        uint8_t Alpha = 255;
        
    };

    struct Tilemap_Deleter
    {
        void operator()(OpenCore_Tilemap* texture) const
        {
            texture->Destroy();
        }
    };

    inline bool OpenCore_PointInRect(OpenCore_Point &Point, OpenCore_Rect &Rect)
    {
        if(Rect.x <= Point.x and Point.x <= Rect.x+ Rect.h)
        {
            if(Rect.y <= Point.y and Point.y <= Rect.y + Rect.h)
            {
                return true;
            }
        }
        else {
            return false;
        }
        return false;
    }

    using TexturePtr = std::unique_ptr<OpenCore_Tilemap, Tilemap_Deleter>;
};

using namespace::OpenCore_Renderer;

class IRenderer
{
    public:

    virtual ~IRenderer() = default;

    virtual bool InitRenderer(Window_Info &Info) = 0;

    virtual int RenderTilemap(OpenCore_Tilemap &Tile, OpenCore_Rect *srcRect, OpenCore_Rect *dstRect, Tilemap_Info Info) = 0;

    virtual void RendererCleanUp() = 0;

    virtual OpenCore_Tilemap* createTexture(uint16_t w, uint16_t h) = 0;

    virtual int setOffScreenRenderer(OpenCore_Tilemap *tileMap) = 0;

    virtual void LoadTile(short id, const string& path) = 0;

    virtual void SyncFunction() = 0;

    rendererType rType;
};

#endif //_IRENDERER_HPP_