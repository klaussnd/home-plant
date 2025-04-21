#include "../utils/clock.h"
#include "../utils/settings.h"
#include "acquire.h"

#include <can.h>
#include <can_message/content_plant.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef DEBUG
#include <hal/usart.h>
#include <hal/usart_stdout.h>
#endif

const uint8_t CAN_FILTER[] PROGMEM = {
   // Group 0
   MCP2515_FILTER_EXTENDED(CanMsg::PLANT_MEAS_SETTINGS.type),  // Filter 0
   MCP2515_FILTER_EXTENDED(CanMsg::PLANT_MEAS_SETTINGS.type),  // Filter 1

   // Group 1
   MCP2515_FILTER_EXTENDED(CanMsg::PLANT_MEAS_SETTINGS.type),  // Filter 2
   MCP2515_FILTER_EXTENDED(CanMsg::PLANT_MEAS_SETTINGS.type),  // Filter 3
   MCP2515_FILTER_EXTENDED(CanMsg::PLANT_MEAS_SETTINGS.type),  // Filter 4
   MCP2515_FILTER_EXTENDED(CanMsg::PLANT_MEAS_SETTINGS.type),  // Filter 5

   MCP2515_FILTER_EXTENDED(0xffff00),  // Mask 0 (for group 0)
   MCP2515_FILTER_EXTENDED(0xffff00),  // Mask 1 (for group 1)
};

constexpr uint8_t CHANNEL_COUNT = 4;

void sendValue(uint8_t channel, uint16_t value);

int main()
{
   DDRB = 0b00101101;  // SS(0), TWI-SS(2), MOSI(3), SCK(5)
   PORTB = 0;

   clockInit();
   initAcquisition();
#ifdef DEBUG
   usartInit<4800>();
#endif

   sei();

#ifdef DEBUG
   const bool init_ok =
#endif
      can_init(BITRATE_125_KBPS);
   can_static_filter(CAN_FILTER);

#ifdef DEBUG
   {
      const uint16_t comp_match = OCR1A;
      fprintf_P(usart_stdout, PSTR("Init done: CAN %d, COMP1A %u\n"), init_ok,
                (unsigned int)comp_match);
   }
#endif

   auto settings = readInitialSettingsFromEeprom();
   setTime(settings.interval);

   while (true)
   {
      const auto current_time = getTime();
#ifdef DEBUG
      fprintf_P(usart_stdout, PSTR("current time %d interval %d\n"), current_time,
                settings.interval);
#endif
      if (current_time > settings.interval)
      {
#ifdef DEBUG
         fprintf_P(usart_stdout, PSTR("Start measurement, enabled channels %hu\n"),
                   settings.enabled_channels);
#endif
         setTime(0);

         enableAnalogue();
         _delay_ms(250);

         for (uint8_t channel = 0; channel < CHANNEL_COUNT; ++channel)
         {
            if (settings.enabled_channels & (1 << channel))
            {
               const uint16_t raw_value = acquireRaw(channel);
               sendValue(channel, raw_value);
#ifdef DEBUG
               fprintf_P(usart_stdout, PSTR("ch %d raw %d\n"), channel, raw_value);
#endif
            }
         }
         disableAnalogue();
      }
#ifdef DEBUG
      _delay_ms(100);
#endif
      set_sleep_mode(SLEEP_MODE_IDLE);
      sleep_mode();
   }

   return 0;
}

void sendValue(uint8_t channel, uint16_t value)
{
   CanMsg::Tensiometer tensio{channel, value};
   const auto msg = CanMsg::makeTensiometer(tensio, MY_DEVICE_ID);
   can_send_message(&msg);
}
