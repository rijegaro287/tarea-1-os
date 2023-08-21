#include "stdlib.h"

#include "sys/alt_irq.h"

#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"

#include "system.h"

/*
* PIO_SWITCHES_0_BASE[1:0] -> mode
* PIO_SWITCHES_0_BASE[2] -> stop
*/
unsigned mode = 0;
unsigned stop = 0;

unsigned minutes = 0;
unsigned seconds = 0;
unsigned millis = 0;

void set_displays() {
  unsigned minutes_display_1 = minutes / 10;
  unsigned minutes_display_0 = minutes % 10;

  unsigned seconds_display_1 = seconds / 10;
  unsigned seconds_display_0 = seconds % 10;

  unsigned millis_display_1 = (millis / 100) % 10;
  unsigned millis_display_0 = (millis / 10) % 10;

  IOWR_ALTERA_AVALON_PIO_DATA(MINUTES_DISPLAY_1_BASE, minutes_display_1);
  IOWR_ALTERA_AVALON_PIO_DATA(MINUTES_DISPLAY_0_BASE, minutes_display_0);

  IOWR_ALTERA_AVALON_PIO_DATA(SECONDS_DISPLAY_1_BASE, seconds_display_1);
  IOWR_ALTERA_AVALON_PIO_DATA(SECONDS_DISPLAY_0_BASE, seconds_display_0);

  IOWR_ALTERA_AVALON_PIO_DATA(MILLIS_DISPLAY_1_BASE, millis_display_1);
  IOWR_ALTERA_AVALON_PIO_DATA(MILLIS_DISPLAY_0_BASE, millis_display_0);
}

void set_timer(void* context) {
  (void)context;

  millis++;

  if (millis == 1000) {
    millis = 0;
    seconds++;
  }

  if (seconds == 60) {
    seconds = 0;
    minutes++;
  }

  if (minutes == 99) {
    minutes = 0;
  }

  set_displays();

  IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);
}

// static void stop_timer() {
//   const unsigned timer_control = IORD_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE)
//     | ALTERA_AVALON_TIMER_CONTROL_STOP_MSK;

//   IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, timer_control);
// }

// static void start_timer() {
//   const unsigned timer_control = IORD_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE)
//     | ALTERA_AVALON_TIMER_CONTROL_START_MSK;

//   IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, timer_control);
// }

int main()
{
  // mode = IORD_ALTERA_AVALON_PIO_DATA(SWITCHES_0_BASE) & 0b011;
  // stop = IORD_ALTERA_AVALON_PIO_DATA(SWITCHES_0_BASE) & 0b100;

  // if (stop) {
  //   stop_timer();
  // }
  // else {
  //   start_timer();
  // }

  // switch (mode) {
  // case 0:
  //   /* code */
  //   break;
  // case 1:
  //   /* code */
  //   break;
  // case 2:
  //   /* code */
  //   break;

  // default:
  //   break;
  // }

  alt_ic_isr_register(
    TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID,
    TIMER_0_IRQ,
    set_timer,
    NULL,
    NULL
  );

  IOWR_ALTERA_AVALON_TIMER_CONTROL(
    TIMER_0_BASE,
    ALTERA_AVALON_TIMER_CONTROL_ITO_MSK
    | ALTERA_AVALON_TIMER_CONTROL_CONT_MSK);

  /* Event loop never exits. */
  while (1);

  return 0;
}