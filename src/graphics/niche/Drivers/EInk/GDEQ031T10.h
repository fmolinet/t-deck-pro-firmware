/*

E-Ink display driver
    - GDEQ031T10
    - Controller: UC8253
    - Size: 3.1 inch
    - Resolution: 240px x 320px
    - Used in: LilyGo T-Deck Pro

    Command sequence follows GxEPD2_310_GDEQ031T10.
    The panel is kept out of deep sleep between updates: T-Deck Pro v1.0 has no reset pin wired,
    and deep sleep is unreliable on v1.1 (see EINK_NOT_HIBERNATE in the BaseUI variant).

*/

#pragma once

#ifdef MESHTASTIC_INCLUDE_NICHE_GRAPHICS

#include "configuration.h"

#include "./UC8175.h"

namespace NicheGraphics::Drivers
{

class GDEQ031T10 : public UC8175
{
  private:
    static constexpr uint16_t width = 240;
    static constexpr uint16_t height = 320;
    static constexpr UpdateTypes supported = (UpdateTypes)(FULL | FAST);

  public:
    GDEQ031T10();

  protected:
    void reset() override;
    void configCommon() override;
    void configFull() override;
    void configFast() override;
    void detachFromUpdate() override;
    void finalizeUpdate() override;

  private:
    bool hardwareResetDone = false;
};

} // namespace NicheGraphics::Drivers

#endif // MESHTASTIC_INCLUDE_NICHE_GRAPHICS
