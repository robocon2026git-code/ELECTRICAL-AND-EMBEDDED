#include <string.h>
#include <stdint.h>
#include <BluetoothSerial.h>
#include <ps5Controller.h>
#include <math.h> // for fabsf()

#define BUTTON_DEBOUNCING_DELAY   50

#define TXD2              19
#define RXD2              21

#define BT_PIN            7

#define BAUD_RATE         115200
#define BUFFER_SIZE       10

#define ANALOG_ERROR      2

#define STX               0xAA

typedef union{
  uint8_t byte;
  struct{
    uint8_t up        :1;
    uint8_t down      :1;
    uint8_t left      :1;
    uint8_t right     :1;
    uint8_t triangle  :1;
    uint8_t cross     :1;
    uint8_t square    :1;
    uint8_t circle    :1;
  } bits;
} ButtonField;

typedef struct __attribute__((packed)) {
    uint8_t  btn_flag;     // 1 byte
    float    lx;       // 4 bytes
    float    ly;       // 4 bytes
    float    rx;       // 4 bytes
    float    ry;       // 4 bytes
} Packet;


BluetoothSerial SerialBT;
HardwareSerial commSerial(1);

int btn_click = 0;

char data[8] = {0};
int i = 0;

ButtonField button;
Packet pkt;


bool valid_val = false;

uint8_t flag;
float lx_val;
float ly_val;
float rx_val;
float ry_val;

char tx_buffer[BUFFER_SIZE];
float tx_buffer_analog;

void onConnect();
void onDisconnect();

void send_uart_data(const char *data);
void send_uart_analog_data(const float data);
void send_uart_val(const int *val);


void setup() {
  Serial.begin(115200);
  
  pinMode(2, OUTPUT);

  button.byte = 0X00;

  SerialBT.begin("ESP32_UART_TST");
  Serial.println("ESP32 Ready To Pair With BT");

  commSerial.begin(BAUD_RATE, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial 2 has Started at 9600 baud rate");

  ps5.attachOnConnect(onConnect);
  ps5.attachOnDisconnect(onDisconnect);

  // ps5.begin("14:3A:9A:91:49:EE");         //Black colour
  ps5.begin("E8:47:3A:36:ED:CA");         //White colour


  while(ps5.isConnected() == false){
    Serial.println("PS5 Not Found");
    delay(350);
  }

  // while(SerialBT.connected() == false){
  //   Serial.println("ESP32 BT Not Connected");
  //   delay(500);
  // }

  // if(SerialBT.connected()){
  //   Serial.println("ESP32 Connected To BT");
  //   digitalWrite(BT_PIN, HIGH);
  // }
}

void loop() {
  // Prepare "ON" in the buffer
  bt_handler();
  notify();
  //send_uart_val(button.byte);
  button.byte = 0x00;
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

void send_packet(uint8_t btn_flag, float lx_val, float ly_val, float rx_val, float ry_val) {
    pkt.btn_flag = btn_flag;
    pkt.lx = lx_val;
    pkt.ly = ly_val;
    pkt.rx = rx_val;
    pkt.ry = ry_val;

    commSerial.write(STX);
    commSerial.write(sizeof(Packet));
    commSerial.write((uint8_t*)&pkt, sizeof(Packet));   // send raw bytes
    Serial.printf("btn flag --> %02X | LX -->  %.2f | LY  --> %.2f | RX -->  %.2f | RY --> %.2f\n", btn_flag, lx_val, ly_val, rx_val, ry_val);

    button.byte = 0x00;
    pkt.lx = lx_val = 0;
    pkt.ly = ly_val = 0;
    pkt.rx = rx_val = 0;
    pkt.ry = ry_val = 0;
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
    valid_val = true;
    ps5.data.button.right = 0;
    delay(BUTTON_DEBOUNCING_DELAY);
    button.bits.right = 1;
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

  flag = button.byte;
  if(valid_val == true){
    send_packet(flag, lx_val, ly_val, rx_val, ry_val);
    valid_val = false;
  }
}

void bt_handler(){
  if(SerialBT.connected()){
    digitalWrite(BT_PIN, HIGH);
  }
  else{
    digitalWrite(BT_PIN, LOW);
  }

  while (SerialBT.available() > 0) {
    char incoming = SerialBT.read();
    if (i < sizeof(data) - 1) {  // Prevent overflow
      data[i++] = incoming;
    }
  }

  if (i > 0) {
    data[i] = '\0';  // Null-terminate the string

    Serial.print("RECEIVED ==== ");
    Serial.println(data);
    send_uart_data(data);
    i = 0; // Reset for next message
  }
}

void recv_uart_data(){
 while(SerialBT.available() > 0){
  int dataLen = sizeof(SerialBT.available());
  uint8_t btData[] = {};
  for(int i = 0; i < dataLen; i++){
    btData[i] = SerialBT.read();
  }
 }
}


void onConnect(){
  Serial.println("PS5 Connected");
}

void onDisconnect(){
  Serial.println("PS5 Disconnected");
}