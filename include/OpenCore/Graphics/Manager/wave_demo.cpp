
#include <SDL2/SDL.h>
#include <vector>
#include <cmath>
#include <array>
#include <algorithm>
#include <iostream>

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
std::array<float,3> WaterNormal(const std::vector<WaveComponent>& comps, float x, float y, float t_ms) {
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
    return {nx*invLen, ny*invLen, nz*invLen};
}

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
        std::array<float,3> light = {0.6f, -0.4f, 0.7f};
        float llen = std::sqrtf(light[0]*light[0]+light[1]*light[1]+light[2]*light[2]);
        if (llen > 0.0f) { light[0]/=llen; light[1]/=llen; light[2]/=llen; }
        std::array<float,3> view = {0.0f, 0.0f, 1.0f};
        std::array<float,3> halfv = {light[0]+view[0], light[1]+view[1], light[2]+view[2]};
        float hlen = std::sqrtf(halfv[0]*halfv[0]+halfv[1]*halfv[1]+halfv[2]*halfv[2]);
        if (hlen > 0.0f) { halfv[0]/=hlen; halfv[1]/=hlen; halfv[2]/=hlen; }

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
                float nx = n[0], ny = n[1], nz = n[2];

                // 漫反射项 N·L
                float NdotL = std::max(0.0f, nx*light[0] + ny*light[1] + nz*light[2]);
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
                float HdotN = std::max(0.0f, halfv[0]*nx + halfv[1]*ny + halfv[2]*nz);
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
