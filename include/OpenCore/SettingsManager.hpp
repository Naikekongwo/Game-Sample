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

  private:
    shared_ptr<float> musicVolume = std::make_shared<float>(1.0f);
};