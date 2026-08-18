#pragma once

// GameSettings.hpp
// 项目级游戏设置（替代旧引擎的 SettingsManager）
//
// 新版 OpenCore 已移除 SettingsManager，本项目在项目层重新实现，
// 保留原接口（含净水器等玩法状态），供各场景与玩法系统使用。

#include <cmath>
#include <memory>

namespace Eclipsea
{

class GameSettings
{
  public:
    static GameSettings &getInstance()
    {
        static GameSettings instance;
        return instance;
    }

    void RefreshSettings();

    // 音量 / 渲染 / 多人模式
    std::shared_ptr<float> getMusicVolume() const noexcept
    {
        return musicVolume;
    }
    std::shared_ptr<float> getChunkVolume() const noexcept
    {
        return soundVolume;
    }
    std::shared_ptr<float> getRenderFactor() const noexcept
    {
        return renderFactor;
    }
    std::shared_ptr<bool> getMultiStatus() const noexcept
    {
        return multiplayerMode;
    }
    std::shared_ptr<uint8_t> getRenderWidth() const noexcept
    {
        return render_width;
    }
    std::shared_ptr<uint8_t> getRenderHeight() const noexcept
    {
        return render_height;
    }

    int getTargetWidth() const noexcept { return targetRenderWidth; }
    int getTargetHeight() const noexcept { return targetRenderHeight; }

    void setTargetSize(int width, int height)
    {
        targetRenderWidth  = width;
        targetRenderHeight = height;
    }

    // 设计基准分辨率：所有针对 1920×1080 硬编码的绝对坐标均以此换算到
    // 当前逻辑分辨率（如 4K），避免高分辨率下布局错位。
    static constexpr int kDesignWidth  = 1920;
    static constexpr int kDesignHeight = 1080;

    // 设计基准坐标 → 当前逻辑分辨率坐标。
    // 返回 int；Move 等 uint16_t 参数经隐式转换保持原有负值回绕语义。
    int designX(int designX) const noexcept
    {
        return static_cast<int>(std::round(static_cast<float>(designX) *
                                           targetRenderWidth / kDesignWidth));
    }
    int designY(int designY) const noexcept
    {
        return static_cast<int>(std::round(static_cast<float>(designY) *
                                           targetRenderHeight / kDesignHeight));
    }

    // 净水器玩法状态
    std::shared_ptr<float> getWaterStorage() const noexcept
    {
        return waterStorage;
    }
    std::shared_ptr<float> getPurifiedWaterStorage() const noexcept
    {
        return purifiedWaterStorage;
    }
    std::shared_ptr<float> getWaterStorageMax() const noexcept
    {
        return waterStorageMax;
    }
    std::shared_ptr<float> getWaterPurifyRate() const noexcept
    {
        return waterPurifyRate;
    }

  private:
    GameSettings() = default;

    std::shared_ptr<float>   musicVolume     = std::make_shared<float>(0.3f);
    std::shared_ptr<float>   soundVolume     = std::make_shared<float>(0.3f);
    std::shared_ptr<bool>    multiplayerMode = std::make_shared<bool>(false);
    std::shared_ptr<float>   renderFactor    = std::make_shared<float>(1.0f);
    std::shared_ptr<uint8_t> render_width    = std::make_shared<uint8_t>(16);
    std::shared_ptr<uint8_t> render_height   = std::make_shared<uint8_t>(9);

    std::shared_ptr<float> purifiedWaterStorage = std::make_shared<float>(0.0f);
    std::shared_ptr<float> waterStorage         = std::make_shared<float>(1.0f);
    std::shared_ptr<float> waterStorageMax      = std::make_shared<float>(1.0f);
    std::shared_ptr<float> waterPurifyRate      = std::make_shared<float>(1.0f);

    int targetRenderWidth  = 1920;
    int targetRenderHeight = 1080;
};

} // namespace Eclipsea
