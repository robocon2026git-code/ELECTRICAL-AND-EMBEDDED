#include <string.h>
#include <stdint.h>
#include <ps5Controller.h>

#define TXD2              16
#define RXD2              17

#define BAUD_RATE         9600
#define BUFFER_SIZE       10

void onConnect();
void onDisconnect();


HardwareSerial commSerial(1);

char tx_buffer[BUFFER_SIZE];

void send_uart_data(const char *data);

void setup() {
  Serial.begin(115200);

  commSerial.begin(BAUD_RATE, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial 2 has Started at 9600 baud rate");

  ps5.attachOnConnect(onConnect);
  ps5.attachOnDisconnect(onDisconnect);

  while(ps5.isConnected() == false){
    Serial.println("PS5 Not Found");
    delay(500);
  }
}

void loop() {


  send_uart_data(tx_buffer);
  delay(50);
}

void write_tx_buffer(){
  if(ps5.data.button.circle){
      tx_buffer[0] = 'C';
      tx_buffer[1] = 'I';
      tx_buffer[2] = '\0';  // null-terminate so it's a proper C-string
  }
  else if(ps5.data.button.cross){
      tx_buffer[0] = 'C';
      tx_buffer[1] = 'R';
      tx_buffer[2] = '\0';
  }
    else if(ps5.data.button.square){
      tx_buffer[0] = 'S';
      tx_buffer[1] = 'Q';
      tx_buffer[2] = '\0';
  }
}

void send_uart_data(const char *data) {
  commSerial.print(data);
  Serial.println(data);
}


void onConnect(){
  Serial.println("PS5 Connected");
}

void onDisconnect(){
  Serial.println("PS5 Disconnected");
}