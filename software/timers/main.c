/*
 * "Small Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It requires a STDOUT  device in your system's hardware.
 *
 * The purpose of this example is to demonstrate the smallest possible Hello
 * World application, using the Nios II HAL library.  The memory footprint
 * of this hosted application is ~332 bytes by default using the standard
 * reference design.  For a more fully featured Hello World application
 * example, see the example titled "Hello World".
 *
 * The memory footprint of this example has been reduced by making the
 * following changes to the normal "Hello World" example.
 * Check in the Nios II Software Developers Manual for a more complete
 * description.
 *
 * In the SW Application project (small_hello_world):
 *
 *  - In the C/C++ Build page
 *
 *    - Set the Optimization Level to -Os
 *
 * In System Library project (small_hello_world_syslib):
 *  - In the C/C++ Build page
 *
 *    - Set the Optimization Level to -Os
 *
 *    - Define the preprocessor option ALT_NO_INSTRUCTION_EMULATION
 *      This removes software exception handling, which means that you cannot
 *      run code compiled for Nios II cpu with a hardware multiplier on a core
 *      without a the multiply unit. Check the Nios II Software Developers
 *      Manual for more details.
 *
 *  - In the System Library page:
 *    - Set Periodic system timer and Timestamp timer to none
 *      This prevents the automatic inclusion of the timer driver.
 *
 *    - Set Max file descriptors to 4
 *      This reduces the size of the file handle pool.
 *
 *    - Check Main function does not exit
 *    - Uncheck Clean exit (flush buffers)
 *      This removes the unneeded call to exit when main returns, since it
 *      won't.
 *
 *    - Check Don't use C++
 *      This builds without the C++ support code.
 *
 *    - Check Small C library
 *      This uses a reduced functionality C library, which lacks
 *      support for buffering, file IO, floating point and getch(), etc.
 *      Check the Nios II Software Developers Manual for a complete list.
 *
 *    - Check Reduced device drivers
 *      This uses reduced functionality drivers if they're available. For the
 *      standard design this means you get polled UART and JTAG UART drivers,
 *      no support for the LCD driver and you lose the ability to program
 *      CFI compliant flash devices.
 *
 *    - Check Access device drivers directly
 *      This bypasses the device file system to access device drivers directly.
 *      This eliminates the space required for the device file system services.
 *      It also provides a HAL version of libc services that access the drivers
 *      directly, further reducing space. Only a limited number of libc
 *      functions are available in this configuration.
 *
 *    - Use ALT versions of stdio routines:
 *
 *           Function                  Description
 *        ===============  =====================================
 *        alt_printf       Only supports %s, %x, and %c ( < 1 Kbyte)
 *        alt_putstr       Smaller overhead than puts with direct drivers
 *                         Note this function doesn't add a newline.
 *        alt_putchar      Smaller overhead than putchar with direct drivers
 *        alt_getchar      Smaller overhead than getchar with direct drivers
 *
 */
#include "stdlib.h"
#include "stdbool.h"

#include "sys/alt_irq.h"

#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"

#include "system.h"

 /* Constants for displaying numbers in a 7-segment display */
#define DISPLAY_OFF 0b1111111
#define DISPLAY_0 0b1000000
#define DISPLAY_1 0b1111001
#define DISPLAY_2 0b0100100
#define DISPLAY_3 0b0110000
#define DISPLAY_4 0b0011001
#define DISPLAY_5 0b0010010
#define DISPLAY_6 0b0000010
#define DISPLAY_7 0b1111000
#define DISPLAY_8 0b0000000
#define DISPLAY_9 0b0010000

/*
* PIO_SWITCHES_0_BASE[1:0] -> mode
* PIO_SWITCHES_0_BASE[2] -> stop
*/
static unsigned mode = 0;
static bool stop = false;

static unsigned minutes = 0;
static unsigned seconds = 0;
static unsigned millis = 0;

static unsigned minutes_display_0 = DISPLAY_OFF;
static unsigned minutes_display_1 = DISPLAY_OFF;

static unsigned seconds_display_0 = DISPLAY_OFF;
static unsigned seconds_display_1 = DISPLAY_OFF;

static unsigned millis_display_0 = DISPLAY_OFF;
static unsigned millis_display_1 = DISPLAY_OFF;
static unsigned millis_display_2 = DISPLAY_OFF;

static unsigned get_display_bits(unsigned value) {
  unsigned display_bits;

  switch (value) {
  case 0:
    display_bits = DISPLAY_0;
    break;

  case 1:
    display_bits = DISPLAY_1;
    break;

  case 2:
    display_bits = DISPLAY_2;
    break;

  case 4:
    display_bits = DISPLAY_4;
    break;

  case 5:
    display_bits = DISPLAY_5;
    break;

  case 6:
    display_bits = DISPLAY_6;
    break;

  case 7:
    display_bits = DISPLAY_7;
    break;

  case 8:
    display_bits = DISPLAY_8;
    break;

  case 9:
    display_bits = DISPLAY_9;
    break;

  default:
    display_bits = DISPLAY_OFF;
    break;
  }

  return display_bits;
}

static void set_displays() {
  minutes_display_0 = get_display_bits(minutes % 10);
  minutes_display_1 = get_display_bits(minutes / 10);

  seconds_display_0 = get_display_bits(seconds % 10);
  seconds_display_1 = get_display_bits(seconds / 10);

  millis_display_0 = get_display_bits(millis % 10);
  millis_display_1 = get_display_bits((millis / 10) % 10);
  millis_display_2 = get_display_bits((millis / 100) % 10);

  // IOWR_ALTERA_AVALON_PIO_DATA(MINUTES_DISPLAY_0_BASE, minutes_display_0);
  // IOWR_ALTERA_AVALON_PIO_DATA(MINUTES_DISPLAY_1_BASE, minutes_display_1);

  IOWR_ALTERA_AVALON_PIO_DATA(SECONDS_DISPLAY_0_BASE, seconds_display_0);
  IOWR_ALTERA_AVALON_PIO_DATA(SECONDS_DISPLAY_1_BASE, seconds_display_1);

  // IOWR_ALTERA_AVALON_PIO_DATA(SEGMENT_DISPLAY_4_BASE, millis_display_0);
  // IOWR_ALTERA_AVALON_PIO_DATA(SEGMENT_DISPLAY_5_BASE, millis_display_1);
  // IOWR_ALTERA_AVALON_PIO_DATA(SEGMENT_DISPLAY_6_BASE, millis_display_2);
}

static void stop_timer() {
  const unsigned timer_control = IORD_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE)
    & ALTERA_AVALON_TIMER_CONTROL_STOP_MSK;

  IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, timer_control);
}

static void start_timer() {
  const unsigned timer_control = IORD_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE)
    & ALTERA_AVALON_TIMER_CONTROL_START_MSK;

  IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, timer_control);
}

static void set_timer(void* context) {
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
}

int main()
{
  mode = IORD_ALTERA_AVALON_PIO_DATA(SWITCHES_0_BASE) & 0b0011;
  stop = IORD_ALTERA_AVALON_PIO_DATA(SWITCHES_0_BASE) & 0b0100;

  if (stop) {
    stop_timer();
  }
  else {
    start_timer();
  }

  switch (mode) {
  case 0:
    /* code */
    break;
  case 1:
    /* code */
    break;
  case 2:
    /* code */
    break;

  default:
    break;
  }

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
