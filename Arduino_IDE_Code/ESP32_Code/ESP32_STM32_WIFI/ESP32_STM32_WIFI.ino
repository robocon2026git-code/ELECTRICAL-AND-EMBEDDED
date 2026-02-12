#include "Esp32Telemetry.h"

#define BAUD_RATE         115200

int btn_click = 0;

void onConnect();
void onDisconnect();

void setup() {
  Serial.begin(BAUD_RATE);
  
  pinMode(2, OUTPUT);

  button.byte = 0X00;

  // SerialBT.begin("ESP32_UART_TST");
  // Serial.println("ESP32 Ready To Pair With BT");

  Setup_StreamWifi();
  commSerial.begin(BAUD_RATE, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial 2 has Started at" + String(BAUD_RATE) + "baud rate");

  Set_BotStatusTemp();

  ps5.attachOnConnect(onConnect);
  ps5.attachOnDisconnect(onDisconnect);

  // ps5.begin("14:3A:9A:91:49:EE");         //Black colour
  // ps5.begin("E8:47:3A:36:ED:CA");         //White colour
  ps5.begin("90:B6:85:64:59:2B");           //Camofledge colour


  // while(ps5.isConnected() == false){
  //   Serial.println("PS5 Not Found");
  //   delay(350);
  // }

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
  // bt_handler();
  notify();
  //send_uart_val(button.byte);
  receive_pkt();
  button.byte = 0x00;
  Send_BotStatusWifi();
}


void Set_BotStatusTemp(){
  war_status.bot_speed = 10;
  war_status.kfs_p1 = 0;
  war_status.kfs_p2 = 0;
  war_status.kfs_p3 = 0;
  war_status.kfs_ToF = 95;
  war_status.staff_p1 = 1;
  war_status.staff_p2 = 0;
  war_status.staff_p3 = 0;
}

void onConnect(){
  Serial.println("PS5 Connected");
}

void onDisconnect(){
  Serial.println("PS5 Disconnected");
}