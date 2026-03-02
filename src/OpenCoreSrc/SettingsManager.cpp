
#include "OpenCore/OpenCore.hpp"
#include <memory>

void SettingsManager::RefreshSettings()
{
    int musicVol = static_cast<int>(128.0f * (*musicVolume));
    OpenCoreManagers::SFXManager.setVolume(musicVol);
}