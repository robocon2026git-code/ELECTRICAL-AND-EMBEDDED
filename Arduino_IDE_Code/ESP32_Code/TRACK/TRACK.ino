#include <ps5Controller.h>
#include <ESP32Servo.h>

#define MAX_REV               1000U
#define NEUTRAL               1500U
#define MAX_FRW               2000U

#define MAX_BOT_SPEED_BLDC    200U

#define L_PWM_PIN             14
#define R_PWM_PIN             27

#define DIR_L1                25
#define DIR_R1                26
#define DIR_L2                32
#define DIR_R2                33

#define ERROR                 15

void run_track_dcm_init();
void run_track_bldcm_init();

void run_track_dcm(int pwm);
void run_track_bldcm(int pwm_bldc);

Servo bldc_l;
Servo bldc_r;

void setup() {
  Serial.begin(115200);

  run_track_dcm_init();
  run_track_bldcm_init();

  ps5.begin("14:3A:9A:91:49:EE");         //Black colour
  // ps5.begin("E8:47:3A:36:ED:CA");         //White colour
  // ps5.begin("90:B6:85:64:59:2B");         //Camofledge colour

  while (ps5.isConnected() == false) {
    Serial.println("Waiting for PS5 Controller....");
    delay(500);
  }

  Serial.println("PS5 Connected!");

}

void loop() {
  // if (!ps5.isConnected()) return;

  int Speed = ps5.LStickY();
  if (abs(Speed) < ERROR) Speed = 0;

  Speed = constrain(Speed, -127, 127);

  int pwm = map(abs(Speed), 0, 127, 0, 255);
  int pwm_bldc = map(abs(Speed), 0, 127, 0, MAX_BOT_SPEED_BLDC);

  // ===== BUTTON LOGIC =====
  run_track_bldcm(pwm_bldc);

}


void run_track_dcm_init() {
  pinMode(L_PWM_PIN, OUTPUT);
  pinMode(R_PWM_PIN, OUTPUT);
  pinMode(DIR_L1, OUTPUT);
  pinMode(DIR_R1, OUTPUT);
}

void run_track_bldcm_init() {
  bldc_l.attach(L_PWM_PIN, MAX_REV, MAX_FRW);
  bldc_r.attach(R_PWM_PIN, MAX_REV, MAX_FRW);
}



void run_track_dcm(int pwm) {
  if (ps5.Triangle()) {           // Forward
    digitalWrite(DIR_L1, LOW);
    digitalWrite(DIR_R1, LOW);
    // digitalWrite(DIR_L2, HIGH);
    // digitalWrite(DIR_R2, HIGH);
    analogWrite(L_PWM_PIN, pwm);
    analogWrite(R_PWM_PIN, pwm);
    Serial.print("Forward PWM: ");
    Serial.println(pwm);

  } else if (ps5.Cross()) {       // Backward
    digitalWrite(DIR_L1, HIGH);
    digitalWrite(DIR_R1, HIGH);
    // digitalWrite(DIR_L2, LOW);
    // digitalWrite(DIR_R2, LOW);
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

void run_track_bldcm(int pwm_bldc) {
  int fw_bldc_lm = NEUTRAL + pwm_bldc;
  int fw_bldc_rm = NEUTRAL + pwm_bldc;

  int rw_bldc_lm = NEUTRAL - pwm_bldc;
  int rw_bldc_rm = NEUTRAL - pwm_bldc;

  int lt_bldc_lm = NEUTRAL - pwm_bldc;
  int lt_bldc_rm = NEUTRAL + pwm_bldc;

  int rt_bldc_lm = NEUTRAL + pwm_bldc;
  int rt_bldc_rm = NEUTRAL - pwm_bldc;


  if (ps5.Triangle()) {           // Forward
    bldc_l.writeMicroseconds(fw_bldc_lm);
    bldc_r.writeMicroseconds(fw_bldc_rm);
    Serial.print("Forward PWM Left Motor: ");
    Serial.print(fw_bldc_lm);
    Serial.print(" | Forward PWM RIght Motor: ");
    Serial.println(fw_bldc_rm);

  } else if (ps5.Cross()) {       // Backward
    bldc_l.writeMicroseconds(rw_bldc_lm);
    bldc_r.writeMicroseconds(rw_bldc_rm);
    Serial.print("Reverse PWM Left Motor: ");
    Serial.print(rw_bldc_lm);
    Serial.print(" | Reverse PWM RIght Motor: ");
    Serial.println(rw_bldc_rm);

  } else if (ps5.Square()) {      // Left turn
    bldc_l.writeMicroseconds(lt_bldc_lm);
    bldc_r.writeMicroseconds(lt_bldc_rm);
    Serial.print("Left PWM Left Motor: ");
    Serial.print(lt_bldc_lm);
    Serial.print(" | Left PWM RIght Motor: ");
    Serial.println(lt_bldc_rm);

  } else if (ps5.Circle()) {      // Right turn
    bldc_l.writeMicroseconds(rt_bldc_lm);
    bldc_r.writeMicroseconds(rt_bldc_rm);
    Serial.print("Right PWM Left Motor: ");
    Serial.print(rt_bldc_lm);
    Serial.print(" | Right PWM RIght Motor: ");
    Serial.println(rt_bldc_rm);

  } else {
    bldc_l.writeMicroseconds(NEUTRAL);
    bldc_r.writeMicroseconds(NEUTRAL);
  }
}