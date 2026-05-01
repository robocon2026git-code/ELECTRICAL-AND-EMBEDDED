#include "Esp32Telemetry.h"

#define BAUD_RATE         115200

int btn_click = 0;

void onConnect();
void onDisconnect();

void setup() {
  Serial.begin(BAUD_RATE);
  
  pinMode(2, OUTPUT);

  button.halfword = 0X00;

  // Setup_StreamWifi();
  commSerial.begin(BAUD_RATE, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial 2 has Started at" + String(BAUD_RATE) + "baud rate");

  Set_BotStatusTemp();
  // setup_bt();

  ps5.attachOnConnect(onConnect);
  ps5.attachOnDisconnect(onDisconnect);

  // ps5.begin("14:3A:9A:91:49:EE");         //Black colour
  // ps5.begin("E8:47:3A:36:ED:CA");         //White colour
  ps5.begin("90:B6:85:64:59:2B");         //Camofledge colour


  while(ps5.isConnected() == false){
    Serial.println("PS5 Not Found");
    delay(350);
  }
}

void loop() {
  // Prepare "ON" in the buffer 
  // bt_handler();
  notify();
  //send_uart_val(button.byte);
  // receive_pkt();
  button.halfword = 0x00;
}

void Set_BotStatusTemp(){
  war_status.bot_speed = 0;
  war_status.kfs_p1 = 0;
  war_status.kfs_p2 = 0;
  war_status.kfs_p3 = 0;
  war_status.kfs_ToF = 0;
  war_status.staff_p1 = 0;
  war_status.staff_p2 = 0;
  war_status.staff_p3 = 0;
}

void onConnect(){
  Serial.println("PS5 Connected");
}

void onDisconnect(){
  Serial.println("PS5 Disconnected");
}