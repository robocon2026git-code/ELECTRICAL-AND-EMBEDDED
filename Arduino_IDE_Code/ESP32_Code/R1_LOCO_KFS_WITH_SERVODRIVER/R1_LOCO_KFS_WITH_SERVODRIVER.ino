#include <ps5Controller.h>
#include <ESP32Servo.h>
#include <stdint.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>

#define SPARK_PULSE_PIN 25

#define OFFSET_PWM_PIN 18
#define OFFSET_DIR_PIN 12

#define OFFSET_PWM_VAL 200

#define SPARK_UP_SPEED 1700
#define SPARK_STOP 1500
#define SPARK_DOWN_SPEED 1300

#define p1 13
#define p2 14
#define p3 32
#define p4 33

#define pwm1 26
#define pwm2 27
#define pwm3 16
#define pwm4 17

#define SERVO_FREQ 50

#define SERVO_MIN 102
#define SERVO_MAX 512

#define SERVO_MAIN_CH 0
#define SERVO_S3_CH 1
#define SERVO_S2_CH 2

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

int x, y;
int vx = 0, vy = 0, omega = 0;

float m1, m2, m3, m4, maxraw;

Servo spark;

int t2_K_pos_1 = 135;//130
int t3_K_pos_1 = 74;//80
int t1_K_pos_1 = 60;

int t2_K_pos_2 = 115;
int t3_K_pos_2 = 94;
int t1_K_pos_2 = 120;

//MAIN-60,S2-74,S3-135
//MAIN-120,S2-94,S3-115

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
int stepSize = 3;

void Calculation(int vx, int vy, int omega);
void Motor(float m, int p, int pwmPin);
void stopAllMotors();
void servofunction(uint8_t channel, int angle);

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

  Wire.begin();

  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);

  delay(500);

  servofunction(SERVO_MAIN_CH, t1_K_pos_1);
  servofunction(SERVO_S3_CH, t2_K_pos_1);
  servofunction(SERVO_S2_CH, t3_K_pos_1);

  spark.attach(SPARK_PULSE_PIN);

  pivot1_current = t1_K_pos_1;
  pivot1_target = t1_K_pos_1;

  spark.writeMicroseconds(SPARK_STOP);

  ps5.begin("90:B6:85:64:59:2B");

  Serial.println("System Ready");
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

      pivot1_target =kfs_state.kfs_s1_state ? t1_K_pos_2 : t1_K_pos_1;
    }

    lastTriangle = currentTriangle;

    if (millis() - lastMoveTime >= stepDelay) {

      lastMoveTime = millis();

      if (pivot1_current < pivot1_target) {

        pivot1_current += stepSize;

        if (pivot1_current > pivot1_target)
          pivot1_current = pivot1_target;

        servofunction(SERVO_MAIN_CH, pivot1_current);
      }

      else if (pivot1_current > pivot1_target) {

        pivot1_current -= stepSize;

        if (pivot1_current < pivot1_target)
          pivot1_current = pivot1_target;

        servofunction(SERVO_MAIN_CH, pivot1_current);
      }
    }

    bool currentCross = ps5.Cross();

    if (currentCross && !lastCross) {

      kfs_state.kfs_s2_state = !kfs_state.kfs_s2_state;
      kfs_state.kfs_s3_state = !kfs_state.kfs_s3_state;

      servofunction(SERVO_S3_CH,kfs_state.kfs_s2_state ? t2_K_pos_2 : t2_K_pos_1);

      servofunction(SERVO_S2_CH,kfs_state.kfs_s3_state ? t3_K_pos_2 : t3_K_pos_1);
    }

    lastCross = currentCross;


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

    if (ps5.Down()) {

      spark.writeMicroseconds(SPARK_UP_SPEED);
      Serial.println("Spark Up");

    }
    else if (ps5.Up()) {

      spark.writeMicroseconds(SPARK_DOWN_SPEED);
      Serial.println("Spark Down");

    }
    else {

      spark.writeMicroseconds(SPARK_STOP);
    }

  }
  else {

    analogWrite(OFFSET_PWM_PIN, 0);

    spark.writeMicroseconds(SPARK_STOP);

    stopAllMotors();
  }
}


void stopAllMotors() {

  analogWrite(pwm1, 0);
  analogWrite(pwm2, 0);
  analogWrite(pwm3, 0);
  analogWrite(pwm4, 0);

  digitalWrite(p1, LOW);
  digitalWrite(p2, LOW);
  digitalWrite(p3, LOW);
  digitalWrite(p4, LOW);
}


void Calculation(int vx, int vy, int omega) {

  m1 = (-vx - vy + omega);
  m2 = ( vx - vy - omega);
  m3 = (-vx - vy - omega);
  m4 = ( vx - vy + omega);

  maxraw = max(max(abs(m1), abs(m2)),max(abs(m3), abs(m4)));

  if (maxraw > 200) {

    float scale = 200.0 / maxraw;

    m1 *= scale;
    m2 *= scale;
    m3 *= scale;
    m4 *= scale;
  }
}


void Motor(float m, int p, int pwmPin) {

  if (abs(m) < 10) {

    analogWrite(pwmPin, 0);
    return;
  }

  digitalWrite(p, m >= 0 ? HIGH : LOW);

  analogWrite(pwmPin,constrain(abs(m), 0, 200));
}


void servofunction(uint8_t channel, int angle) {

  angle = constrain(angle, 0, 180);

  int pulse = map(angle,0,180,SERVO_MIN,SERVO_MAX);

  pwm.setPWM(channel, 0, pulse);
}