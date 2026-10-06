#ifdef MESHTASTIC_INCLUDE_NICHE_GRAPHICS

#include "./TDeckProKeys.h"

#if defined(T_DECK_PRO)

#include "input/TDeckProKeyboard.h"
#include "mesh/Throttle.h"
#include <Wire.h>

using namespace NicheGraphics::Inputs;

// Poll quickly while the user is typing, slower when idle
static constexpr uint32_t POLL_ACTIVE_MS = 20;
static constexpr uint32_t POLL_IDLE_MS = 100;
static constexpr uint32_t ACTIVE_WINDOW_MS = 5000;

TDeckProKeys::TDeckProKeys() : concurrency::OSThread("TDeckProKeys")
{
    OSThread::disable();
}

TDeckProKeys *TDeckProKeys::getInstance()
{
    static TDeckProKeys *instance = new TDeckProKeys;
    return instance;
}

void TDeckProKeys::setHandler(Callback onKey)
{
    this->onKey = onKey;
}

void TDeckProKeys::start()
{
    if (!keyboard) {
        keyboard = new TDeckProKeyboard();
        keyboard->begin(TCA8418_KB_ADDR, &Wire);
    }
    OSThread::setIntervalFromNow(POLL_IDLE_MS);
    OSThread::enabled = true;
}

int32_t TDeckProKeys::runOnce()
{
    keyboard->trigger();

    while (keyboard->hasEvent()) {
        char key = keyboard->dequeueEvent();
        if (key == TCA8418KeyboardBase::NONE)
            continue;
        lastKeyAt = millis();
        if (onKey)
            onKey(key);
    }

    return Throttle::isWithinTimespanMs(lastKeyAt, ACTIVE_WINDOW_MS) ? POLL_ACTIVE_MS : POLL_IDLE_MS;
}

#endif // T_DECK_PRO

#endif // MESHTASTIC_INCLUDE_NICHE_GRAPHICS
