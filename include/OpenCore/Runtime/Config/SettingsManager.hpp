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

  private:
    shared_ptr<float> musicVolume = std::make_shared<float>(0.3f);
    shared_ptr<float> soundVolume = std::make_shared<float>(0.3f);

    shared_ptr<uint8_t> render_width = std::make_shared<uint8_t>(16);
    shared_ptr<uint8_t> render_height = std::make_shared<uint8_t>(9);
};