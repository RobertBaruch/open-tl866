#include <xc.h>

#include "../../arglib.h"
#include "../../at28.h"
#include "../../comlib.h"
#include "../../stock_compat.h"
#include "../../system.h"

int checking_sig = 1;

static inline void print_help(void) {
  com_println("open-tl866 (at28)");
  com_println("-----------------");
  com_println("i              Initialize/reset pins");
  com_println("r addr len     Read from target");
  com_println("h              Print help");
  com_println("L val          LED on/off");
  com_println("b              reset to bootloader");
  com_println("");
  com_println("addr, len in hex");
}

static void print_read(unsigned int addr, unsigned int len) {
  printf("%04X", addr);

  for (unsigned int byte_idx = 0; byte_idx < len; byte_idx++) {
    printf(" %02X", at28_read(addr + byte_idx));
  }
  printf("\r\n");
}

static inline void eval_command(char *cmd) {
  unsigned char *cmd_t = strtok(cmd, " ");

  if (cmd_t == NULL) return;

  switch (cmd_t[0]) {
    case 'i':
      LED = 0;
      at28_plcc_setup();
      at28_disable();
      LED = 1;
      break;

    case 'r': {
      unsigned int addr = xtoi(strtok(NULL, " "));
      unsigned int len = xtoi(strtok(NULL, " "));
      LED = 0;
      at28_enable();
      at28_start_read();
      print_read(addr, len);
      at28_stop_read();
      at28_disable();
      LED = 1;
      break;
    }

    case '?':
    case 'h':
      print_help();
      break;

    // LED on/off
    case 'L': {
      if (arg_bit()) LED = last_bit;
      break;
    }

    case 'b':
      stock_reset_to_bootloader();
      break;

    default:
      printf("ERROR: unknown command 0x%02X (%c)\r\n", cmd_t[0], cmd_t[0]);
      break;
  }
}

void mode_main(void) {
  vpp_dis();

  while (1) {
    eval_command(com_cmd_prompt());
  }
}

void interrupt high_priority isr() { usb_service(); }
