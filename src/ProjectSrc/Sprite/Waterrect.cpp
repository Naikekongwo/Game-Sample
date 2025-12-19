#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include <algorithm>


Waterrect::Waterrect(const string &id, short layer, unique_ptr<Texture> texture)
{
    this->id = id;
    this->layer = layer;

    if(!texture)
    {
        SDL_Log("Waterrect() Encountered a empty texture.");
        return;
    }

    this->texture = std::move(texture);

    precomputeLighting();
}

void Waterrect::precomputeLighting()
{
    // 光源方向（视点右前方45度的空中）
    float lx = 0.707f, ly = -0.5f, lz = 0.5f;
    float invLen = 1.0f / sqrt(lx*lx + ly*ly + lz*lz);
    lightX = lx * invLen;
    lightY = ly * invLen;
    lightZ = lz * invLen;
    
    // 预计算半程向量
    float vx = 0.0f, vy = 0.0f, vz = 1.0f;
    float hx = lightX + vx;
    float hy = lightY + vy;
    float hz = lightZ + vz;
    invLen = 1.0f / sqrt(hx*hx + hy*hy + hz*hz);
    halfX = hx * invLen;
    halfY = hy * invLen;
    halfZ = hz * invLen;
}


void Waterrect::onUpdate(float totalTime)
{
    // 使用 std::for_each 和 lambda 表达式
    std::for_each(Vertices.begin(), Vertices.end(),
        [this, totalTime](SDL_Vertex& vertex) {
            FreshVertex(vertex, totalTime);
        }
    );
}

void Waterrect::setWave(unique_ptr<Wave> newWave)
{
    waveInfo = std::move(newWave);
}


void Waterrect::onRender()
{
    const auto& GFXManager = OpenCoreManagers::GFXManager;

    auto dick = GFXManager.getInstance().getRenderEngine();

    auto engine = static_cast<SDL_Adapter*>(dick);

    engine->RenderVertices(this->texture->texture.get(), Vertices.data(), Vertices.size(), indices.data(), indices.size());
}

bool Waterrect::onDestroy()
{
    // 销毁函数

    return true;
}


// 注意！这里是亮度的映射函数
int Waterrect::FreshVertex(SDL_Vertex& vertex, float totalTime)
{
    OpenCore_Rect Border = getRenderedBounds();
    
    // 基础位置计算
    vertex.position.x = vertex.tex_coord.x * Border.w + Border.x;
    vertex.position.y = vertex.tex_coord.y * Border.h + Border.y;
    
    // 计算高度（优化：避免重复计算range）
    float rawHeight = waveInfo->getHeight(vertex.position.x, vertex.position.y, totalTime);
    float range = waveInfo->getRange();  // 可能需要缓存这个值
    
    // 优化：使用预计算的 1.0f/(2.0f*range)，如果range不常变化
    static float lastRange = 0.0f;
    static float invTwoRange = 0.5f;  // 1.0f/(2.0f*range)的缓存
    
    if (range <= 0.0f) range = 1.0f;
    if (std::abs(range - lastRange) > 0.001f) {
        lastRange = range;
        invTwoRange = 1.0f / (2.0f * range);
    }
    
    float Height = (rawHeight + range) * invTwoRange;
    
    // 快速clamp（比std::max/min快）
    if (Height < 0.0f) Height = 0.0f;
    else if (Height > 1.0f) Height = 1.0f;
    
    // 3D变形（如果启用）
    if(specialDraw && rectSets.size() >= 8)
    {
        // 优化：预计算缩放因子
        static float scaleFactor = 0.0f;
        static bool scaleComputed = false;
        
        if (!scaleComputed) {
            scaleFactor = OpenCoreManagers::GFXManager.getScale()->GetScale() * 1920.0f;
            scaleComputed = true;
        }
        
        // 快速计算RenderSets
        float RenderSets[8];
        for(int i = 0; i < 8; i++) {
            RenderSets[i] = rectSets[i] * scaleFactor;
        }
        
        float factor = vertex.tex_coord.y;
        
        // 预计算公共部分
        float diff64 = RenderSets[6] - RenderSets[4] - RenderSets[2] + RenderSets[0];
        
        float startX = diff64 * 0.5f * (1.0f - factor) + RenderSets[4];
        float width = diff64 * factor + (RenderSets[2] - RenderSets[0]);
        
        // 应用3D变形
        vertex.position.x = startX + width * vertex.tex_coord.x;
        vertex.position.y = (RenderSets[5] - RenderSets[1]) * factor + RenderSets[1] 
                          + (waveFactor * Height * (1.0f + factor));
    }

    
    // =========== 光照计算（优化版） ===========
    
    // 1. 获取归一化法线
    auto normal = waveInfo->getNormalizedVector(vertex.position.x, vertex.position.y, totalTime);
    float nx = normal[0];
    float ny = normal[1]; 
    float nz = normal[2];
    
    // 2. 点积计算（使用预计算的向量）
    float NdotL = nx*lightX + ny*lightY + nz*lightZ;
    float diffuse = NdotL > 0.0f ? NdotL : 0.0f;
    
    // 3. 镜面高光
    float NdotH = nx*halfX + ny*halfY + nz*halfZ;
    float specular = 0.0f;
    
    if (NdotH > 0.0f) {
        // 快速幂计算（针对specPower=32优化）
        float NdotH2 = NdotH * NdotH;      // 平方
        float NdotH4 = NdotH2 * NdotH2;    // 四次方
        float NdotH8 = NdotH4 * NdotH4;    // 八次方
        float NdotH16 = NdotH8 * NdotH8;   // 十六次方
        float NdotH32 = NdotH16 * NdotH16; // 三十二次方
        specular = specIntensity * NdotH32;
    }
    
    // 4. 组合光照
    float lighting = ambient + diffuse + specular;
    if (lighting > 1.2f) lighting = 1.2f;
    
    // 5. 高度影响
    float heightEffect = heightBase + heightRange * Height;
    
    // =========== 颜色计算 ===========
    
    // 混合背景色和水本色
    float blendFactor = lighting * 0.0f;  // 0.0-0.6范围
    float oneMinusBlend = 1.0f - blendFactor;
    
    float r = waterR * oneMinusBlend + bgR * blendFactor;
    float g = waterG * oneMinusBlend + bgG * blendFactor;
    float b = waterB * oneMinusBlend + bgB * blendFactor;
    
    // 应用光照和高度效果
    float finalMultiplier = lighting * heightEffect;
    r *= finalMultiplier;
    g *= finalMultiplier;
    b *= finalMultiplier;
    
    // 确保最小值
    if (r < minBrightness) r = minBrightness;
    if (g < minBrightness) g = minBrightness;
    if (b < minBrightness) b = minBrightness;
    
    if (r > 1.0f) r = 1.0f;
    if (g > 1.0f) g = 1.0f;
    if (b > 1.0f) b = 1.0f;
    
    // =========== 顶部透明度渐隐 ===========
    
    // 获取顶点在Waterrect中的相对高度位置（0=底部, 1=顶部）
    float relativeY = vertex.tex_coord.y;
    
    // 透明度计算：非线性渐隐
    float alpha = 1.0f;
    
    if (relativeY < borderFade) {
        float fadeFactor = relativeY / borderFade;
        // 快速S形曲线：t²*(3-2t)
        float t2 = fadeFactor * fadeFactor;
        alpha = t2 * (3.0f - 2.0f * fadeFactor);
    }
    
    // 转换为8-bit颜色值
    vertex.color.r = static_cast<uint8_t>(r * 255.0f);
    vertex.color.g = static_cast<uint8_t>(g * 255.0f);
    vertex.color.b = static_cast<uint8_t>(b * 255.0f);
    vertex.color.a = static_cast<uint8_t>(alpha * 255.0f);

    OpenCore_Rect newRect;

    newRect.x = vertex.position.x;
    newRect.y =vertex.position.y;

    newRect = OpenCoreManagers::GFXManager.getScale()->ToScreen(newRect);

    vertex.position.x = newRect.x;
    vertex.position.y = newRect.y;

    return 0;
}


void Waterrect::setDetail(uint8_t detail) {
    this->detail = detail;
    Vertices.clear();
    indices.clear();
    
    // 1. 创建 detail × detail 个顶点
    for(int row = 0; row < detail; row++) {
        for(int col = 0; col < detail; col++) {
            SDL_Vertex vertex;
            // 纹理坐标：归一化到[0,1]
            // 注意处理detail=1的情况
            if(detail > 1) {
                vertex.tex_coord.x = static_cast<float>(col) / (detail - 1);
                vertex.tex_coord.y = static_cast<float>(row) / (detail - 1);
            } else {
                vertex.tex_coord.x = 0.0f;
                vertex.tex_coord.y = 0.0f;
            }
            vertex.position = {0, 0};
            vertex.color = {255, 255, 255, 255};
            Vertices.push_back(vertex);
        }
    }
    
    // 2. 为每个四边形创建索引（需要detail>=2才有四边形）
    if(detail >= 2) {
        for(int row = 0; row < detail - 1; row++) {
            for(int col = 0; col < detail - 1; col++) {
                // 当前四边形的4个顶点索引
                int topLeft = row * detail + col;
                int topRight = topLeft + 1;
                int bottomLeft = (row + 1) * detail + col;
                int bottomRight = bottomLeft + 1;
                
                // 第一个三角形：左上→右上→左下（顺时针或逆时针，保持一致即可）
                indices.push_back(topLeft);
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                
                // 第二个三角形：右上→右下→左下
                indices.push_back(topRight);
                indices.push_back(bottomRight);
                indices.push_back(bottomLeft);
            }
        }
    }
    // 如果detail=1，没有四边形，顶点也不会被渲染（需要至少3个顶点才能组成三角形）
}