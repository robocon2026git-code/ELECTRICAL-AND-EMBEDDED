#ifndef CUSTOMBT_H
#define CUSTOMBT_H

#include "Esp32Telemetry.h"

#define BT_PIN            7

void setup_bt();

void bt_handler();

void recv_uart_data();

#endif
