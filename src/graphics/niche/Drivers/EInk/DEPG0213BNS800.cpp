#include "./DEPG0213BNS800.h"

#ifdef WATER_SENSOR_MESH_INCLUDE_NICHE_GRAPHICS

using namespace NicheGraphics::Drivers;

// Describes the operation performed when a "fast refresh" is performed
// Source: Modified from GxEPD2 (GxEPD2_213_BN)
static const uint8_t LUT_FAST[] = {
    // 1     2     3
    0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // B2B (Existing black pixels)
    0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // B2W (New white pixels)
    0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // W2B (New black pixels)
    0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // W2W (Existing white pixels)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // VCOM

    0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,             // 1. Any pixels changing W2B or B2W. Two medium taps.
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             // 2. All pixels. One short tap.
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             // 3. Cooldown
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             //
    0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, //
};

// How strongly the pixels are pulled and pushed
void DEPG0213BNS800::configVoltages()
{
    switch (updateType) {
    case FAST:
        // Reference: display datasheet, GxEPD1
        sendCommand(0x03); // Gate voltage
        sendData(0x17);    // VGH: 20V

        // Reference: display datasheet, GxEPD1
        sendCommand(0x04); // Source voltage
        sendData(0x41);    // VSH1: 15V
        sendData(0x00);    // VSH2: NA
        sendData(0x32);    // VSL: -15V

        // GxEPD1 sets this at -1.2V, but that seems to be drive the pixels very hard
        sendCommand(0x2C); // VCOM voltage
        sendData(0x08);    // VCOM: -0.2V
        break;

    case FULL:
    default:
        // From OTP memory
        break;
    }
}

// Load settings about how the pixels are moved from old state to new state during a refresh
// - manually specified,
// - or with stored values from displays OTP memory
void DEPG0213BNS800::configWaveform()
{
    switch (updateType) {
    case FAST:
        sendCommand(0x3C); // Border waveform:
        sendData(0x80);    // VSS

        sendCommand(0x32);                    // Write LUT register from MCU:
        sendData(LUT_FAST, sizeof(LUT_FAST)); // (describes operation for a FAST refresh)
        break;

    case FULL:
    default:
        // From OTP memory
        break;
    }
}

// Describes the sequence of events performed by the displays controller IC during a refresh
// Includes "power up", "load settings from memory", "update the pixels", etc
void DEPG0213BNS800::configUpdateSequence()
{
    switch (updateType) {
    case FAST:
        sendCommand(0x22); // Set "update sequence"
        sendData(0xCF);    // Differential, use manually loaded waveform
        break;

    case FULL:
    default:
        sendCommand(0x22); // Set "update sequence"
        sendData(0xF7);    // Non-differential, load waveform from OTP
        break;
    }
}

// Once the refresh operation has been started,
// begin periodically polling the display to check for completion, using the normal Water_Sensor_Mesh threading code
// Only used when refresh is "async"
void DEPG0213BNS800::detachFromUpdate()
{
    switch (updateType) {
    case FAST:
        return beginPolling(50, 500); // At least 500ms, then poll every 50ms
    case FULL:
    default:
        return beginPolling(100, 3500); // At least 3500ms, then poll every 100ms
    }
}

// For this display, we do not need to re-write the new image.
// We're overriding SSD16XX::finalizeUpdate to make this small optimization.
// The display does also work just fine with the generic SSD16XX method, though.
void DEPG0213BNS800::finalizeUpdate()
{
    // Put a copy of the image into the "old memory".
    // Used with differential refreshes (e.g. FAST update), to determine which px need to move, and which can remain in place
    // We need to keep the "old memory" up to date, because don't know whether next refresh will be FULL or FAST etc.
    if (updateType != FULL) {
        // writeNewImage(); // Not required for this display
        writeOldImage();
        sendCommand(0x7F); // Terminate image write without update
        wait();
    }

    // Enter deep-sleep to save a few µA
    // Waking from this requires that display's reset pin is broken out
    if (pin_rst != 0xFF)
        deepSleep();
}
#endif // WATER_SENSOR_MESH_INCLUDE_NICHE_GRAPHICS