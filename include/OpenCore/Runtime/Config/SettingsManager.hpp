#pragma once

// SettingsManager
// 设置管理器

#include <cstdint>
#include <memory>

using std::shared_ptr;

class SettingsManager
{
  public:
    static SettingsManager &getInstance()
    {
        static SettingsManager instance;
        return instance;
    }

    void RefreshSettings();

    shared_ptr<float> getMusicVolume() const noexcept { return musicVolume; }
    shared_ptr<uint8_t> getRenderWidth() const noexcept { return render_width; }
    shared_ptr<uint8_t> getRenderHeight() const noexcept
    {
        return render_height;
    }
    shared_ptr<bool> getMultiStatus() const noexcept { return multiplayerMode; }
    shared_ptr<float> getChunkVolume() const noexcept { return soundVolume; }
    shared_ptr<float> getRenderFactor() const noexcept { return renderFactor; }

    int getTargetWidth() const noexcept { return targetRenderWidth; }
    int getTargetHeight() const noexcept { return targetRenderHeight; }

    void setTargetWidth(int &width, int &height)
    {
        targetRenderHeight = height;
        targetRenderWidth = width;
    }

    shared_ptr<float> getWaterStorage() const noexcept { return waterStorage; }
    shared_ptr<float> getPurifiedWaterStorage() const noexcept
    {
        return purifiedWaterStorage;
    }
    shared_ptr<float> getWaterStorageMax() const noexcept
    {
        return waterStorageMax;
    }
    shared_ptr<float> getWaterPurifyRate() const noexcept
    {
        return waterPurifyRate;
    }

  private:
    shared_ptr<float> musicVolume = std::make_shared<float>(0.3f);
    shared_ptr<float> soundVolume = std::make_shared<float>(0.3f);

    shared_ptr<bool> multiplayerMode = std::make_shared<bool>(false);

    shared_ptr<float> renderFactor = std::make_shared<float>(1.0f);

    shared_ptr<uint8_t> render_width = std::make_shared<uint8_t>(16);
    shared_ptr<uint8_t> render_height = std::make_shared<uint8_t>(9);

    shared_ptr<float> purifiedWaterStorage = std::make_shared<float>(0.0f);
    shared_ptr<float> waterStorage = std::make_shared<float>(1.0f);
    shared_ptr<float> waterStorageMax = std::make_shared<float>(1.0f);
    shared_ptr<float> waterPurifyRate = std::make_shared<float>(1.0f);

    int targetRenderWidth = 1920;
    int targetRenderHeight = 1080;
};