
#include <SDL2/SDL.h>
#include <vector>
#include <cmath>
#include <array>
#include <algorithm>
#include <iostream>

// 模块选择宏定义
// 取消注释你想要运行的模块，确保只有一个模块被取消注释
//#define RUN_ORIGINAL_MAIN    // 运行原始水波模拟
#define RUN_MODULAR_MAIN     // 运行模块化版本

// 如果未定义任何模块，默认运行原始版本
#if !defined(RUN_ORIGINAL_MAIN) && !defined(RUN_MODULAR_MAIN)
#define RUN_ORIGINAL_MAIN
#endif

// 条件编译：确保只有一个main函数被编译
#if defined(RUN_ORIGINAL_MAIN) && defined(RUN_MODULAR_MAIN)
#error "只能定义一个运行模块！请确保只有一个RUN_*宏被取消注释"
#endif


// 说明：此文件包含一个简单的 CPU 端水波演示实现，使用多个正弦波分量叠加。
// 主要步骤：
// 1) 定义波分量参数结构 `WaveComponent`（振幅、波数、方向、角速度、相位、衰减）
// 2) 提供 `WaterHeight` 和 `WaterNormal` 函数计算高度与法线
// 3) 在 `main` 中初始化 SDL、窗口与渲染器
// 4) 在主循环中处理事件、计算网格高度、逐单元计算光照并绘制
// 注：坐标系以窗口中心为 (0,0)，x,y 单位为像素，时间 t 单位为毫秒

struct WaveComponent {
    float A;    // 振幅（像素）
    float k;    // 波数（弧度/像素）
    float dirX; // 方向单位向量 x 分量
    float dirY; // 方向单位向量 y 分量
    float omega; // 角速度（弧度/毫秒）
    float phase; // 相位（弧度）
    float damp;  // 衰减系数（1/毫秒）
};
typedef struct Vec3 {
    float x, y, z;
}Vec3;

float inner_product(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 cross_product(const Vec3& a, const Vec3& b) {
    return Vec3{
        a.y * b.z - a.z * b.y,  
        a.z * b.x - a.x * b.z,  
        a.x * b.y - a.y * b.x   
    };
}

static void normalizeDir(float &x, float &y) {
    float l = std::sqrtf(x*x + y*y);
    if (l > 0.000001f) { x /= l; y /= l; }
}

// 将波方向向量归一化，确保 dirX/dirY 是单位向量
// 用于把传播方向正确地映射到 x,y 平面

// 计算给定位置 (x,y) 和时间 t_ms 的水面高度
// 模型（分量叠加）：
// H(x,y,t) = Σ A_i * sin(k_i * (dir_i·pos) - ω_i * t + φ_i) * exp(-damp_i * t)
// 说明：
// - x,y 单位为像素（窗口中心为原点）
// - t_ms 单位为毫秒；若已知频率 f(Hz)，ω = 2π*f/1000 （弧度/毫秒）
// - 波数 k 由像素单位的波长 λ 转换：k = 2π / λ
// - damp 为指数衰减系数（1/ms），设为 0 则不衰减
float WaterHeight(const std::vector<WaveComponent>& comps, float x, float y, float t_ms) {
    float h = 0.0f;
    for (const auto &c : comps) {
        float d = x * c.dirX + y * c.dirY;
        float arg = c.k * d - c.omega * t_ms + c.phase;
        h += c.A * std::sinf(arg) * std::expf(-c.damp * t_ms);
    }
    return h;
}

// 计算给定位置和时间的法线（用于光照）
// 采用近似法线：n = (-∂H/∂x, -∂H/∂y, 1)，并归一化
// ∂/∂x [A*sin(k*(dir·pos) - ωt + φ)] = A * cos(...) * k * dirX
struct Vec3 WaterNormal(const std::vector<WaveComponent>& comps, float x, float y, float t_ms) {
    float dhdx = 0.0f, dhdy = 0.0f;
    for (const auto &c : comps) {
        float d = x * c.dirX + y * c.dirY;
        float arg = c.k * d - c.omega * t_ms + c.phase;
        float common = c.A * std::cosf(arg) * std::expf(-c.damp * t_ms) * c.k;
        dhdx += common * c.dirX;
        dhdy += common * c.dirY;
    }
    float nx = -dhdx;
    float ny = -dhdy;
    float nz = 1.0f;
    float invLen = 1.0f / std::sqrtf(nx*nx + ny*ny + nz*nz);
    struct Vec3 n = {nx*invLen, ny*invLen, nz*invLen};
    return n;
}
#ifdef RUN_ORIGINAL_MAIN
int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
        return 1;
    }

    const int W = 1920, H = 1080;
    SDL_Window* win = SDL_CreateWindow("Wave Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, 0);
    if (!win) { std::cerr << "CreateWindow failed: " << SDL_GetError() << std::endl; SDL_Quit(); return 1; }
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren) { std::cerr << "CreateRenderer failed: " << SDL_GetError() << std::endl; SDL_DestroyWindow(win); SDL_Quit(); return 1; }

    // 已调好的波分量（单位：x,y 为像素，t 为毫秒）
    // 注意：在此处以像素为空间单位进行建模，渲染时将窗口中心作为 (0,0)
    std::vector<WaveComponent> comps;
    const float PI = 3.14159265358979323846f;
    // 初始化波分量（全局周期波）
    // 说明：下面使用波长 lambda（像素）计算波数 k = 2π / lambda，
    //       使用频率 f（Hz）计算角速度 omega = 2π * f / 1000（弧度/毫秒）
    // 大尺度波：波长 lambda = 1200 像素
    float lambda1 = 1200.0f; float k1 = 2.0f * PI / lambda1; float f1 = 0.02f; float omega1 = 2.0f * PI * f1 / 1000.0f;
    comps.push_back({50.0f, k1, 1.0f, 0.0f, omega1, 0.0f, 0.0f});
    // 中尺度波：增大波长以减少波密度
    float lambda2 = 600.0f; float k2 = 2.0f * PI / lambda2; float f2 = 0.12f; float omega2 = 2.0f * PI * f2 / 1000.0f;
    comps.push_back({18.0f, k2, 0.8f, 0.6f, omega2, 1.0f, 0.0f});
    // 细节波：使细波更稀疏且幅度更弱
    float lambda3 = 180.0f; float k3 = 2.0f * PI / lambda3; float f3 = 0.6f; float omega3 = 2.0f * PI * f3 / 1000.0f;
    comps.push_back({4.0f, k3, -0.3f, 0.95f, omega3, 0.5f, 0.0f});
    for (auto &c : comps) normalizeDir(c.dirX, c.dirY);

    // 采样与渲染说明：
    // - sampleScale 控制网格采样大小（像素）。1 = 逐像素采样，最清晰但最耗 CPU；
    // - gw, gh 为采样网格的宽高（以采样单元计），渲染时对每个单元取中心点计算高度与法线。

    bool running = true;
    SDL_Event ev;
    const int sampleScale = 2; // 网格采样大小（像素）。1 表示逐像素采样，清晰但更慢
    const int gw = W / sampleScale;
    const int gh = H / sampleScale;

    std::vector<float> grid(gw * gh);

    while (running) {
        // 事件处理：检查窗口关闭和 Esc 键退出
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = false;
            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) running = false;
        }

        // 以毫秒为单位获取当前时间，用于波动计算
        float t = static_cast<float>(SDL_GetTicks()); // ms

        // 预计算网格上每个采样点的高度（按采样块中心点采样）
        for (int gy = 0; gy < gh; ++gy) {
            for (int gx = 0; gx < gw; ++gx) {
                // 计算该采样单元中心在以窗口中心为原点的坐标
                float x = (gx*sampleScale + sampleScale*0.5f) - W*0.5f;
                float y = (gy*sampleScale + sampleScale*0.5f) - H*0.5f;
                // 计算高度并存入网格缓存，供渲染阶段使用
                float h = WaterHeight(comps, x, y, t);
                grid[gy*gw + gx] = h;
            }
        }

        SDL_SetRenderDrawColor(ren, 10, 24, 40, 255);
        SDL_RenderClear(ren);

        // 灯光设置（归一化光照方向和视角）
        struct Vec3 light = {0.6f, -0.4f, 0.7f};
        float llen = std::sqrtf(light.x*light.x+light.y*light.y+light.z*light.z);
        if (llen > 0.0f) { light.x/=llen; light.y/=llen; light.z/=llen; }
        struct Vec3 view = {0.0f, 0.0f, 1.0f};
        struct Vec3 halfv = {light.x+view.x, light.y+view.y, light.z+view.z};
        float hlen = std::sqrtf(halfv.x*halfv.x+halfv.y*halfv.y+halfv.z*halfv.z);
        if (hlen > 0.0f) { halfv.x/=hlen; halfv.y/=hlen; halfv.z/=hlen; }

        auto clamp01 = [](float v){ return std::min(1.0f, std::max(0.0f, v)); };

        // 渲染网格单元（按采样块中心计算法线并着色）
        for (int gy = 0; gy < gh; ++gy) {
            for (int gx = 0; gx < gw; ++gx) {
                int idx = gy*gw + gx;
                // 从高度缓存读取预计算高度
                float h = grid[idx];
                // 重新计算该单元中心坐标（用于法线计算）
                float x = (gx*sampleScale + sampleScale*0.5f) - W*0.5f;
                float y = (gy*sampleScale + sampleScale*0.5f) - H*0.5f;

                // 计算法线用于光照
                auto n = WaterNormal(comps, x, y, t);
                float nx = n.x, ny = n.y, nz = n.z;

                // 漫反射项 N·L
                float NdotL = std::max(0.0f, nx*light.x + ny*light.y + nz*light.z);
                // 略微增强漫反射对比，使明暗边界更清晰
                float diff = std::pow(NdotL, 1.25f) * 1.9f;
                diff = std::min(1.0f, diff);

                // 波峰白化（泡沫/高亮）控制：按高度非线性提升
                float heightBoost = (h + 80.0f) / 160.0f;
                heightBoost = std::pow(std::max(0.0f, heightBoost), 1.8f) * 0.5f;

                // 基础海水颜色（暗蓝）并根据漫反射插值
                float baseR = 20.0f/255.0f, baseG = 70.0f/255.0f, baseB = 140.0f/255.0f;
                float shadedR = baseR * (0.1f + 0.9f * diff);
                float shadedG = baseG * (0.15f + 0.85f * diff);
                float shadedB = baseB * (0.2f + 0.8f * diff);

                // 把白色高光与基础颜色按 heightBoost 混合
                float r = shadedR * (1.0f - heightBoost) + 1.0f * heightBoost;
                float g = shadedG * (1.0f - heightBoost) + 1.0f * heightBoost;
                float b = shadedB * (1.0f - heightBoost) + 0.9f * heightBoost;

                // 计算半程向量与法线的镜面项 H·N，用于高光（Blinn-Phong）
                float HdotN = std::max(0.0f, halfv.x*nx + halfv.y*ny + halfv.z*nz);
                // 柔和的镜面高光，但为可见性略微增强
                float spec = std::pow(HdotN, 25.0f) * 0.28f;
                r += spec; g += spec; b += spec*0.7f;

                // 颜色夹束与伽马校正
                r = clamp01(r); g = clamp01(g); b = clamp01(b);
                float gamma = 1.8f;
                r = std::powf(r, 1.0f/gamma);
                g = std::powf(g, 1.0f/gamma);
                b = std::powf(b, 1.0f/gamma);

                // 转换为 8-bit 并绘制采样单元
                int ir = static_cast<int>(r*255.0f);
                int ig = static_cast<int>(g*255.0f);
                int ib = static_cast<int>(b*255.0f);
                SDL_SetRenderDrawColor(ren, ir, ig, ib, 255);
                SDL_Rect rect{gx*sampleScale, gy*sampleScale, sampleScale, sampleScale};
                SDL_RenderFillRect(ren, &rect);
            }
        }

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
#endif
#ifdef RUN_MODULAR_MAIN
//模块函数声明
bool InitializeSDL(SDL_Window** window, SDL_Renderer** renderer, int width, int height);
void CleanupSDL(SDL_Window* window, SDL_Renderer* renderer);
std::vector<WaveComponent> InitializeWaveComponents();//波分量初始化
bool HandleEvents();
void PrecomputeHeightMap(const std::vector<WaveComponent>& comps, std::vector<float>& grid, 
                        int gw, int gh, int sampleScale, int width, int height, float time);//计算波的高度
void RenderWater(SDL_Renderer* renderer, const std::vector<WaveComponent>& comps,
                const std::vector<float>& grid, int gw, int gh, 
                int sampleScale, int width, int height, float time);//渲染函数

int main(int argc, char** argv) {
    
    const int WIDTH = 1920, HEIGHT = 1080;
    const int SAMPLE_SCALE = 2;
    const int GRID_WIDTH = WIDTH / SAMPLE_SCALE;
    const int GRID_HEIGHT = HEIGHT / SAMPLE_SCALE;
    
    // 初始化SDL
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!InitializeSDL(&window, &renderer, WIDTH, HEIGHT)) {
        return 1;
    }
    
    // 初始化波分量
    std::vector<WaveComponent> waveComps = InitializeWaveComponents();
    std::vector<float> heightGrid(GRID_WIDTH * GRID_HEIGHT);
    
    // 主循环
    bool running = true;
    while (running) {
        running = HandleEvents();
        float currentTime = static_cast<float>(SDL_GetTicks());
        
        PrecomputeHeightMap(waveComps, heightGrid, GRID_WIDTH, GRID_HEIGHT, 
                           SAMPLE_SCALE, WIDTH, HEIGHT, currentTime);
        
        RenderWater(renderer, waveComps, heightGrid, GRID_WIDTH, GRID_HEIGHT,
                   SAMPLE_SCALE, WIDTH, HEIGHT, currentTime);
        
        SDL_Delay(16);
    }
    
    CleanupSDL(window, renderer);
    return 0;
}

bool InitializeSDL(SDL_Window** window, SDL_Renderer** renderer, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
        return false;
    }

    *window = SDL_CreateWindow("Wave Demo - Modular", SDL_WINDOWPOS_CENTERED, 
                              SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!*window) {
        std::cerr << "CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer) {
        std::cerr << "CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return false;
    }
    return true;
}

void CleanupSDL(SDL_Window* window, SDL_Renderer* renderer) {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

std::vector<WaveComponent> InitializeWaveComponents() {
    std::vector<WaveComponent> comps;
    const float PI = 3.14159265358979323846f;
    
    float lambda1 = 1200.0f; float k1 = 2.0f * PI / lambda1; float f1 = 0.02f; float omega1 = 2.0f * PI * f1 / 1000.0f;
    comps.push_back({50.0f, k1, 1.0f, 0.0f, omega1, 0.0f, 0.0f});
    
    float lambda2 = 600.0f; float k2 = 2.0f * PI / lambda2; float f2 = 0.12f; float omega2 = 2.0f * PI * f2 / 1000.0f;
    comps.push_back({18.0f, k2, 0.8f, 0.6f, omega2, 1.0f, 0.0f});
    
    float lambda3 = 180.0f; float k3 = 2.0f * PI / lambda3; float f3 = 0.6f; float omega3 = 2.0f * PI * f3 / 1000.0f;
    comps.push_back({4.0f, k3, -0.3f, 0.95f, omega3, 0.5f, 0.0f});
    
    for (auto &c : comps) normalizeDir(c.dirX, c.dirY);
    return comps;
}

bool HandleEvents() {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) return false;
        if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) return false;
    }
    return true;
}

void PrecomputeHeightMap(const std::vector<WaveComponent>& comps, std::vector<float>& grid, 
                        int gw, int gh, int sampleScale, int width, int height, float time) {
    for (int gy = 0; gy < gh; ++gy) {
        for (int gx = 0; gx < gw; ++gx) {
            float x = (gx * sampleScale + sampleScale * 0.5f) - width * 0.5f;
            float y = (gy * sampleScale + sampleScale * 0.5f) - height * 0.5f;
            float h = WaterHeight(comps, x, y, time);
            grid[gy * gw + gx] = h;
        }
    }
}

struct Color {
    int r, g, b;
};

Color CalculateWaterColor(float h, const Vec3& normal, const Vec3& light, 
                         const Vec3& halfv, int gx, int gy, 
                         int sampleScale, int width, int height, float time) {
    auto clamp01 = [](float v){ return std::min(1.0f, std::max(0.0f, v)); };
    
    Vec3 n{normal.x, normal.y, normal.z};
    Vec3 viewDir{0.0f, 0.0f, 1.0f}; // 假设视线方向为(0,0,1)
    //float nx = normal.x, ny = normal.y, nz = normal.z;
    
    // 基于波高计算基础亮度：将h线性映射到[0,1]范围
    // 这里是根据黄老师给出的波高范围，波高在[-77,77],这里大致取[-80, 80]范围内，使用时根据实际修改，映射公式：height_brightness = (h + 80) / 160
    float height_brightness = (h + 80.0f) / 160.0f;
    height_brightness = clamp01(height_brightness);
    
    // 计算法向量与光照方向的点积（允许负值，区分朝向/背对光照）
    float NdotL = inner_product(n,light); // 不再使用std::max(0, ...)限制
    
    // 根据NdotL正负计算光照因子
    float light_factor;
    if (NdotL > 0.0f) {
        // 朝向光照方向：增加亮度（因子>1模拟高光）
        light_factor = 1.0f + 0.5f * NdotL; // 可调整系数，当前为1.0~1.5（当NdotL=0~1）
    } else {
        // 背对光照方向：加深阴影（因子<1）
        light_factor = 0.3f + 0.2f * (1.0f + NdotL); // 当NdotL=-1时因子=0.3，NdotL=0时因子=0.5，这里光照因子延用黄老师的参数，后续根据实际调整
        light_factor = std::max(0.0f, light_factor); // 确保非负
    }
    
    // 结合波高亮度和光照因子
    float final_brightness = height_brightness * light_factor;
    final_brightness = clamp01(final_brightness);

    // 环境光  
    float ambientStrength = 0.1f; // 环境光强度，根据实际调整
    Vec3 ambientColor = {0.02f, 0.05f, 0.1f}; // 深蓝色环境光
    // 菲涅尔效应
    Vec3 viewDirNormalized = {viewDir.x, viewDir.y, viewDir.z}; // 应已归一化
    float VdotN = std::max(0.0f, std::min(1.0f, inner_product(viewDirNormalized, n)));
    float fresnelRatio = 0.02f + (1.0f - 0.02f) * std::pow(1.0f - VdotN, 5.0f); // Schlick近似，这里菲涅尔系数0.02可调
    //光线衰减
    float depthAttenuation = 1.0f - (h + 80.0f) / 160.0f * 0.5f; // 波谷(h小)处更暗
    // 颜色混合
    // 基础水色 (透射色/折射色)
    Vec3 waterBaseColor = {20.0f/255.0f, 70.0f/255.0f, 140.0f/255.0f};
    // 反射色 (如天空色)
    Vec3 reflectColor = {0.3f, 0.4f, 0.6f}; // 浅蓝色，模拟天空反射
    
    // 应用菲涅尔混合：结合反射色和水底色
    Vec3 mixedColor;
    mixedColor.x = reflectColor.x * fresnelRatio + waterBaseColor.x * (1.0f - fresnelRatio);
    mixedColor.y = reflectColor.y * fresnelRatio + waterBaseColor.y * (1.0f - fresnelRatio);
    mixedColor.z = reflectColor.z * fresnelRatio + waterBaseColor.z * (1.0f - fresnelRatio);
    
    // 应用光照和衰减
    float shadedR = (ambientColor.x + mixedColor.x * final_brightness) * depthAttenuation;
    float shadedG = (ambientColor.y + mixedColor.y * final_brightness) * depthAttenuation;
    float shadedB = (ambientColor.z + mixedColor.z * final_brightness) * depthAttenuation;
    
    // 计算镜面高光（Blinn-Phong模型），仅用于朝向光照区域
    float HdotN = std::max(0.0f, inner_product(halfv, n));
    float spec = std::pow(HdotN, 25.0f) * 0.28f;
    
    // 仅当朝向光照时添加高光效果
    if (NdotL > 0.0f) {
        shadedR += spec;
        shadedG += spec;
        shadedB += spec * 0.7f; // 蓝色通道高光减弱以保持自然色调
    }

    // 颜色钳制与伽马校正
    shadedR = clamp01(shadedR);
    shadedG = clamp01(shadedG);
    shadedB = clamp01(shadedB);
    float gamma = 1.8f;
    float r = std::powf(shadedR, 1.0f/gamma);
    float g = std::powf(shadedG, 1.0f/gamma);
    float b = std::powf(shadedB, 1.0f/gamma);
    r = clamp01(r); g = clamp01(g); b = clamp01(b);

    // 返回Color结构体，包含RGB整数值
    return {static_cast<int>(r * 255.0f), static_cast<int>(g * 255.0f), static_cast<int>(b * 255.0f)};
}

void RenderWater(SDL_Renderer* renderer, const std::vector<WaveComponent>& comps,
                const std::vector<float>& grid, int gw, int gh, 
                int sampleScale, int width, int height, float time) {
    SDL_SetRenderDrawColor(renderer, 10, 24, 40, 255);
    SDL_RenderClear(renderer);
    
    Vec3 light = {0.6f, -0.4f, 0.7f};
    float llen = std::sqrtf(light.x * light.x + light.y * light.y + light.z * light.z);
    if (llen > 0.0f) { light.x /= llen; light.y /= llen; light.z /= llen; }
    
    Vec3 view = {0.0f, 0.0f, 1.0f};
    Vec3 halfv = {light.x + view.x, light.y + view.y, light.z + view.z};
    float hlen = std::sqrtf(halfv.x * halfv.x + halfv.y * halfv.y + halfv.z * halfv.z);
    if (hlen > 0.0f) { halfv.x /= hlen; halfv.y /= hlen; halfv.z /= hlen; }
    
    for (int gy = 0; gy < gh; ++gy) {
        for (int gx = 0; gx < gw; ++gx) {
            float h = grid[gy * gw + gx];
            float x = (gx * sampleScale + sampleScale * 0.5f) - width * 0.5f;
            float y = (gy * sampleScale + sampleScale * 0.5f) - height * 0.5f;
            
            Vec3 normal = WaterNormal(comps, x, y, time);
            Color color = CalculateWaterColor(h, normal, light, halfv, gx, gy, 
                                           sampleScale, width, height, time);
            
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
            SDL_Rect rect{gx * sampleScale, gy * sampleScale, sampleScale, sampleScale};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_RenderPresent(renderer);
}
#endif
