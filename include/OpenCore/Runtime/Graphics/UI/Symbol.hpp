#pragma once

#include "OpenCore/Core/Math/OpenCore_Vec3.hpp"
#include "OpenCore/Runtime/Graphics/IDrawableObject/IDrawableObject.hpp"
#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"

enum SymbolType : int
{
    SYMBOL_QUESTION,
    SYMBOL_WARNING,
};

class Symbol : public UIElement
{
  public:
    Symbol(std::string_view id, short layer, short texMetaID);

    void SetSymbolType(SymbolType type) { m_symbolType = type; }

    void handlEvents(SDL_Event &event, float totalTime) override;
    void onUpdate(float totalTime) override;

    void Draw() override;

    bool onDestroy() override { return true; }

  private:
    SymbolType m_symbolType = SYMBOL_QUESTION;
};