#ifdef MESHTASTIC_INCLUDE_NICHE_GRAPHICS

#include "./GDEQ031T10.h"

using namespace NicheGraphics::Drivers;

GDEQ031T10::GDEQ031T10() : UC8175(width, height, supported) {}

// UC8253 has no software reset command (0x12 is "display refresh"), so the base class fallback is unsuitable.
// Hardware reset once at boot if the pin is wired, then soft-reset via PSR before every update.
void GDEQ031T10::reset()
{
    if (!hardwareResetDone && pin_rst != (uint8_t)-1) {
        digitalWrite(pin_rst, LOW);
        delay(20);
        digitalWrite(pin_rst, HIGH);
        delay(20);
        wait(3000);
    }
    hardwareResetDone = true;

    sendCommand(0x00); // Panel setting
    sendData(0x1E);    // Soft reset
    sendData(0x0D);
    delay(1);
}

void GDEQ031T10::configCommon()
{
    sendCommand(0x00); // Panel setting
    sendData(0x1F);    // B/W, LUT from OTP
    sendData(0x0D);
}

void GDEQ031T10::configFull()
{
    sendCommand(0xE0); // Cascade setting
    sendData(0x02);    // TSFIX: use forced temperature
    sendCommand(0xE5); // Force temperature
    sendData(0x5A);    // Selects the fast OTP full-refresh waveform

    sendCommand(0x50); // VCOM and data interval
    sendData(0x97);

    powerOn();
}

void GDEQ031T10::configFast()
{
    sendCommand(0xE0); // Cascade setting
    sendData(0x02);    // TSFIX: use forced temperature
    sendCommand(0xE5); // Force temperature
    sendData(0x79);    // Selects the OTP partial-refresh waveform

    sendCommand(0x50); // VCOM and data interval
    sendData(0xD7);

    powerOn();
}

void GDEQ031T10::detachFromUpdate()
{
    switch (updateType) {
    case FAST:
        return beginPolling(50, 700);
    case FULL:
    default:
        return beginPolling(100, 1100);
    }
}

void GDEQ031T10::finalizeUpdate()
{
    powerOff();
}

#endif // MESHTASTIC_INCLUDE_NICHE_GRAPHICS
