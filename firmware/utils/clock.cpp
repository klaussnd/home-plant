#include "clock.h"

#include <hal/avr/timer.h>
#include <util/atomic.h>

namespace
{
volatile time_t m_time = 0;
}

void clockInit(void)
{
#define PRESCALER 64
#define TIMER_NO 1
   // Set CTC mode
   TCCR1A = 0;
   TCCR1B = (1 << WGM12);
   TMR_SET_PRESCALER(TIMER_NO, PRESCALER);
   TMR_SET_OCR2(TIMER_NO, A, TMR_MSEC2TIMER(1000, PRESCALER) - 1);
   TMR_SET_COMP_IRQ(TIMER_NO, A); /* enable interrupt for compare match A */
}

time_t getTime()
{
   time_t time;
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
   {
      time = m_time;
   }
   return time;
}

void setTime(time_t time)
{
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
   {
      m_time = time;
   }
}

TMR_COMP_ISR(1, A)
{
   ++m_time;
}
