#ifdef WATER_SENSOR_MESH_INCLUDE_INKHUD

/*

Sub-menu for InkHUD::MenuApplet
Structure of the menu is defined in InkHUD::showPage

*/

#pragma once

#include "configuration.h"

namespace NicheGraphics::InkHUD
{

// Sub-menu for MenuApplet
enum MenuPage : uint8_t {
    ROOT, // Initial menu page
    SEND,
    CANNEDMESSAGE_RECIPIENT, // Select destination for a canned message
    OPTIONS,
    APPLETS,
    AUTOSHOW,
    RECENTS, // Select length of "recentlyActiveSeconds"
    EXIT,    // Dismiss the menu applet
};

} // namespace NicheGraphics::InkHUD

#endif