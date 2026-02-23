#include <ps5Controller.h>

#define DIR 22
#define PUL 23
#define ENA 18

int Y;

const int minDelay = 700;     
const int maxDelay = 1800;    
const int rampStep = 40;
const int deadZone = 20;

int currentDelay = 1800;
int targetDelay  = 1800;

void setup() {
  ps5.begin("14:3A:9A:91:49:EE");

  pinMode(DIR, OUTPUT);
  pinMode(PUL, OUTPUT);
  pinMode(ENA, OUTPUT);

  digitalWrite(ENA, LOW);
}

void loop() {

  if (!ps5.isConnected()) return;

  Y = ps5.RStickY();

  if (abs(Y) < deadZone) return;

  digitalWrite(DIR, Y > 0 ? LOW : HIGH);

  targetDelay = map(abs(Y), 0, 127, maxDelay, minDelay);

  if (currentDelay > targetDelay)
    currentDelay -= rampStep;
  else if (currentDelay < targetDelay)
    currentDelay += rampStep;

  currentDelay = constrain(currentDelay, minDelay, maxDelay);

  digitalWrite(PUL, HIGH);
  delayMicroseconds(currentDelay);
  digitalWrite(PUL, LOW);
  delayMicroseconds(currentDelay);
}