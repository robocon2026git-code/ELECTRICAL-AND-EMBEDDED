#include <ps5Controller.h>
#include <ESP32Servo.h>

Servo extension;

#define extpin 18

int value = 1500;   // neutral

unsigned long lastPress = 0;
int stepDelay = 150;   // debounce / step timing

void setup()
{
  Serial.begin(115200);

  extension.attach(extpin, 1000, 2000);

  ps5.begin("90:B6:85:64:59:2B");

  // ARMING (IMPORTANT for BLDC / ESC)
  extension.writeMicroseconds(1500);
  delay(3000);
}

void loop()
{
  if (!ps5.isConnected()) return;

  unsigned long now = millis();

  // ⬆️ UP → Clockwise increase
  if (ps5.Up() && (now - lastPress > stepDelay))
  {
    value += 10;
    if (value > 2000) value = 2000;

    lastPress = now;
  }

  // ⬇️ DOWN → Anticlockwise decrease
  if (ps5.Down() && (now - lastPress > stepDelay))
  {
    value -= 10;
    if (value < 1000) value = 1000;

    lastPress = now;
  }

  // Send PWM
  extension.writeMicroseconds(value);

  // Debug
  Serial.print("PWM: ");
  Serial.println(value);

  delay(20);
}