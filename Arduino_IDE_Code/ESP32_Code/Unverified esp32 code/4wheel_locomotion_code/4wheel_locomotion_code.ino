#include <ps5Controller.h>
#define p1 12
#define p2 14
#define p3 4
#define p4 13
#define pwm1 25
#define pwm2 26
#define pwm3 27
#define pwm4 12
int x,y,w,vx=0,vy=0,omega=0;
float m1=0,m2=0,m3=0,m4=0;
float R=0.5,r=0.5;
unsigned long curr=0,prev=0;
void Calculation(int vx,int vy, int omega);
void Motor(float m, int p, int pwm);
void setup()
{
 Serial.begin(9600);
 pinMode(p1,OUTPUT);
 pinMode(p2,OUTPUT);
 pinMode(p3,OUTPUT);
 pinMode(p4,OUTPUT);
 pinMode(pwm1,OUTPUT);
 pinMode(pwm2,OUTPUT);
 pinMode(pwm3,OUTPUT);
 pinMode(pwm4,OUTPUT);
 ps5.begin("");
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
   Motor(m4,p4,pwm4);
  }
}
void Calculation(int vx,int vy, int omega)
{
 m1=(vx+vy-omega)/r;
  m2=(vx-vy+omega)/r;
  m3=(vx-vy-omega)/r;
  m4=(vx+vy+omega)/r;
 float maxraw=max(max(abs(m1),abs(m2)),max(abs(m3),abs(m4));
 if(maxraw>255)
 {
  float scale=255/maxraw
  m1=(int)(m1*scale);
  m2=(int)(m2*scale);
  m3=(int)(m3*scale);
  m4=(int)(m4*scale);
 }
 curr=millis()
 if(cuur-prev>=1000)
 {
 Serial.print("m1=");Serial.print(m1);
 Serial.print("m2=");Serial.print(m2);
 Serial.print("m3=");Serial.print(m3);
 Serial.print("m4=");Serial.println(m4);
 prev=curr;
 }
}
void Motor(float m, int p, int pwm)
{
 if(m>0)
 {
  digital.Write(p,HIGH);
 }
 else
 {
  digital.Write(p,LOW);
  m=abs(m);
 }
 analogWrite(pwm,m);
}