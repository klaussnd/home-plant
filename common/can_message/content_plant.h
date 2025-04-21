#pragma once

#include <utils/can_message.h>

namespace CanMsg
{
// plant data -------------------------------------------------------------------------

// plant tensiometer measurement
// data: uint8 channel, uint16 value
constexpr CanId PLANT_MEAS_TENSIO = {0x700200, true, false, 3, 0};
// measurement settings
// data: uint16 interval
//       uint8  bitfield enabled channels
constexpr CanId PLANT_MEAS_SETTINGS = {0x700300, true, false, 3, 0};

// irrigation request
constexpr CanId IRRIGATION_REQUEST = {0x700000, true, false, 3, 0};
// irrigation acknowledgement
constexpr CanId IRRIGATION_ACK = {0x700100, true, false, 3, 0};

struct Tensiometer
{
   uint8_t channel;
   uint16_t potential;  // raw value
};

inline CanMessage makeTensiometer(const Tensiometer& value, uint8_t sender_device_id)
{
   auto msg =
      makeBasicCanMessage(CanMsg::PLANT_MEAS_TENSIO, sender_device_id, ANY_DEVICE_ID);
   static_assert(CanMsg::PLANT_MEAS_TENSIO.length == 3, "Inconsistent data length");
   msg.data[0] = value.channel;
   msg.data[1] = static_cast<uint8_t>(value.potential);
   msg.data[2] = static_cast<uint8_t>(value.potential >> 8);
   return msg;
}

inline Tensiometer readTensiometer(const CanMessage& msg)
{
   Tensiometer data;
   data.channel = msg.data[0];
   data.potential =
      static_cast<uint16_t>(msg.data[1]) | static_cast<uint16_t>(msg.data[2] << 8);
   return data;
}

struct PlantMeasSettings
{
   uint16_t interval;         // seconds
   uint8_t enabled_channels;  // bitfield
};

inline CanMessage makePlantSettings(const PlantMeasSettings& settings,
                                    uint8_t sender_device_id)
{
   auto msg =
      makeBasicCanMessage(CanMsg::PLANT_MEAS_SETTINGS, sender_device_id, ANY_DEVICE_ID);
   static_assert(CanMsg::PLANT_MEAS_SETTINGS.length == 3, "Inconsistent data length");
   msg.data[0] = static_cast<uint8_t>(settings.interval);
   msg.data[1] = static_cast<uint8_t>(settings.interval >> 8);
   msg.data[2] = settings.enabled_channels;
   return msg;
}

inline PlantMeasSettings readPlantSettings(const CanMessage& msg)
{
   PlantMeasSettings settings;
   settings.interval =
      static_cast<uint16_t>(msg.data[0]) | static_cast<uint16_t>(msg.data[1] << 8);
   settings.enabled_channels = msg.data[2];
   return settings;
}
}  // namespace CanMsg
