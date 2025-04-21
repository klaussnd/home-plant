#include "acquire.h"

#include <hal/adc.h>
#include <hal/avr/pin_io.h>

#include <avr/sleep.h>
#include <util/delay.h>

// clang-format off
#define MUX_CHANNEL_PIN1  C,1
#define MUX_CHANNEL_PIN2  C,2
#define MUX_ENABLE_PIN    C,3
#define ANALOG_ENABLE_PIN C,4
// clang-format on

void initAcquisition()
{
   confPinAsOutput(ANALOG_ENABLE_PIN);
   confPinAsOutput(MUX_CHANNEL_PIN1);
   confPinAsOutput(MUX_CHANNEL_PIN2);
   confPinAsOutput(MUX_ENABLE_PIN);

   pinSetLow(MUX_ENABLE_PIN);
   disableAnalogue();

   adcSetReference(AdcReference::EXTERNAL_AREF);
}

uint16_t acquireRaw(const uint8_t channel)
{
   muxSelectChannel(channel);
   const auto value = readout();
   muxDeselectAllChannels();
   return value;
}

uint16_t readout()
{
   _delay_ms(500);                 /* wait for the measurement to settle */
   set_sleep_mode(SLEEP_MODE_ADC); /* to avoid hangup during ADC aquisition */
   constexpr uint8_t adc_channel = 7;
   constexpr uint8_t measurement_count = 5;
   return adcRead(adc_channel, measurement_count);
}

void enableAnalogue()
{
   pinSetLow(ANALOG_ENABLE_PIN);
}

void disableAnalogue()
{
   pinSetHigh(ANALOG_ENABLE_PIN);
}

void muxSelectChannel(uint8_t channel)
{
   const uint8_t mask = (channel & 0x03) << 1;
   PORTC = (PORTC & 0xf9) | mask;
   pinSetHigh(MUX_ENABLE_PIN);
}

void muxDeselectAllChannels()
{
   pinSetLow(MUX_ENABLE_PIN);
}
