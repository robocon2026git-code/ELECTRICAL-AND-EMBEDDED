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
    if (commSerial.available() >= sizeof(BOT_Status_t)) {
        commSerial.readBytes((char*)&war_status, sizeof(BOT_Status_t));
        Serial.println(war_status.kfs_ToF);
        Send_BotStatusWifi();
    }
}

void send_packet(uint16_t btn_flag, float lx_val, float ly_val, float rx_val, float ry_val, float l2_val, float r2_val) {
    pkt.btn_flag = btn_flag;
    pkt.lx = lx_val;
    pkt.ly = ly_val;
    pkt.rx = rx_val;
    pkt.ry = ry_val;
    pkt.l2 = l2_val;
    pkt.r2 = r2_val;

    commSerial.write(STX);
    commSerial.write(sizeof(Packet));
    commSerial.write((uint8_t*)&pkt, sizeof(Packet));   
  
    // Formatted perfectly to show all 16 bits and trigger values
    Serial.printf("FLAG: %04X | LX: %6.2f | LY: %6.2f | RX: %6.2f | RY: %6.2f | L2: %6.2f | R2: %6.2f\n", 
                   btn_flag, lx_val, ly_val, rx_val, ry_val, l2_val, r2_val);
}

void send_uart_data(const char *data) {
    commSerial.write(data);
    Serial.println(data);
}

void send_uart_val(const int val) {
    if(val > 0){
        commSerial.write(val);
        Serial.println(val, HEX);
    }
}

void send_uart_analog_data(const float data){
    commSerial.print(data);
    Serial.println(data);
}

void notify() {
    // Read all digital buttons safely (No delays, no clearing to 0)
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

    // Read analog values using the new Deadzone to stop the spam
    lx_val = (fabsf(ps5.data.analog.stick.lx) > ANALOG_DEADZONE) ? ps5.data.analog.stick.lx : 0;
    ly_val = (fabsf(ps5.data.analog.stick.ly) > ANALOG_DEADZONE) ? ps5.data.analog.stick.ly : 0;
    rx_val = (fabsf(ps5.data.analog.stick.rx) > ANALOG_DEADZONE) ? ps5.data.analog.stick.rx : 0;
    ry_val = (fabsf(ps5.data.analog.stick.ry) > ANALOG_DEADZONE) ? ps5.data.analog.stick.ry : 0;
    l2_val = (fabsf(ps5.data.analog.button.l2) > ANALOG_DEADZONE) ? ps5.data.analog.button.l2 : 0;
    r2_val = (fabsf(ps5.data.analog.button.r2) > ANALOG_DEADZONE) ? ps5.data.analog.button.r2 : 0;

    // Track state changes so we only send data when something ACTUALLY moves or is pressed
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