#include "Channels.h"

#include "CryptoEngine.h"
#include "Default.h"
#include "DisplayFormatters.h"
#include "NodeDB.h"
#include "RadioInterface.h"
#include "configuration.h"

#include <assert.h>

#if !WATER_SENSOR_MESH_EXCLUDE_MQTT
#include "mqtt/MQTT.h"
#endif

Channels channels;

const char *Channels::adminChannel = "admin";
const char *Channels::gpioChannel = "gpio";
const char *Channels::serialChannel = "serial";
#if !WATER_SENSOR_MESH_EXCLUDE_MQTT
const char *Channels::mqttChannel = "mqtt";
#endif

uint8_t xorHash(const uint8_t *p, size_t len)
{
    uint8_t code = 0;
    for (size_t i = 0; i < len; i++)
        code ^= p[i];
    return code;
}

/** Given a channel number, return the (0 to 255) hash for that channel.
 * The hash is just an xor of the channel name followed by the channel PSK being used for encryption
 * If no suitable channel could be found, return -1
 */
int16_t Channels::generateHash(ChannelIndex channelNum)
{
    auto k = getKey(channelNum);
    if (k.length < 0)
        return -1; // invalid
    else {
        const char *name = getName(channelNum);
        uint8_t h = xorHash((const uint8_t *)name, strlen(name));

        h ^= xorHash(k.bytes, k.length);

        return h;
    }
}

/**
 * Validate a channel, fixing any errors as needed
 */
water_sensor_mesh_Channel &Channels::fixupChannel(ChannelIndex chIndex)
{
    water_sensor_mesh_Channel &ch = getByIndex(chIndex);

    ch.index = chIndex; // Preinit the index so it be ready to share with the phone (we'll never change it later)

    if (!ch.has_settings) {
        // No settings! Must disable and skip
        ch.role = water_sensor_mesh_Channel_Role_DISABLED;
        memset(&ch.settings, 0, sizeof(ch.settings));
        ch.has_settings = true;
    } else {
        water_sensor_mesh_ChannelSettings &water_sensor_mesh_channelSettings = ch.settings;

        // Convert the old string "Default" to our new short representation
        if (strcmp(water_sensor_mesh_channelSettings.name, "Default") == 0)
            *water_sensor_mesh_channelSettings.name = '\0';
    }

    hashes[chIndex] = generateHash(chIndex);

    return ch;
}

void Channels::initDefaultLoraConfig()
{
    water_sensor_mesh_Config_LoRaConfig &loraConfig = config.lora;

    loraConfig.modem_preset = water_sensor_mesh_Config_LoRaConfig_ModemPreset_LONG_FAST; // Default to Long Range & Fast
    loraConfig.use_preset = true;
    loraConfig.tx_power = 0; // default
    loraConfig.channel_num = 0;

#ifdef USERPREFS_LORACONFIG_MODEM_PRESET
    loraConfig.modem_preset = USERPREFS_LORACONFIG_MODEM_PRESET;
#endif
#ifdef USERPREFS_LORACONFIG_CHANNEL_NUM
    loraConfig.channel_num = USERPREFS_LORACONFIG_CHANNEL_NUM;
#endif
}

bool Channels::ensureLicensedOperation()
{
    if (!owner.is_licensed) {
        return false;
    }
    bool hasEncryptionOrAdmin = false;
    for (uint8_t i = 0; i < MAX_NUM_CHANNELS; i++) {
        auto channel = channels.getByIndex(i);
        if (!channel.has_settings) {
            continue;
        }
        auto &channelSettings = channel.settings;
        if (strcasecmp(channelSettings.name, Channels::adminChannel) == 0) {
            channel.role = water_sensor_mesh_Channel_Role_DISABLED;
            channelSettings.psk.bytes[0] = 0;
            channelSettings.psk.size = 0;
            hasEncryptionOrAdmin = true;
            channels.setChannel(channel);

        } else if (channelSettings.psk.size > 0) {
            channelSettings.psk.bytes[0] = 0;
            channelSettings.psk.size = 0;
            hasEncryptionOrAdmin = true;
            channels.setChannel(channel);
        }
    }
    return hasEncryptionOrAdmin;
}

/**
 * Write a default channel to the specified channel index
 */
void Channels::initDefaultChannel(ChannelIndex chIndex)
{
    water_sensor_mesh_Channel &ch = getByIndex(chIndex);
    water_sensor_mesh_ChannelSettings &channelSettings = ch.settings;

    uint8_t defaultpskIndex = 1;
    channelSettings.psk.bytes[0] = defaultpskIndex;
    channelSettings.psk.size = 1;
    strncpy(channelSettings.name, "", sizeof(channelSettings.name));
    channelSettings.module_settings.position_precision = 13; // default to sending location on the primary channel
    channelSettings.has_module_settings = true;

    ch.has_settings = true;
    ch.role = chIndex == 0 ? water_sensor_mesh_Channel_Role_PRIMARY : water_sensor_mesh_Channel_Role_SECONDARY;

    switch (chIndex) {
    case 0:
#ifdef USERPREFS_CHANNEL_0_PSK
        static const uint8_t defaultpsk0[] = USERPREFS_CHANNEL_0_PSK;
        memcpy(channelSettings.psk.bytes, defaultpsk0, sizeof(defaultpsk0));
        channelSettings.psk.size = sizeof(defaultpsk0);
#endif
#ifdef USERPREFS_CHANNEL_0_NAME
        strcpy(channelSettings.name, (const char *)USERPREFS_CHANNEL_0_NAME);
#endif
#ifdef USERPREFS_CHANNEL_0_PRECISION
        channelSettings.module_settings.position_precision = USERPREFS_CHANNEL_0_PRECISION;
#endif
#ifdef USERPREFS_CHANNEL_0_UPLINK_ENABLED
        channelSettings.uplink_enabled = USERPREFS_CHANNEL_0_UPLINK_ENABLED;
#endif
#ifdef USERPREFS_CHANNEL_0_DOWNLINK_ENABLED
        channelSettings.downlink_enabled = USERPREFS_CHANNEL_0_DOWNLINK_ENABLED;
#endif
        break;
    case 1:
#ifdef USERPREFS_CHANNEL_1_PSK
        static const uint8_t defaultpsk1[] = USERPREFS_CHANNEL_1_PSK;
        memcpy(channelSettings.psk.bytes, defaultpsk1, sizeof(defaultpsk1));
        channelSettings.psk.size = sizeof(defaultpsk1);
#endif
#ifdef USERPREFS_CHANNEL_1_NAME
        strcpy(channelSettings.name, (const char *)USERPREFS_CHANNEL_1_NAME);
#endif
#ifdef USERPREFS_CHANNEL_1_PRECISION
        channelSettings.module_settings.position_precision = USERPREFS_CHANNEL_1_PRECISION;
#endif
#ifdef USERPREFS_CHANNEL_1_UPLINK_ENABLED
        channelSettings.uplink_enabled = USERPREFS_CHANNEL_1_UPLINK_ENABLED;
#endif
#ifdef USERPREFS_CHANNEL_1_DOWNLINK_ENABLED
        channelSettings.downlink_enabled = USERPREFS_CHANNEL_1_DOWNLINK_ENABLED;
#endif
        break;
    case 2:
#ifdef USERPREFS_CHANNEL_2_PSK
        static const uint8_t defaultpsk2[] = USERPREFS_CHANNEL_2_PSK;
        memcpy(channelSettings.psk.bytes, defaultpsk2, sizeof(defaultpsk2));
        channelSettings.psk.size = sizeof(defaultpsk2);
#endif
#ifdef USERPREFS_CHANNEL_2_NAME
        strcpy(channelSettings.name, (const char *)USERPREFS_CHANNEL_2_NAME);
#endif
#ifdef USERPREFS_CHANNEL_2_PRECISION
        channelSettings.module_settings.position_precision = USERPREFS_CHANNEL_2_PRECISION;
#endif
#ifdef USERPREFS_CHANNEL_2_UPLINK_ENABLED
        channelSettings.uplink_enabled = USERPREFS_CHANNEL_2_UPLINK_ENABLED;
#endif
#ifdef USERPREFS_CHANNEL_2_DOWNLINK_ENABLED
        channelSettings.downlink_enabled = USERPREFS_CHANNEL_2_DOWNLINK_ENABLED;
#endif
        break;
    default:
        break;
    }
}

CryptoKey Channels::getKey(ChannelIndex chIndex)
{
    water_sensor_mesh_Channel &ch = getByIndex(chIndex);
    const water_sensor_mesh_ChannelSettings &channelSettings = ch.settings;

    CryptoKey k;
    memset(k.bytes, 0, sizeof(k.bytes)); // In case the user provided a short key, we want to pad the rest with zeros

    if (!ch.has_settings || ch.role == water_sensor_mesh_Channel_Role_DISABLED) {
        k.length = -1; // invalid
    } else {
        memcpy(k.bytes, channelSettings.psk.bytes, channelSettings.psk.size);
        k.length = channelSettings.psk.size;
        if (k.length == 0) {
            if (ch.role == water_sensor_mesh_Channel_Role_SECONDARY) {
                LOG_DEBUG("Unset PSK for secondary channel %s. use primary key", ch.settings.name);
                k = getKey(primaryIndex);
            } else {
                LOG_WARN("User disabled encryption");
            }
        } else if (k.length == 1) {
            // Convert the short single byte variants of psk into variant that can be used more generally

            uint8_t pskIndex = k.bytes[0];
            LOG_DEBUG("Expand short PSK #%d", pskIndex);
            if (pskIndex == 0)
                k.length = 0; // Turn off encryption
            else {
                memcpy(k.bytes, defaultpsk, sizeof(defaultpsk));
                k.length = sizeof(defaultpsk);
                // Bump up the last byte of PSK as needed
                uint8_t *last = k.bytes + sizeof(defaultpsk) - 1;
                *last = *last + pskIndex - 1; // index of 1 means no change vs defaultPSK
            }
        } else if (k.length < 16) {
            // Error! The user specified only the first few bits of an AES128 key.  So by convention we just pad the rest of the
            // key with zeros
            LOG_WARN("User provided a too short AES128 key - padding");
            k.length = 16;
        } else if (k.length < 32 && k.length != 16) {
            // Error! The user specified only the first few bits of an AES256 key.  So by convention we just pad the rest of the
            // key with zeros
            LOG_WARN("User provided a too short AES256 key - padding");
            k.length = 32;
        }
    }

    return k;
}

/** Given a channel index, change to use the crypto key specified by that index
 */
int16_t Channels::setCrypto(ChannelIndex chIndex)
{
    CryptoKey k = getKey(chIndex);

    if (k.length < 0)
        return -1;
    else {
        // Tell our crypto engine about the psk
        crypto->setKey(k);
        return getHash(chIndex);
    }
}

void Channels::initDefaults()
{
    channelFile.channels_count = MAX_NUM_CHANNELS;
    for (int i = 0; i < channelFile.channels_count; i++)
        fixupChannel(i);
    initDefaultLoraConfig();

#ifdef USERPREFS_CHANNELS_TO_WRITE
    for (int i = 0; i < USERPREFS_CHANNELS_TO_WRITE; i++) {
        initDefaultChannel(i);
    }
#else
    initDefaultChannel(0);
#endif
}

void Channels::onConfigChanged()
{
    // Make sure the phone hasn't mucked anything up
    for (int i = 0; i < channelFile.channels_count; i++) {
        const water_sensor_mesh_Channel &ch = fixupChannel(i);

        if (ch.role == water_sensor_mesh_Channel_Role_PRIMARY)
            primaryIndex = i;
    }
#if !WATER_SENSOR_MESH_EXCLUDE_MQTT
    if (channels.anyMqttEnabled() && mqtt && !mqtt->isEnabled()) {
        LOG_DEBUG("MQTT is enabled on at least one channel, so set MQTT thread to run immediately");
        mqtt->start();
    }
#endif
}

water_sensor_mesh_Channel &Channels::getByIndex(ChannelIndex chIndex)
{
    // remove this assert cause malformed packets can make our firmware reboot here.
    if (chIndex < channelFile.channels_count) { // This should be equal to MAX_NUM_CHANNELS
        water_sensor_mesh_Channel *ch = channelFile.channels + chIndex;
        return *ch;
    } else {
        LOG_ERROR("Invalid channel index %d > %d, malformed packet received?", chIndex, channelFile.channels_count);

        static water_sensor_mesh_Channel *ch = (water_sensor_mesh_Channel *)malloc(sizeof(water_sensor_mesh_Channel));
        memset(ch, 0, sizeof(water_sensor_mesh_Channel));
        // ch.index -1 means we don't know the channel locally and need to look it up by settings.name
        // not sure this is handled right everywhere
        ch->index = -1;
        return *ch;
    }
}

water_sensor_mesh_Channel &Channels::getByName(const char *chName)
{
    for (ChannelIndex i = 0; i < getNumChannels(); i++) {
        if (strcasecmp(getGlobalId(i), chName) == 0) {
            return channelFile.channels[i];
        }
    }

    return getByIndex(getPrimaryIndex());
}

void Channels::setChannel(const water_sensor_mesh_Channel &c)
{
    water_sensor_mesh_Channel &old = getByIndex(c.index);

    // if this is the new primary, demote any existing roles
    if (c.role == water_sensor_mesh_Channel_Role_PRIMARY)
        for (int i = 0; i < getNumChannels(); i++)
            if (channelFile.channels[i].role == water_sensor_mesh_Channel_Role_PRIMARY)
                channelFile.channels[i].role = water_sensor_mesh_Channel_Role_SECONDARY;

    old = c; // slam in the new settings/role
}

bool Channels::anyMqttEnabled()
{
#if USERPREFS_EVENT_MODE && !WATER_SENSOR_MESH_EXCLUDE_MQTT
    // Don't publish messages on the public MQTT broker if we are in event mode
    if (mqtt && mqtt->isUsingDefaultServer() && mqtt->isUsingDefaultRootTopic()) {
        return false;
    }
#endif
    for (int i = 0; i < getNumChannels(); i++)
        if (channelFile.channels[i].role != water_sensor_mesh_Channel_Role_DISABLED && channelFile.channels[i].has_settings &&
            (channelFile.channels[i].settings.downlink_enabled || channelFile.channels[i].settings.uplink_enabled))
            return true;

    return false;
}

const char *Channels::getName(size_t chIndex)
{
    // Convert the short "" representation for Default into a usable string
    const water_sensor_mesh_ChannelSettings &channelSettings = getByIndex(chIndex).settings;
    const char *channelName = channelSettings.name;
    if (!*channelName) { // emptystring
        // Per mesh.proto spec, if bandwidth is specified we must ignore modemPreset enum, we assume that in that case
        // the app effed up and forgot to set channelSettings.name
        if (config.lora.use_preset) {
            channelName = DisplayFormatters::getModemPresetDisplayName(config.lora.modem_preset, false);
        } else {
            channelName = "Custom";
        }
    }

    return channelName;
}

bool Channels::isDefaultChannel(ChannelIndex chIndex)
{
    const auto &ch = getByIndex(chIndex);
    if (ch.settings.psk.size == 1 && ch.settings.psk.bytes[0] == 1) {
        const char *name = getName(chIndex);
        const char *presetName = DisplayFormatters::getModemPresetDisplayName(config.lora.modem_preset, false);
        // Check if the name is the default derived from the modem preset
        if (strcmp(name, presetName) == 0)
            return true;
    }
    return false;
}

bool Channels::hasDefaultChannel()
{
    // If we don't use a preset or the default frequency slot, or we override the frequency, we don't have a default channel
    if (!config.lora.use_preset || !RadioInterface::uses_default_frequency_slot || config.lora.override_frequency)
        return false;
    // Check if any of the channels are using the default name and PSK
    for (size_t i = 0; i < getNumChannels(); i++) {
        if (isDefaultChannel(i))
            return true;
    }
    return false;
}

/** Given a channel hash setup crypto for decoding that channel (or the primary channel if that channel is unsecured)
 *
 * This method is called before decoding inbound packets
 *
 * @return false if the channel hash or channel is invalid
 */
bool Channels::decryptForHash(ChannelIndex chIndex, ChannelHash channelHash)
{
    if (chIndex > getNumChannels() || getHash(chIndex) != channelHash) {
        // LOG_DEBUG("Skip channel %d (hash %x) due to invalid hash/index, want=%x", chIndex, getHash(chIndex),
        // channelHash);
        return false;
    } else {
        LOG_DEBUG("Use channel %d (hash 0x%x)", chIndex, channelHash);
        setCrypto(chIndex);
        return true;
    }
}

/** Given a channel index setup crypto for encoding that channel (or the primary channel if that channel is unsecured)
 *
 * This method is called before encoding outbound packets
 *
 * @return the (0 to 255) hash for that channel - if no suitable channel could be found, return -1
 */
int16_t Channels::setActiveByIndex(ChannelIndex channelIndex)
{
    return setCrypto(channelIndex);
}