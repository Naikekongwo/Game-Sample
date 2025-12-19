#ifndef _WATERECT_HPP_
#define _WATERECT_HPP_

// Waterrect
// 水矩形的精灵类

#include "OpenCore/Graphics/IDrawableObject/Sprite.hpp"

#include <array>
#include <memory>
#include <vector>
#include <string>

using std::unique_ptr;
using std::string;
using std::vector;

class OpenCore_Wave;
class SDL_Vertex;

using Wave = OpenCore_Wave;

struct Texture;

using OpenCore_Renderer::OpenCore_Rect;

class Waterrect : public Sprite
{
public:
    Waterrect(const string &id, short layer, unique_ptr<Texture> texture);
    
    void onUpdate(float totalTime) override;
    void onRender() override;
    bool onDestroy() override;

    void setWave(unique_ptr<Wave> newWave);

    void setDetail(uint8_t detail);
    int FreshVertex(SDL_Vertex& vertex, float totalTime);
    
    void SpecialDraw(bool special, vector<float> &rSets)
    {
        if(rSets.size() != 8)
        {
            return;
        }
        else {
            if(special)
            {
                specialDraw = special;
                rectSets = rSets;
            }
        }
    }
    
private:
    // 预计算光照参数
    void precomputeLighting();
    
    
    // 波形的储存类
    unique_ptr<Wave> waveInfo;

    uint8_t detail = 5;

    vector<SDL_Vertex> Vertices;
    vector<int> indices;

    // 四个绘制角的屏幕相对坐标
    vector<float> rectSets;
    bool specialDraw = false;
    float waveFactor = 32.0f;
    
    // =========== 预计算的常量 ===========
    // 光照向量（归一化）
    float lightX = 0.0f, lightY = 0.0f, lightZ = 0.0f;
    float halfX = 0.0f, halfY = 0.0f, halfZ = 0.0f;  // 半程向量
    
    // 光照参数
    float ambient = 0.4f;
    float specIntensity = 0.8f;
    float specPower = 32.0f;
    
    // 颜色常量（归一化到0-1）
    float bgR = 6.0f / 255.0f;
    float bgG = 111.0f / 255.0f;
    float bgB = 162.0f / 255.0f;
    
    float waterR = 0.2f;
    float waterG = 0.4f;
    float waterB = 0.8f;
    
    // 高度和透明度参数
    float heightBase = 0.6f;
    float heightRange = 0.4f;
    float minBrightness = 0.1f;
    float foamThreshold = 0.7f;
    float foamRange = 0.3f;
    float borderFade = 0.10f;
};

#endif //_WATERECT_HPP_