#include "CustomBt.h"


char data[8] = {0};
int i = 0;


void setup_bt(){
  SerialBT.begin("ESP32_UART_TST");
  Serial.println("ESP32 Ready To Pair With BT");

  while(SerialBT.connected() == false){
    Serial.println("ESP32 BT Not Connected");
    delay(500);
  }

  if(SerialBT.connected()){
    Serial.println("ESP32 Connected To BT");
    digitalWrite(BT_PIN, HIGH);
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