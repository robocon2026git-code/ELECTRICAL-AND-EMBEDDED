#include <locomotion.h>

unsigned long prev = 0;
int lx, ly;
int vx, vy, omega;
float m1, m2, m3, m4, maxraw;

int ROT_SPEED = 140;   // rotation speed

void bot_run(){
    // ---- Left stick ----
  ly = ps5.RStickY();
  lx = ps5.RStickX();

  if (abs(lx) < 15) lx = 0;
  if (abs(ly) < 15) ly = 0;

  vx = map(ly, -127, 127, -200, 200);
  vy = map(lx, -127, 127, -200, 200);

  // ---- Button rotation ----
  omega = 0;

  if (ps5.L1()) omega = ROT_SPEED;
  if (ps5.R1()) omega = -ROT_SPEED;

  Calculation(vx,vy,omega);

  Motor(m1,p1,pwm1);
  Motor(m2,p2,pwm2);
  Motor(m3,p3,pwm3);
  Motor(m4,p4,pwm4);

}


// ================= OMNI X MIX =================
void Calculation(int vx, int vy, int omega)
{
  m1 = vx + vy + omega;
  m2 = vx - vy - omega;
  m3 = vx + vy - omega;
  m4 = vx - vy + omega;

  maxraw = max(max(abs(m1),abs(m2)), max(abs(m3),abs(m4)));

  if (maxraw > 255)
  {
    float s = 255.0 / maxraw;
    m1 *= s;
    m2 *= s;
    m3 *= s;
    m4 *= s;
  }

  // if (millis() - prev > 1000)
  // {
  //   Serial.printf("m: %.0f %.0f %.0f %.0f\n", m1,m2,m3,m4);
  //   prev = millis();
  // }
}

// ================= MOTOR =================
void Motor(float m, int dirPin, int pwmPin)
{
  if (m >= 0)
  {
    digitalWrite(dirPin, HIGH);
  }
  else
  {
    digitalWrite(dirPin, LOW);
    m = -m;
  }

  analogWrite(pwmPin, constrain((int)m,0,255));
}