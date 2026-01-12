#include <ps5Controller.h>

#define L_PWM_PIN   25
#define R_PWM_PIN   27
#define DIR_L1      14
#define DIR_R1      26
#define DIR_L2      32
#define DIR_R2      33



#define ERROR 15

void setup() {
  Serial.begin(115200);

  pinMode(L_PWM_PIN, OUTPUT);
  pinMode(R_PWM_PIN, OUTPUT);
  pinMode(DIR_L, OUTPUT);
  pinMode(DIR_R, OUTPUT);

  ps5.begin("14:3a:9a:91:49:ee");
  Serial.println("Waiting for PS5 controller...");
}

void loop() {
  if (!ps5.isConnected()) return;

  int Speed = ps5.LStickY();
  if (abs(Speed) < ERROR) Speed = 0;

  Speed = constrain(Speed, -127, 127);

  int pwm = map(abs(Speed), 0, 127, 0, 255);

  // ===== BUTTON LOGIC =====
  if (ps5.Triangle()) {           // Forward
    digitalWrite(DIR_L1, LOW);
    digitalWrite(DIR_R1, LOW);
    digitalWrite(DIR_L2, HIGH);
    digitalWrite(DIR_R2, HIGH);
    analogWrite(L_PWM_PIN, pwm);
    analogWrite(R_PWM_PIN, pwm);
    Serial.print("Forward PWM: ");
    Serial.println(pwm);

  } else if (ps5.Cross()) {       // Backward
    digitalWrite(DIR_L1, HIGH);
    digitalWrite(DIR_R1, HIGH);
    digitalWrite(DIR_L2, LOW);
    digitalWrite(DIR_R2, LOW);
    analogWrite(L_PWM_PIN, pwm);
    analogWrite(R_PWM_PIN, pwm);
    Serial.print("Reverse PWM: ");
    Serial.println(pwm);

  } else if (ps5.Square()) {      // Left turn
    digitalWrite(DIR_L1, HIGH);
    digitalWrite(DIR_R1, LOW);
    digitalWrite(DIR_L2, LOW);
    digitalWrite(DIR_R2, HIGH);
    analogWrite(L_PWM_PIN, pwm);
    analogWrite(R_PWM_PIN, pwm);
    Serial.print("Left PWM: ");
    Serial.println(pwm);

  } else if (ps5.Circle()) {      // Right turn
    digitalWrite(DIR_L1, LOW);
    digitalWrite(DIR_R1, HIGH);
    digitalWrite(DIR_L2, HIGH);
    digitalWrite(DIR_R2, LOW);
    analogWrite(L_PWM_PIN, pwm);
    analogWrite(R_PWM_PIN, pwm);
    Serial.print("Right PWM: ");
    Serial.println(pwm);

  } else {
    analogWrite(L_PWM_PIN, 0);      // Stop
    analogWrite(R_PWM_PIN, 0);      // Stop
  }
}
