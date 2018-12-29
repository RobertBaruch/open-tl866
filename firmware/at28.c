#include <xc.h>

#include "system.h"
#include "at28.h"

#define AT28_ADDR_PINS 15
#define AT28_DATA_PINS 8

typedef struct at28_pin_info {
  const char addr[AT28_ADDR_PINS];
  const char data[AT28_DATA_PINS];
  const unsigned int ce;
  const unsigned int oe;
  const unsigned int we;
  const unsigned int gnd;
  const unsigned int vdd;
} at28_pin_info_t;

/*
 * AT28BV64 PLCC Pinout:
 *
 * Target   Chip  Dir ZIF
 * ------------------------
 * VCC      32    <-  40
 * GND      16    <-  16
 * /CE      23    <-  31
 * /OE      25    <-  33
 * /WE      31    <-  39
 * A0       11    <-  11
 * A1       10    <-  10
 * A2        9    <-   9
 * A3        8    <-   8
 * A4        7    <-   7
 * A5        6    <-   6
 * A6        5    <-   5
 * A7        4    <-   4
 * A8       29    <-  37
 * A9       28    <-  36
 * A10      24    <-  32
 * A11      27    <-  35
 * A12       3    <-   3
 * A13      30    <-  38            [AT28BV256 only]
 * A14       2    <-   2            [AT28BV256 only]
 * D0       13   <->  13
 * D1       14   <->  14
 * D2       15   <->  15
 * D3       18   <->  26
 * D4       19   <->  27
 * D5       20   <->  28
 * D6       21   <->  29
 * D7       22   <->  30
 */

const at28_pin_info_t at28_plcc_pins = {
    {11, 10, 9, 8, 7, 6, 5, 4, 29, 28, 24, 27, 3, 30, 2},  // addr
    {13, 14, 15, 18, 19, 20, 21, 22},                      // data
    23,                                                    // ce
    25,                                                    // oe
    31,                                                    // we
    16,                                                    // gnd
    32,                                                    // vdd
};

const at28_pin_info_t *at28_pins = &at28_plcc_pins;

void at28_setup() {
  ezzif_reset();
  ezzif_gnd(at28_pins->gnd);
  ezzif_vdd(at28_pins->vdd, VDD_35);
  ezzif_bus_dir(at28_pins->addr, AT28_ADDR_PINS, 1);
  ezzif_o(at28_pins->ce, 1);
  ezzif_o(at28_pins->oe, 1);
  ezzif_o(at28_pins->we, 1);
  ezzif_bus_dir(at28_pins->data, AT28_DATA_PINS, 0);
}

void at28_plcc_setup() {
  ezzif_set_dip_pins(32);
  at28_pins = &at28_plcc_pins;
  at28_setup();
}

void at28_start_read() {
  ezzif_w(at28_pins->ce, 0);
  ezzif_w(at28_pins->oe, 0);
}

void at28_stop_read() {
  ezzif_w(at28_pins->ce, 1);
  ezzif_w(at28_pins->oe, 1);
}

unsigned char at28_read(unsigned int addr) {
  ezzif_bus_dir(at28_pins->data, AT28_DATA_PINS, 0);
  __delay_us(1);
  for (int i = 0; i < AT28_ADDR_PINS; i++, addr >>= 1) {
    ezzif_w(at28_pins->addr[i], addr & 0x01);
  }
  __delay_us(1);

  unsigned char data = 0;
  for (int i = AT28_DATA_PINS - 1; i >= 0; i--) {
    data <<= 1;
    data |= ezzif_r(at28_pins->data[i]);
  }
  return data;
}
