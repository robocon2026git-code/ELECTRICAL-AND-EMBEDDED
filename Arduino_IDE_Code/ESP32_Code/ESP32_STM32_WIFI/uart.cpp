/*
 * uart.cpp
 *  Fixed: Removed Serial.printf from send_packet (was blocking ~2-3ms every send)
 *         notify() state tracking now works correctly
 */

#include "uart.h"
#include <ps5Controller.h>

BluetoothSerial SerialBT;
HardwareSerial commSerial(1);

ButtonField button;
Packet pkt;

uint16_t flag;
float lx_val = 0;
float ly_val = 0;
float rx_val = 0;
float ry_val = 0;
float l2_val = 0;
float r2_val = 0;

char tx_buffer[BUFFER_SIZE];
float tx_buffer_analog;
bool valid_val = false;

void receive_pkt() {
    if (commSerial.available() >= (int)sizeof(BOT_Status_t)) {
        commSerial.readBytes((char*)&war_status, sizeof(BOT_Status_t));
        Send_BotStatusWifi();
    }
}

void send_packet(uint16_t btn_flag, float lx, float ly, float rx, float ry, float l2, float r2) {
    pkt.btn_flag = btn_flag;
    pkt.lx = lx;
    pkt.ly = ly;
    pkt.rx = rx;
    pkt.ry = ry;
    pkt.l2 = l2;
    pkt.r2 = r2;

    commSerial.write(STX);
    commSerial.write((uint8_t)sizeof(Packet));
    commSerial.write((uint8_t*)&pkt, sizeof(Packet));

    // Serial.printf REMOVED — was blocking ~2-3ms every transmission
    // Uncomment only for temporary debugging:
    // Serial.printf("FLAG: %04X | LX: %6.2f | LY: %6.2f | RX: %6.2f | RY: %6.2f\n",
    //                btn_flag, lx, ly, rx, ry);
}

void send_uart_data(const char *data) {
    commSerial.write(data);
}

void send_uart_val(const int val) {
    if (val > 0) {
        commSerial.write(val);
    }
}

void send_uart_analog_data(const float data) {
    commSerial.print(data);
}

void notify() {
    button.bits.up       = ps5.data.button.up;
    button.bits.down     = ps5.data.button.down;
    button.bits.left     = ps5.data.button.left;
    button.bits.right    = ps5.data.button.right;
    button.bits.triangle = ps5.data.button.triangle;
    button.bits.cross    = ps5.data.button.cross;
    button.bits.square   = ps5.data.button.square;
    button.bits.circle   = ps5.data.button.circle;
    button.bits.l1       = ps5.data.button.l1;
    button.bits.r1       = ps5.data.button.r1;
    button.bits.options  = ps5.data.button.options;
    button.bits.ps       = ps5.data.button.ps;
    button.bits.share    = ps5.data.button.share;
    button.bits.touchpad = ps5.data.button.touchpad;

    lx_val = (fabsf(ps5.data.analog.stick.lx)  > ANALOG_DEADZONE) ? ps5.data.analog.stick.lx  : 0;
    ly_val = (fabsf(ps5.data.analog.stick.ly)  > ANALOG_DEADZONE) ? ps5.data.analog.stick.ly  : 0;
    rx_val = (fabsf(ps5.data.analog.stick.rx)  > ANALOG_DEADZONE) ? ps5.data.analog.stick.rx  : 0;
    ry_val = (fabsf(ps5.data.analog.stick.ry)  > ANALOG_DEADZONE) ? ps5.data.analog.stick.ry  : 0;
    l2_val = (fabsf(ps5.data.analog.button.l2) > ANALOG_DEADZONE) ? ps5.data.analog.button.l2 : 0;
    r2_val = (fabsf(ps5.data.analog.button.r2) > ANALOG_DEADZONE) ? ps5.data.analog.button.r2 : 0;

    static uint16_t last_btn_flag = 0;
    static float last_lx = 0, last_ly = 0, last_rx = 0, last_ry = 0, last_l2 = 0, last_r2 = 0;

    if (button.halfword != last_btn_flag ||
        lx_val != last_lx || ly_val != last_ly ||
        rx_val != last_rx || ry_val != last_ry ||
        l2_val != last_l2 || r2_val != last_r2)
    {
        send_packet(button.halfword, lx_val, ly_val, rx_val, ry_val, l2_val, r2_val);

        last_btn_flag = button.halfword;
        last_lx = lx_val; last_ly = ly_val;
        last_rx = rx_val; last_ry = ry_val;
        last_l2 = l2_val; last_r2 = r2_val;
    }
}