/*

Most of the Meshtastic firmware uses preprocessor macros throughout the code to support different hardware variants.
NicheGraphics attempts a different approach:

Per-device config takes place in this setupNicheGraphics() method
(And a small amount in platformio.ini)

This file sets up InkHUD for the LilyGo T-Deck Pro (v1.0 and v1.1).
Input is the physical keyboard. Touch is not used.

Keys, when not typing a message:
    Enter       select / open menu
    Backspace   back
    W A S D     up, left, down, right (also Alt + E S F X)
    N / P       next / previous applet
    M           menu
    C or R      compose a message (Enter sends, then pick channel or DM; Alt + Q cancels)

*/

#pragma once

#include "configuration.h"

#ifdef MESHTASTIC_INCLUDE_NICHE_GRAPHICS

// InkHUD-specific components
// ---------------------------
#include "graphics/niche/InkHUD/InkHUD.h"

// Applets
#include "graphics/niche/InkHUD/Applets/User/AllMessage/AllMessageApplet.h"
#include "graphics/niche/InkHUD/Applets/User/DM/DMApplet.h"
#include "graphics/niche/InkHUD/Applets/User/FavoritesMap/FavoritesMapApplet.h"
#include "graphics/niche/InkHUD/Applets/User/Heard/HeardApplet.h"
#include "graphics/niche/InkHUD/Applets/User/Positions/PositionsApplet.h"
#include "graphics/niche/InkHUD/Applets/User/RecentsList/RecentsListApplet.h"
#include "graphics/niche/InkHUD/Applets/User/ThreadedMessage/ThreadedMessageApplet.h"

// Shared NicheGraphics components
// --------------------------------
#include "graphics/niche/Drivers/EInk/GDEQ031T10.h"
#include "graphics/niche/Inputs/TDeckProKeys.h"
#include "input/TCA8418KeyboardBase.h"

void setupNicheGraphics()
{
    using namespace NicheGraphics;

    // E-Ink Driver
    // -----------------------------
    // Display shares the SPI bus with the LoRa radio and SD card. main.cpp has already started it.

    Drivers::EInk *driver = new Drivers::GDEQ031T10;
    driver->begin(&SPI, PIN_EINK_DC, PIN_EINK_CS, PIN_EINK_BUSY, PIN_EINK_RES);

    // InkHUD
    // ----------------------------

    InkHUD::InkHUD *inkhud = InkHUD::InkHUD::getInstance();

    // Set the driver
    inkhud->setDriver(driver);

    // Set how many FAST updates per FULL update
    // Set how unhealthy additional FAST updates beyond this number are
    inkhud->setDisplayResilience(10, 1.5);

    // Select fonts
    InkHUD::Applet::fontLarge = FREESANS_12PT_WIN1252;
    InkHUD::Applet::fontMedium = FREESANS_9PT_WIN1252;
    InkHUD::Applet::fontSmall = FREESANS_6PT_WIN1252;

    // Keyboard is the only input: typed freetext, no on-screen keyboard
    inkhud->physicalKeyboard = true;

    // Customize default settings
    inkhud->persistence->settings.userTiles.maxCount = 2; // Two applets, stacked
    inkhud->persistence->settings.rotation = 0;           // Portrait, keyboard below the screen
    inkhud->persistence->settings.userTiles.count = 1;    // One tile only by default, keep things simple for new users
    inkhud->persistence->settings.optionalFeatures.batteryIcon = true;
    inkhud->persistence->settings.optionalMenuItems.backlight = false;
    inkhud->persistence->settings.joystick.enabled = true; // Keyboard provides up/down/left/right/select/back
    inkhud->persistence->settings.joystick.aligned = true; // Keys are fixed, no alignment step needed

    // Pick applets
    // Note: order of applets determines priority of "auto-show" feature
    inkhud->addApplet("All Messages", new InkHUD::AllMessageApplet, true, true);        // Activated, autoshown
    inkhud->addApplet("DMs", new InkHUD::DMApplet, true, true);                         // Activated, autoshown
    inkhud->addApplet("Channel 0", new InkHUD::ThreadedMessageApplet(0), true, true);   // Activated, autoshown
    inkhud->addApplet("Channel 1", new InkHUD::ThreadedMessageApplet(1), false, false); // Not active
    inkhud->addApplet("Positions", new InkHUD::PositionsApplet, false, false);          // Not active
    inkhud->addApplet("Favorites Map", new InkHUD::FavoritesMapApplet, false, false);   // Not active
    inkhud->addApplet("Recents List", new InkHUD::RecentsListApplet, true, false);      // Activated, not autoshown
    inkhud->addApplet("Heard", new InkHUD::HeardApplet, true, false, 0);                // Activated, default on tile 0

    // Start running InkHUD
    inkhud->begin();

    // Keyboard
    // --------------------------

    Inputs::TDeckProKeys *keys = Inputs::TDeckProKeys::getInstance();

    keys->setHandler([inkhud](char c) {
        using Key = TCA8418KeyboardBase::TCA8418Key;
        const uint8_t k = (uint8_t)c;

        // Typing a message: everything is text, except send / cancel / delete
        if (inkhud->isFreeTextActive()) {
            if (k == Key::SELECT)
                inkhud->freeTextDone();
            else if (k == Key::ESC)
                inkhud->freeTextCancel();
            else if (k == Key::BSP)
                inkhud->freeText('\b');
            else if (k >= 0x20 && k < 0x7F)
                inkhud->freeText(c);
            return;
        }

        // Otherwise, keys navigate
        switch (k) {
        case Key::SELECT:
            inkhud->shortpress();
            break;
        case Key::BSP:
        case Key::ESC:
            inkhud->exitShort();
            break;
        case Key::UP:
        case 'w':
        case 'W':
            inkhud->navUp();
            break;
        case Key::DOWN:
        case 's':
        case 'S':
            inkhud->navDown();
            break;
        case Key::LEFT:
        case 'a':
        case 'A':
            inkhud->navLeft();
            break;
        case Key::RIGHT:
        case 'd':
        case 'D':
            inkhud->navRight();
            break;
        case Key::TAB:
        case 'n':
        case 'N':
            inkhud->nextApplet();
            break;
        case 'p':
        case 'P':
            inkhud->prevApplet();
            break;
        case 'm':
        case 'M':
            inkhud->openMenu();
            break;
        case 'c':
        case 'C':
        case 'r':
        case 'R':
            inkhud->composeMessage();
            break;
        default:
            break;
        }
    });

    keys->start();
}

#endif
