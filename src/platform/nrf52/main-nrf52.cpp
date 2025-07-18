#include "configuration.h"
#include <Adafruit_TinyUSB.h>
#include <Adafruit_nRFCrypto.h>
#include <InternalFileSystem.h>
#include <SPI.h>
#include <Wire.h>
#include <assert.h>
#include <ble_gap.h>
#include <memory.h>
#include <stdio.h>
// #include <Adafruit_USBD_Device.h>
#include "NodeDB.h"
#include "PowerMon.h"
#include "error.h"
#include "main.h"
#include "meshUtils.h"

#ifdef BQ25703A_ADDR
#include "BQ25713.h"
#endif

static inline void debugger_break(void)
{
    __asm volatile("bkpt #0x01\n\t"
                   "mov pc, lr\n\t");
}

bool loopCanSleep()
{
    // turn off sleep only while connected via USB
    // return true;
    return !Serial; // the bool operator on the nrf52 serial class returns true if connected to a PC currently
    // return !(TinyUSBDevice.mounted() && !TinyUSBDevice.suspended());
}

// handle standard gcc assert failures
void __attribute__((noreturn)) __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
    LOG_ERROR("assert failed %s: %d, %s, test=%s", file, line, func, failedexpr);
    // debugger_break(); FIXME doesn't work, possibly not for segger
    // Reboot cpu
    NVIC_SystemReset();
}

void getMacAddr(uint8_t *dmac)
{
    const uint8_t *src = (const uint8_t *)NRF_FICR->DEVICEADDR;
    dmac[5] = src[0];
    dmac[4] = src[1];
    dmac[3] = src[2];
    dmac[2] = src[3];
    dmac[1] = src[4];
    dmac[0] = src[5] | 0xc0; // MSB high two bits get set elsewhere in the bluetooth stack
}

static void initBrownout()
{
    auto vccthresh = POWER_POFCON_THRESHOLD_V24;

    auto err_code = sd_power_pof_enable(POWER_POFCON_POF_Enabled);
    assert(err_code == NRF_SUCCESS);

    err_code = sd_power_pof_threshold_set(vccthresh);
    assert(err_code == NRF_SUCCESS);

    // We don't bother with setting up brownout if soft device is disabled - because during production we always use softdevice
}

// This is a public global so that the debugger can set it to false automatically from our gdbinit
bool useSoftDevice = true; // Set to false for easier debugging

#if !WATER_SENSOR_MESH_EXCLUDE_BLUETOOTH
void setBluetoothEnable(bool enable)
{
    // For debugging use: don't use bluetooth
    if (!useSoftDevice) {
        if (enable)
            LOG_INFO("Disable NRF52 BLUETOOTH WHILE DEBUGGING");
        return;
    }

    // If user disabled bluetooth: init then disable advertising & reduce power
    // Workaround. Avoid issue where device hangs several days after boot..
    // Allegedly, no significant increase in power consumption
    if (!config.bluetooth.enabled) {
        static bool initialized = false;
        if (!initialized) {
            nrf52Bluetooth = new NRF52Bluetooth();
            nrf52Bluetooth->startDisabled();
            initBrownout();
            initialized = true;
        }
        return;
    }

    if (enable) {
        powerMon->setState(water_sensor_mesh_PowerMon_State_BT_On);

        // If not yet set-up
        if (!nrf52Bluetooth) {
            LOG_DEBUG("Init NRF52 Bluetooth");
            nrf52Bluetooth = new NRF52Bluetooth();
            nrf52Bluetooth->setup();

            // We delay brownout init until after BLE because BLE starts soft device
            initBrownout();
        }
        // Already setup, apparently
        else
            nrf52Bluetooth->resumeAdvertising();
    }
    // Disable (if previously set-up)
    else if (nrf52Bluetooth) {
        powerMon->clearState(water_sensor_mesh_PowerMon_State_BT_On);
        nrf52Bluetooth->shutdown();
    }
}
#else
#warning NRF52 "Bluetooth disable" workaround does not apply to builds with WATER_SENSOR_MESH_EXCLUDE_BLUETOOTH
void setBluetoothEnable(bool enable) {}
#endif
/**
 * Override printf to use the SEGGER output library (note - this does not effect the printf method on the debug console)
 */
int printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    auto res = SEGGER_RTT_vprintf(0, fmt, &args);
    va_end(args);
    return res;
}

namespace
{
constexpr uint8_t NRF52_MAGIC_LFS_IS_CORRUPT = 0xF5;
constexpr uint32_t MULTIPLE_CORRUPTION_DELAY_MILLIS = 20 * 60 * 1000;
static unsigned long millis_until_formatting_again = 0;

// Report the critical error from loop(), giving a chance for the screen to be initialized first.
inline void reportLittleFSCorruptionOnce()
{
    static bool report_corruption = !!millis_until_formatting_again;
    if (report_corruption) {
        report_corruption = false;
        RECORD_CRITICALERROR(water_sensor_mesh_CriticalErrorCode_FLASH_CORRUPTION_UNRECOVERABLE);
    }
}
} // namespace

void preFSBegin()
{
    // The GPREGRET register keeps its value across warm boots. Check that this is a warm boot and, if GPREGRET
    // is set to NRF52_MAGIC_LFS_IS_CORRUPT, format LittleFS.
    if (!(NRF_POWER->RESETREAS == 0 && NRF_POWER->GPREGRET == NRF52_MAGIC_LFS_IS_CORRUPT))
        return;
    NRF_POWER->GPREGRET = 0;
    millis_until_formatting_again = millis() + MULTIPLE_CORRUPTION_DELAY_MILLIS;
    InternalFS.format();
    LOG_INFO("LittleFS format complete; restoring default settings");
}

extern "C" void lfs_assert(const char *reason)
{
    LOG_ERROR("LittleFS corruption detected: %s", reason);
    if (millis_until_formatting_again > millis()) {
        RECORD_CRITICALERROR(water_sensor_mesh_CriticalErrorCode_FLASH_CORRUPTION_UNRECOVERABLE);
        const long millis_remain = millis_until_formatting_again - millis();
        LOG_WARN("Pausing %d seconds to avoid wear on flash storage", millis_remain / 1000);
        delay(millis_remain);
    }
    LOG_INFO("Rebooting to format LittleFS");
    delay(500); // Give the serial port a bit of time to output that last message.
    // Try setting GPREGRET with the SoftDevice first. If that fails (perhaps because the SD hasn't been initialize yet) then set
    // NRF_POWER->GPREGRET directly.
    if (!(sd_power_gpregret_clr(0, 0xFF) == NRF_SUCCESS && sd_power_gpregret_set(0, NRF52_MAGIC_LFS_IS_CORRUPT) == NRF_SUCCESS)) {
        NRF_POWER->GPREGRET = NRF52_MAGIC_LFS_IS_CORRUPT;
    }
    NVIC_SystemReset();
}

void checkSDEvents()
{
    if (useSoftDevice) {
        uint32_t evt;
        while (NRF_SUCCESS == sd_evt_get(&evt)) {
            switch (evt) {
            case NRF_EVT_POWER_FAILURE_WARNING:
                RECORD_CRITICALERROR(water_sensor_mesh_CriticalErrorCode_BROWNOUT);
                break;

            default:
                LOG_DEBUG("Unexpected SDevt %d", evt);
                break;
            }
        }
    } else {
        if (NRF_POWER->EVENTS_POFWARN)
            RECORD_CRITICALERROR(water_sensor_mesh_CriticalErrorCode_BROWNOUT);
    }
}

void nrf52Loop()
{
    checkSDEvents();
    reportLittleFSCorruptionOnce();
}

#ifdef USE_SEMIHOSTING
#include <SemihostingStream.h>
#include <meshUtils.h>

/**
 * Note: this variable is in BSS and therfore false by default.  But the gdbinit
 * file will be installing a temporary breakpoint that changes wantSemihost to true.
 */
bool wantSemihost;

/**
 * Turn on semihosting if the ICE debugger wants it.
 */
void nrf52InitSemiHosting()
{
    if (wantSemihost) {
        static SemihostingStream semiStream;
        // We must dynamically alloc because the constructor does semihost operations which
        // would crash any load not talking to a debugger
        semiStream.open();
        semiStream.println("Semihosting starts!");
        // Redirect our serial output to instead go via the ICE port
        console->setDestination(&semiStream);
    }
}
#endif

void nrf52Setup()
{
#ifdef ADC_V
    pinMode(ADC_V, INPUT);
#endif

    uint32_t why = NRF_POWER->RESETREAS;
    // per
    // https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.nrf52832.ps.v1.1%2Fpower.html
    LOG_DEBUG("Reset reason: 0x%x", why);

#ifdef USE_SEMIHOSTING
    nrf52InitSemiHosting();
#endif

    // Per
    // https://devzone.nordicsemi.com/nordic/nordic-blog/b/blog/posts/monitor-mode-debugging-with-j-link-and-gdbeclipse
    // This is the recommended setting for Monitor Mode Debugging
    NVIC_SetPriority(DebugMonitor_IRQn, 6UL);

#ifdef BQ25703A_ADDR
    auto *bq = new BQ25713();
    if (!bq->setup())
        LOG_ERROR("ERROR! Charge controller init failed");
#endif

    // Init random seed
    union seedParts {
        uint32_t seed32;
        uint8_t seed8[4];
    } seed;
    nRFCrypto.begin();
    nRFCrypto.Random.generate(seed.seed8, sizeof(seed.seed8));
    LOG_DEBUG("Set random seed %u", seed.seed32);
    randomSeed(seed.seed32);
    nRFCrypto.end();
}

void cpuDeepSleep(uint32_t msecToWake)
{
    // FIXME, configure RTC or button press to wake us
    // FIXME, power down SPI, I2C, RAMs
#if HAS_WIRE
    Wire.end();
#endif
    SPI.end();
#if SPI_INTERFACES_COUNT > 1
    SPI1.end();
#endif
    // This may cause crashes as debug messages continue to flow.
    Serial.end();
#ifdef PIN_SERIAL1_RX
    Serial1.end();
#endif
    setBluetoothEnable(false);

#ifdef RAK4630
#ifdef PIN_3V3_EN
    digitalWrite(PIN_3V3_EN, LOW);
#endif
#ifdef AQ_SET_PIN
    // RAK-12039 set pin for Air quality sensor
    digitalWrite(AQ_SET_PIN, LOW);
#endif
#ifdef RAK14014
    // GPIO restores input status, otherwise there will be leakage current
    nrf_gpio_cfg_default(TFT_BL);
    nrf_gpio_cfg_default(TFT_DC);
    nrf_gpio_cfg_default(TFT_CS);
    nrf_gpio_cfg_default(TFT_SCLK);
    nrf_gpio_cfg_default(TFT_MOSI);
    nrf_gpio_cfg_default(TFT_MISO);
    nrf_gpio_cfg_default(SCREEN_TOUCH_INT);
    nrf_gpio_cfg_default(WB_I2C1_SCL);
    nrf_gpio_cfg_default(WB_I2C1_SDA);

    // nrf_gpio_cfg_default(WB_I2C2_SCL);
    // nrf_gpio_cfg_default(WB_I2C2_SDA);
#endif
#endif
#ifdef MESHLINK
#ifdef PIN_WD_EN
    digitalWrite(PIN_WD_EN, LOW);
#endif
#endif

#ifdef HELTEC_MESH_NODE_T114
    nrf_gpio_cfg_default(PIN_GPS_PPS);
    detachInterrupt(PIN_GPS_PPS);
    detachInterrupt(PIN_BUTTON1);
#endif

#ifdef ELECROW_ThinkNode_M1
    for (int pin = 0; pin < 48; pin++) {
        if (pin == 17 || pin == 19 || pin == 20 || pin == 22 || pin == 23 || pin == 24 || pin == 25 || pin == 9 || pin == 10 ||
            pin == PIN_BUTTON1 || pin == PIN_BUTTON2) {
            continue;
        }
        pinMode(pin, OUTPUT);
    }
    for (int pin = 0; pin < 48; pin++) {
        if (pin == 17 || pin == 19 || pin == 20 || pin == 22 || pin == 23 || pin == 24 || pin == 25 || pin == 9 || pin == 10 ||
            pin == PIN_BUTTON1 || pin == PIN_BUTTON2) {
            continue;
        }
        digitalWrite(pin, LOW);
    }
    for (int pin = 0; pin < 48; pin++) {
        if (pin == 17 || pin == 19 || pin == 20 || pin == 22 || pin == 23 || pin == 24 || pin == 25 || pin == 9 || pin == 10 ||
            pin == PIN_BUTTON1 || pin == PIN_BUTTON2) {
            continue;
        }
        NRF_GPIO->DIRCLR = (1 << pin);
    }
#endif

    // Sleepy trackers or sensors can low power "sleep"
    // Don't enter this if we're sleeping portMAX_DELAY, since that's a shutdown event
    if (msecToWake != portMAX_DELAY &&
        (IS_ONE_OF(config.device.role, water_sensor_mesh_Config_DeviceConfig_Role_TRACKER,
                   water_sensor_mesh_Config_DeviceConfig_Role_TAK_TRACKER, water_sensor_mesh_Config_DeviceConfig_Role_SENSOR) &&
         config.power.is_power_saving == true)) {
        sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
        delay(msecToWake);
        NVIC_SystemReset();
    } else {
        // Resume on user button press
        // https://github.com/lyusupov/SoftRF/blob/81c519ca75693b696752235d559e881f2e0511ee/software/firmware/source/SoftRF/src/platform/nRF52.cpp#L1738
        constexpr uint32_t DFU_MAGIC_SKIP = 0x6d;
        sd_power_gpregret_set(0, DFU_MAGIC_SKIP); // Equivalent NRF_POWER->GPREGRET = DFU_MAGIC_SKIP

        // FIXME, use system off mode with ram retention for key state?
        // FIXME, use non-init RAM per
        // https://devzone.nordicsemi.com/f/nordic-q-a/48919/ram-retention-settings-with-softdevice-enabled

#ifdef ELECROW_ThinkNode_M1
        nrf_gpio_cfg_input(PIN_BUTTON1, NRF_GPIO_PIN_PULLUP); // Configure the pin to be woken up as an input
        nrf_gpio_pin_sense_t sense = NRF_GPIO_PIN_SENSE_LOW;
        nrf_gpio_cfg_sense_set(PIN_BUTTON1, sense);

        nrf_gpio_cfg_input(PIN_BUTTON2, NRF_GPIO_PIN_PULLUP);
        nrf_gpio_pin_sense_t sense1 = NRF_GPIO_PIN_SENSE_LOW;
        nrf_gpio_cfg_sense_set(PIN_BUTTON2, sense1);
#endif

        auto ok = sd_power_system_off();
        if (ok != NRF_SUCCESS) {
            LOG_ERROR("FIXME: Ignoring soft device (EasyDMA pending?) and forcing system-off!");
            NRF_POWER->SYSTEMOFF = 1;
        }
    }

    // The following code should not be run, because we are off
    while (1) {
        delay(5000);
        LOG_DEBUG(".");
    }
}

void clearBonds()
{
    if (!nrf52Bluetooth) {
        nrf52Bluetooth = new NRF52Bluetooth();
        nrf52Bluetooth->setup();
    }
    nrf52Bluetooth->clearBonds();
}

void enterDfuMode()
{
// SDK kit does not have native USB like almost all other NRF52 boards
#ifdef NRF_USE_SERIAL_DFU
    enterSerialDfu();
#else
    enterUf2Dfu();
#endif
}