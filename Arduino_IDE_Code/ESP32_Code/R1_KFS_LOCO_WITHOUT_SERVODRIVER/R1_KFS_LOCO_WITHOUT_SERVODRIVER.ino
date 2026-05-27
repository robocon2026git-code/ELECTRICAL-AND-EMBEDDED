#include <ps5Controller.h>
#include <ESP32Servo.h>
#include <ServoEasing.hpp>
#include <stdint.h>

#define PIVOT_1_K 14
#define PIVOT_2_K 33
#define PIVOT_3_K 13
#define SPARK_PULSE_PIN 22

#define OFFSET_PWM_PIN  19
#define OFFSET_DIR_PIN  23

#define OFFSET_PWM_VAL  200

#define SPARK_UP_SPEED    1700
#define SPARK_STOP        1500
#define SPARK_DOWN_SPEED  1300

#define p1 32
#define p2 25
#define p3 16
#define p4 17

#define pwm1 27
#define pwm2 26
#define pwm3 18
#define pwm4 21

#define s 4

int x, y;
int vx = 0, vy = 0, omega = 0;
float m1, m2, m3, m4, maxraw;

ServoEasing pivot1_K;
ServoEasing pivot2_K;
ServoEasing pivot3_K;
ServoEasing servo;

Servo spark;

int t2_K_pos_1 = 84/*86*//*84*//*82*/,  t3_K_pos_1 = 135,  t1_K_pos_1 = 60/*58*/;
int t2_K_pos_2 = 99/*95*//*90*//*88*/, t3_K_pos_2 = 121/*125*/,   t1_K_pos_2 = 130/*120*/;

struct {
  uint8_t kfs_s1_state;
  uint8_t kfs_s2_state;
  uint8_t kfs_s3_state;
} kfs_state;

bool lastTriangle = false;
bool lastCross = false;

int pivot1_current;
int pivot1_target;
unsigned long lastMoveTime = 0;

int stepDelay = 3;   
int stepSize  = 3;   

void Calculation(int vx, int vy, int omega);
void Motor(float m, int p, int pwm);
void stopAllMotors();

void setup() {
  Serial.begin(115200);

  pinMode(p1, OUTPUT);
  pinMode(p2, OUTPUT);
  pinMode(p3, OUTPUT);
  pinMode(p4, OUTPUT);

  pinMode(pwm1, OUTPUT);
  pinMode(pwm2, OUTPUT);
  pinMode(pwm3, OUTPUT);
  pinMode(pwm4, OUTPUT);

  pinMode(OFFSET_PWM_PIN, OUTPUT);
  pinMode(OFFSET_DIR_PIN, OUTPUT);

  pivot1_K.attach(PIVOT_1_K);
  pivot2_K.attach(PIVOT_2_K);
  pivot3_K.attach(PIVOT_3_K);
  servo.attach(s);

  spark.attach(SPARK_PULSE_PIN);

  pivot1_K.write(t1_K_pos_1);
  pivot2_K.write(t2_K_pos_1);
  pivot3_K.write(t3_K_pos_1);
  servo.write(70);

  pivot1_current = t1_K_pos_1;
  pivot1_target  = t1_K_pos_1;

  spark.writeMicroseconds(SPARK_STOP);

  ps5.begin("14:3A:9A:91:49:EE");
}

void loop() {
  if (ps5.isConnected()) {

    x = ps5.LStickY();
    y = ps5.LStickX();

    if (abs(x) < 20) x = 0;
    if (abs(y) < 20) y = 0;

    vx = map(x, -127, 127, -200, 200);
    vy = map(y, -127, 127, -200, 200);

    if (ps5.R1()) {
      omega = 100;
      vx = 0;
      vy = 0;
    }
    else if (ps5.L1()) {
      omega = -100;
      vx = 0;
      vy = 0;
    }
    else {
      omega = 0;
    }

    Calculation(vx, vy, omega);

    Motor(m1, p1, pwm1);
    Motor(m2, p2, pwm2);
    Motor(m3, p3, pwm3);
    Motor(m4, p4, pwm4);

   
    bool currentTriangle = ps5.Triangle();

    if (currentTriangle && !lastTriangle) {
      kfs_state.kfs_s1_state = !kfs_state.kfs_s1_state;

      pivot1_target = kfs_state.kfs_s1_state ? t1_K_pos_2 : t1_K_pos_1;

      Serial.println("Pivot1 Target Changed");
    }
    lastTriangle = currentTriangle;

    
    if (millis() - lastMoveTime >= stepDelay) {
      lastMoveTime = millis();

      if (pivot1_current < pivot1_target) {
        pivot1_current += stepSize;
        if (pivot1_current > pivot1_target) pivot1_current = pivot1_target;
        pivot1_K.write(pivot1_current);
      } 
      else if (pivot1_current > pivot1_target) {
        pivot1_current -= stepSize;
        if (pivot1_current < pivot1_target) pivot1_current = pivot1_target;
        pivot1_K.write(pivot1_current);
      }
    }

    bool currentCross = ps5.Cross();

    if (currentCross && !lastCross) {
      kfs_state.kfs_s2_state = !kfs_state.kfs_s2_state;
      kfs_state.kfs_s3_state = !kfs_state.kfs_s3_state;

      pivot2_K.setEaseTo(
        kfs_state.kfs_s2_state ? t2_K_pos_2 : t2_K_pos_1,
        400
      );

      pivot3_K.setEaseTo(
        kfs_state.kfs_s3_state ? t3_K_pos_2 : t3_K_pos_1,
        400
      );

      Serial.println("Pivot2 & Pivot3 Moving");
    }
    lastCross = currentCross;

    // OFFSET MOTOR
    if (ps5.Left()) {
      digitalWrite(OFFSET_DIR_PIN, HIGH);
      analogWrite(OFFSET_PWM_PIN, OFFSET_PWM_VAL);
    } 
    else if (ps5.Right()) {
      digitalWrite(OFFSET_DIR_PIN, LOW);
      analogWrite(OFFSET_PWM_PIN, OFFSET_PWM_VAL);
    } 
    else {
      analogWrite(OFFSET_PWM_PIN, 0);
    }

    // SPARK
    if (ps5.Down()) {
      spark.writeMicroseconds(SPARK_UP_SPEED);
      Serial.println("up");
    } 
    else if (ps5.Up()) {
      spark.writeMicroseconds(SPARK_DOWN_SPEED);
      Serial.println("down");
    }
    else {
      spark.writeMicroseconds(SPARK_STOP);
      Serial.println("stop");
    }

    updateAllServos();

  } else {
    analogWrite(OFFSET_PWM_PIN, 0);
    spark.writeMicroseconds(SPARK_STOP);
    stopAllMotors();
  }
}

// ================= MOTOR =================

void stopAllMotors(){
  analogWrite(pwm1, 0);
  analogWrite(pwm2, 0);
  analogWrite(pwm3, 0);
  analogWrite(pwm4, 0);

  digitalWrite(p1, LOW);
  digitalWrite(p2, LOW);
  digitalWrite(p3, LOW);
  digitalWrite(p4, LOW);
}

void Calculation(int vx, int vy, int omega){
  m1 = (-vx - vy + omega);
  m2 = (vx -vy - omega);
  m3 = (-vx - vy - omega);
  m4 = (vx - vy + omega);

  maxraw = max(max(abs(m1), abs(m2)), max(abs(m3), abs(m4)));

  if (maxraw > 200) {
    float scale = 200.0 / maxraw;
    m1 *= scale;
    m2 *= scale;
    m3 *= scale;
    m4 *= scale;
  }
}

void Motor(float m, int p, int pwm){
  if (abs(m) < 10) {
    analogWrite(pwm, 0);
    return;
  }

  digitalWrite(p, m >= 0 ? HIGH : LOW);
  analogWrite(pwm, constrain(abs(m), 0, 200));
}