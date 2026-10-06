#ifdef MESHTASTIC_INCLUDE_NICHE_GRAPHICS

/*

Re-usable NicheGraphics input source

LilyGo T-Deck Pro physical keyboard (TCA8418 over I2C)
Reuses the BaseUI keymap from src/input/TDeckProKeyboard, but hands each key to a NicheGraphics callback
instead of the InputBroker, which NicheGraphics builds exclude.

*/

#pragma once

#include "configuration.h"

#if defined(T_DECK_PRO)

#include "concurrency/OSThread.h"
#include "functional"

class TDeckProKeyboard;

namespace NicheGraphics::Inputs
{

class TDeckProKeys : protected concurrency::OSThread
{
  public:
    typedef std::function<void(char)> Callback;

    static TDeckProKeys *getInstance(); // Create or get the singleton instance
    void setHandler(Callback onKey);    // Receives printable chars, and TCA8418KeyboardBase::TCA8418Key values
    void start();                       // Begin polling the keyboard

  private:
    TDeckProKeys();
    int32_t runOnce() override;

    TDeckProKeyboard *keyboard = nullptr;
    Callback onKey = nullptr;
    uint32_t lastKeyAt = 0;
};

} // namespace NicheGraphics::Inputs

#endif // T_DECK_PRO

#endif // MESHTASTIC_INCLUDE_NICHE_GRAPHICS
