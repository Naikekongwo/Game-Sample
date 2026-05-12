#pragma once

#include "OpenCore/OpenCore.hpp"

#include <array>
#include <cstdint>
#include <utility>

namespace ConnectorTransition
{
struct ConnectorLayer
{
    const char *id;
    uint8_t layer;
    float heightScale;
    float alpha;
    uint16_t coverStartX;
    uint16_t coverEndX;
    float coverDuration;
    uint16_t revealStartX;
    uint16_t revealEndX;
    float revealDuration;
};

inline constexpr std::array<ConnectorLayer, 3> kLayers{{
    {"connector_back", 97, 1.08f, 0.32f, 180, 2680, 5.2f, 2680, 5920, 4.8f},
    {"connector", 99, 1.00f, 0.78f, 0, 2700, 5.0f, 2700, 6000, 4.6f},
    {"connector_front", 100, 0.94f, 0.46f, 320, 2920, 4.4f, 2920, 6220, 4.2f},
}};

inline void PushLayer(ElementManager *elements, const ConnectorLayer &preset,
                      short textureId, bool cover)
{
    auto connector = UI<ImageBoard>(preset.id, preset.layer, textureId, 1, 1);
    connector->Configure()
        .Anchor(AnchorPoint::TopRight)
        .Posite(0.0f, 0.0f)
        .Scale(0.0f, preset.heightScale)
        .Sequence(false)
        .Alpha(cover ? 0.0f : preset.alpha);

    if (cover)
    {
        connector->Animate()
            .Move(preset.coverStartX, 0, preset.coverEndX, 0,
                  preset.coverDuration, false)
            .Fade(0.0f, preset.alpha, preset.coverDuration * 0.72f, false)
            .Commit();
    }
    else
    {
        connector->Animate()
            .Move(preset.revealStartX, 0, preset.revealEndX, 0,
                  preset.revealDuration, false)
            .Fade(preset.alpha, 0.0f, preset.revealDuration, false)
            .Commit();
    }

    elements->PushElement(std::move(connector));
}

inline void PushCover(ElementManager *elements, short textureId)
{
    for (const auto &preset : kLayers)
    {
        PushLayer(elements, preset, textureId, true);
    }
}

inline void PushReveal(ElementManager *elements, short textureId)
{
    for (const auto &preset : kLayers)
    {
        PushLayer(elements, preset, textureId, false);
    }
}

inline bool IsFinished(const ElementManager *elements)
{
    for (const auto &preset : kLayers)
    {
        auto connector = elements->find(preset.id);
        if (!connector || !connector->isAnimeFinished())
        {
            return false;
        }
    }

    return true;
}

inline void Remove(ElementManager *elements)
{
    for (const auto &preset : kLayers)
    {
        elements->removeElement(preset.id);
    }
}
} // namespace ConnectorTransition
