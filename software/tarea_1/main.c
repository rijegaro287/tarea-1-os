#include "stdlib.h"

#include "sys/alt_irq.h"

#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"

#include "system.h"

#define MODE_MSK 0b011
#define STOP_MSK 0b100

/*
* PIO_SWITCHES_0_BASE[1:0] -> mode
* PIO_SWITCHES_0_BASE[2] -> stop
*/
unsigned mode = 0;
unsigned stop = 0;

unsigned minutes = 0;
unsigned seconds = 0;
unsigned millis = 0;

unsigned display_5;
unsigned display_4;
unsigned display_3;
unsigned display_2;
unsigned display_1;
unsigned display_0;

void set_time() {
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
}

void set_mode() {
  switch (mode) {
  case 0b00:
    display_5 = 0;
    display_4 = 0;
    display_3 = 0;
    display_2 = (millis / 100) % 10;
    display_1 = (millis / 10) % 10;
    display_0 = millis % 10;
    break;

  case 0b01:
    display_5 = 0;
    display_4 = 0;
    display_3 = 0;
    display_2 = 0;
    display_1 = seconds / 10;
    display_0 = seconds % 10;
    break;

  default:
    display_5 = minutes / 10;
    display_4 = minutes % 10;

    display_3 = seconds / 10;
    display_2 = seconds % 10;

    display_1 = (millis / 100) % 10;
    display_0 = (millis / 10) % 10;
    break;
  }
}

void set_displays() {
  IOWR_ALTERA_AVALON_PIO_DATA(DISPLAY_5_BASE, display_5);
  IOWR_ALTERA_AVALON_PIO_DATA(DISPLAY_4_BASE, display_4);

  IOWR_ALTERA_AVALON_PIO_DATA(DISPLAY_3_BASE, display_3);
  IOWR_ALTERA_AVALON_PIO_DATA(DISPLAY_2_BASE, display_2);

  IOWR_ALTERA_AVALON_PIO_DATA(DISPLAY_1_BASE, display_1);
  IOWR_ALTERA_AVALON_PIO_DATA(DISPLAY_0_BASE, display_0);
}

void timer_isr(void* context) {
  (void)context;

  set_time();
  set_mode();
  set_displays();

  IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);
}

int main()
{
  alt_ic_isr_register(
    TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID,
    TIMER_0_IRQ,
    timer_isr,
    NULL,
    NULL
  );

  IOWR_ALTERA_AVALON_TIMER_CONTROL(
    TIMER_0_BASE,
    ALTERA_AVALON_TIMER_CONTROL_ITO_MSK
    | ALTERA_AVALON_TIMER_CONTROL_CONT_MSK);

  /* Event loop never exits. */
  while (1) {
    mode = IORD_ALTERA_AVALON_PIO_DATA(SWITCHES_0_BASE) & MODE_MSK;
    stop = IORD_ALTERA_AVALON_PIO_DATA(SWITCHES_0_BASE) & STOP_MSK;

    if (stop) {
      IOWR_ALTERA_AVALON_TIMER_CONTROL(
        TIMER_0_BASE,
        ALTERA_AVALON_TIMER_CONTROL_STOP_MSK
        | ALTERA_AVALON_TIMER_CONTROL_ITO_MSK
        | ALTERA_AVALON_TIMER_CONTROL_CONT_MSK
      );
    }
    else {
      IOWR_ALTERA_AVALON_TIMER_CONTROL(
        TIMER_0_BASE,
        ALTERA_AVALON_TIMER_CONTROL_START_MSK
        | ALTERA_AVALON_TIMER_CONTROL_ITO_MSK
        | ALTERA_AVALON_TIMER_CONTROL_CONT_MSK
      );
    }
  }

  return 0;
}