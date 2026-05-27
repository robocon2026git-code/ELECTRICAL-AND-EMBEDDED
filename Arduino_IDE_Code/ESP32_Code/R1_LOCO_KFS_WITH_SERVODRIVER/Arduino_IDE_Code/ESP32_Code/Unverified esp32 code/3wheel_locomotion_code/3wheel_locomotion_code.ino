#include <ps5Controller.h>
#define p1 5
#define p2 14
#define p3 22
#define pwm1 25
#define pwm2 26
#define pwm3 27
int x,y,w,vx=0,vy=0,omega=0;
float r=0.06;
float m1,m2,m3,maxraw;
unsigned long curr=0,prev=0;
void Calculation(int vx,int vy, int omega);
void Motor(float m, int p, int pwm);
void setup()
{
 Serial.begin(115200);
 pinMode(p1,OUTPUT);
 pinMode(p2,OUTPUT);
 pinMode(p3,OUTPUT);
 pinMode(pwm1,OUTPUT);
 pinMode(pwm2,OUTPUT);
 pinMode(pwm3,OUTPUT);
 ps5.begin("14:3A:9A:91:49:EE");
}
void loop()
{
  if(ps5.isConnected())
  {
   x=ps5.LStickY();
   y=ps5.LStickX();
   w=ps5.RStickX();
   if(abs(x)<20){x=0;}
   if(abs(y)<20){y=0;}
   if(abs(w)<20){w=0;}
   vx=(x*255)/127;
   vy=(y*255)/127;
   omega=(w*255)/127;
   Calculation(vx,vy,omega);
   Motor(m1,p1,pwm1);
   Motor(m2,p2,pwm2);
   Motor(m3,p3,pwm3);
  }
}
void Calculation(int vx,int vy, int omega)
{
  m1=(-vy+omega)/r;
  m2=((0.866*vx)-(0.5*vy)-(omega))/r;
  m3=((-0.866*vx)-(0.5*vy)-(omega))/r;
  maxraw=max(max(abs(m1),abs(m2)),abs(m3));
 if(maxraw>255)
 {
  float scale=255/maxraw;
  m1=(int)(m1*scale);
  m2=(int)(m2*scale);
  m3=(int)(m3*scale);
 }
 curr=millis();
 if(curr-prev>=1000)
 {
 Serial.print("m1=");Serial.print(m1);
 Serial.print("m2=");Serial.print(m2);
 Serial.print("m3=");Serial.println(m3);
 prev=curr;
 }
}

void Motor(float m, int p, int pwm)
{
 if(m>0)
 {
  digitalWrite(p,HIGH);
 }
 else
 {
  digitalWrite(p,LOW);
  m=abs(m);
 }
 analogWrite(pwm,m);
}