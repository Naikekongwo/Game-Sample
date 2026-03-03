
#include "OpenCore/OpenCore.hpp"
#include <memory>

void SettingsManager::RefreshSettings()
{
    int musicVol = static_cast<int>(128.0f * (*musicVolume));
    int seVol = static_cast<int>(128.0f * (*soundVolume));
    OpenCoreManagers::SFXManager.setVolume(musicVol);
    OpenCoreManagers::SFXManager.setAllSEVolume(seVol);
}