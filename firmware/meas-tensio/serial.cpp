#include "acquire.h"

#include <hal/usart.h>
#include <hal/usart_stdout.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
   initAcquisition();
   usartInit<4800>();
   sei();

   bool do_acquisition = false;
   while (1)
   {
      if (usartIsLineAvailableToRead())
      {
         char buf[16];

         usartReadLine(buf, sizeof(buf));

         switch (*buf)
         {
         case '?':
            usartWriteString_P(PSTR("e           enable analogue\n"
                                    "d           disable analogue\n"
                                    "c <channel> select MUX (channel 0 ... 3)\n"
                                    "s           start/stop acquisition\n"));
            break;
         case 'e':
            usartWriteString_P(PSTR("Enable analogue circuit\n"));
            enableAnalogue();
            break;

         case 'd':
            usartWriteString_P(PSTR("Disable analogue circuit\n"));
            muxDeselectAllChannels();
            disableAnalogue();
            break;
         case 'c':
         {
            const uint8_t channel = atoi(buf + 1);
            muxSelectChannel(channel);
            fprintf_P(usart_stdout, PSTR("Selected channel %d\n"), channel);
            break;
         }
         case 's':
            do_acquisition = !do_acquisition;
            fprintf_P(usart_stdout, PSTR("Acquisition %s\n"),
                      do_acquisition ? "on" : "off");
            break;
         }
      }

      if (do_acquisition)
      {
         const uint16_t value = readout();
         fprintf_P(usart_stdout, PSTR("Val: %d\n"), value);
      }

      _delay_ms(500);
   }
   return 0;
}
