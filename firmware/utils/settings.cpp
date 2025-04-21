#include "settings.h"

#include <avr/eeprom.h>
#include <avr/pgmspace.h>

uint8_t eeprom[3] EEMEM;

const uint16_t DEFAULT_MEASURE_INTERVAL PROGMEM = 60;  // seconds
const uint8_t DEFAULT_ENABLED_CHANNELS PROGMEM = 0x03;
const uint16_t UNINITIALISED_EEPROM_VALUE PROGMEM = 0xffff;

CanMsg::PlantMeasSettings readInitialSettingsFromEeprom()
{
   CanMsg::PlantMeasSettings settings;
   settings.interval = eeprom_read_word(reinterpret_cast<uint16_t*>(eeprom));  // seconds
   settings.enabled_channels = eeprom_read_byte(eeprom + 2);

   if (settings.interval == pgm_read_word(&UNINITIALISED_EEPROM_VALUE))
   {
      settings.interval = pgm_read_word(&DEFAULT_MEASURE_INTERVAL);
      settings.enabled_channels = pgm_read_byte(&DEFAULT_ENABLED_CHANNELS);
   }

   return settings;
}

void saveSettingsToEeprom(const CanMsg::PlantMeasSettings& settings)
{
   eeprom_write_word(reinterpret_cast<uint16_t*>(eeprom), settings.interval);
   eeprom_write_byte(eeprom + 2, settings.enabled_channels);
}

void handleSettings(const CanMessage& msg, CanMsg::PlantMeasSettings& settings)
{
   if (isMatchingCanId(CanMsg::PLANT_MEAS_SETTINGS, MY_DEVICE_ID, msg.id))
   {
      if (msg.flags.rtr)
      {
         auto reply = CanMsg::makePlantSettings(settings, MY_DEVICE_ID);
         can_send_message(&reply);
      }
      else if (msg.length == CanMsg::PLANT_MEAS_SETTINGS.length
               && getSender(CanMsg::PLANT_MEAS_SETTINGS, msg.id) == MY_DEVICE_ID)
      {
         settings = CanMsg::readPlantSettings(msg);
         settings.enabled_channels &= 0x0f;  // only 4 channels
         saveSettingsToEeprom(settings);
#ifdef DEBUG
         fprintf_P(usart_stdout, PSTR("New meas interval %u channels %x\n"),
                   settings.interval, settings.enabled_channels);
#endif
      }
   }
}
