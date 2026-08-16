#include "Eclipsea/Core/GameSettings.hpp"
#include "Eclipsea/Core/AudioManager.hpp"

namespace Eclipsea
{

void GameSettings::RefreshSettings()
{
    int musicVol = static_cast<int>(128.0f * (*musicVolume));
    int seVol    = static_cast<int>(128.0f * (*soundVolume));
    AudioManager::getInstance().setVolume(musicVol);
    AudioManager::getInstance().setAllSEVolume(seVol);
}

} // namespace Eclipsea
