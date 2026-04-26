#include "uart.h"

BluetoothSerial SerialBT;
HardwareSerial commSerial(1);

ButtonField button;
Packet pkt;

uint16_t flag;
float lx_val;
float ly_val;
float rx_val;
float ry_val;
float l2_val;
float r2_val;

char tx_buffer[BUFFER_SIZE];
float tx_buffer_analog;

bool valid_val = false;

void receive_pkt()
{
    if (commSerial.available() >= sizeof(BOT_Status_t))
    {
        commSerial.readBytes((char*)&war_status, sizeof(BOT_Status_t));
        // Serial.print("Bot Speed: ");
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
  commSerial.write((uint8_t*)&pkt, sizeof(Packet));   // send raw bytes
  Serial.printf("btn flag --> %02X | LX -->  %.2f | LY  --> %.2f | RX -->  %.2f | RY --> %.2f\n", btn_flag, lx_val, ly_val, rx_val, ry_val);

  button.halfword = 0x00;
  pkt.lx = lx_val = 0;
  pkt.ly = ly_val = 0;
  pkt.rx = rx_val = 0;
  pkt.ry = ry_val = 0;
}


void send_uart_data(const char *data) {
  commSerial.write(data);
  Serial.println(data);
}

void send_uart_val(const int val) {
  if(val>0){
    commSerial.write(val);
    Serial.println(val, HEX);
  }
}

void send_uart_analog_data(const float data){
  commSerial.print(data);
  Serial.println(data);
}


void notify(){
  if(ps5.data.button.circle){
    ps5.data.button.circle = 0;
    delay(BUTTON_DEBOUNCING_DELAY);
    button.bits.circle = 1;
    valid_val = true;
  }
  if(ps5.data.button.square){
    ps5.data.button.square = 0;
    delay(BUTTON_DEBOUNCING_DELAY);
    button.bits.square = 1;
    valid_val = true;
  }
  if(ps5.data.button.triangle){
    ps5.data.button.triangle = 0;
    delay(BUTTON_DEBOUNCING_DELAY);
    button.bits.triangle = 1;
    valid_val = true;
  }
  if(ps5.data.button.cross){
    ps5.data.button.cross = 0;
    delay(BUTTON_DEBOUNCING_DELAY);
    button.bits.cross = 1;
    valid_val = true;
  }
  if(ps5.data.button.up){
    ps5.data.button.up = 0;
    delay(BUTTON_DEBOUNCING_DELAY);
    button.bits.up = 1;
    valid_val = true;
  }
  if(ps5.data.button.down){
    ps5.data.button.down = 0;
    delay(BUTTON_DEBOUNCING_DELAY);
    button.bits.down = 1;
    valid_val = true;
  }
  if(ps5.data.button.left){
    ps5.data.button.left = 0;
    delay(BUTTON_DEBOUNCING_DELAY);
    button.bits.left = 1;
    valid_val = true;
  }
  if(ps5.data.button.right){
    ps5.data.button.right = 0;
    delay(BUTTON_DEBOUNCING_DELAY);
    button.bits.right = 1;
    valid_val = true;
  }
  if(ps5.data.button.l1){
    ps5.data.button.l1 = 0;
    delay(BUTTON_DEBOUNCING_DELAY);
    button.bits.l1 = 1;
    valid_val = true;
  }
  if(ps5.data.button.r1){
    ps5.data.button.r1 = 0;
    delay(BUTTON_DEBOUNCING_DELAY);
    button.bits.r1 = 1;
    valid_val = true;
  }
  if(fabsf(ps5.data.analog.stick.lx) > ANALOG_ERROR){
    lx_val = ps5.data.analog.stick.lx;
    valid_val = true;
  }
  if(fabsf(ps5.data.analog.stick.ly) > ANALOG_ERROR){
    ly_val = ps5.data.analog.stick.ly;
    valid_val = true;
  }
  if(fabsf(ps5.data.analog.stick.rx) > ANALOG_ERROR){
    rx_val = ps5.data.analog.stick.rx;
    valid_val = true;
  }
  if(fabsf(ps5.data.analog.stick.rx) > ANALOG_ERROR){
    ry_val = ps5.data.analog.stick.ry;
    valid_val = true;
  }
  if(fabsf(ps5.data.analog.button.l2) > ANALOG_ERROR){
    l2_val = ps5.data.analog.button.l2;
    valid_val = true;
  }
  if(fabsf(ps5.data.analog.button.r2) > ANALOG_ERROR){
    r2_val = ps5.data.analog.button.r2;
    valid_val = true;
  }

  flag = button.halfword;
  if(valid_val == true){
    send_packet(flag, lx_val, ly_val, rx_val, ry_val, l2_val, r2_val);
    valid_val = false;
  }
}