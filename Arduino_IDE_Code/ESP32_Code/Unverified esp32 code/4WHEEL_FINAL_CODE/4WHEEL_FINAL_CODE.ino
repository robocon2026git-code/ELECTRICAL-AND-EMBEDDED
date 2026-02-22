#include <ps5Controller.h>
#include <ESP32Servo.h>
#define p1 5
#define p2 14
#define p3 4
#define p4 22
#define pwm1 25
#define pwm2 26
#define pwm3 27
#define pwm4 23
#define pneumapin 12
#define armpin 13
#define SLND1_OUT_PIN               15
#define D_RELAY                     100
#define D_HANDLER                   200
int x,y,w,vx=0,vy=0,omega=0;
Servo pneumaservo;
Servo armservo;
int pneumaangle =85;
int armangle=70;
int current=0;
int previous=0;
float m1,m2,m3,m4,maxraw;
unsigned long curr=0,prev=0;
void Calculation(int vx,int vy, int omega);
void Motor(float m, int p, int pwm);
void tst_cd();
void circle_handler();
void square_handler();
int EN=1;
int DI=0;
void circle_handler(){
    Serial.println("Circle");
    delay(D_HANDLER);
    digitalWrite(SLND1_OUT_PIN, EN);
}

//This will be called by tst_cd() function when square button is clicked
void square_handler(){
  Serial.println("Square");
  delay(D_HANDLER);
  digitalWrite(SLND1_OUT_PIN, DI);

}
void tst_cd(){
  if(ps5.data.button.circle){
    ps5.data.button.circle = 0;
    circle_handler();
  }

  else if(ps5.data.button.square){
    ps5.data.button.square = 0;
    square_handler();
  }
}


void setup()
{
 Serial.begin(115200); 
 pinMode(SLND1_OUT_PIN, OUTPUT);
 pinMode(p1,OUTPUT);
 pinMode(p2,OUTPUT);
 pinMode(p3,OUTPUT);
 pinMode(p4,OUTPUT);
 pinMode(pwm1,OUTPUT);
 pinMode(pwm2,OUTPUT);
 pinMode(pwm3,OUTPUT);
 pinMode(pwm4,OUTPUT);


 pneumaservo.attach(pneumapin);
 armservo.attach(armpin);

 ps5.begin("14:3A:9A:91:49:EE");

 
}

void loop()
{
  if(ps5.isConnected())
  {
    tst_cd();
   x=ps5.LStickY();
   y=ps5.LStickX();
   w=ps5.RStickX();
   if(abs(x)<20){x=0;}
   if(abs(y)<20){y=0;}
   if(abs(w)<20){w=0;}
   vx=(x*100)/127;
   vy=(y*100)/127;
   omega=(w*100)/127;
   Calculation(vx,vy,omega);
   Motor(m1,p1,pwm1);
   Motor(m2,p2,pwm2);
   Motor(m3,p3,pwm3);
   Motor(m4,p4,pwm4);
   pneumaangle=constrain(pneumaangle,85,154);
   armangle=constrain(armangle,70,145);

    if((int)ps5.data.button.up)
    {
     current=millis();
     if(current-previous>50){
     pneumaangle+=1;
     pneumaangle=constrain(pneumaangle,85,154);
     pneumaservo.write(pneumaangle);
     Serial.println(pneumaangle);
      previous=millis();}
    }

    if((int)ps5.data.button.down)
    {
      current=millis();
     if(current-previous>50){
     pneumaangle-=1;
     pneumaangle=constrain(pneumaangle,85,154);
     pneumaservo.write(pneumaangle);
     Serial.println(pneumaangle);
    previous=millis();}
    }

    if((int)ps5.data.button.triangle)
    {
      current=millis();
     if(current-previous>50){
     armangle+=1;
     armangle=constrain(armangle,70,145);
     armservo.write(armangle);
     Serial.println(armangle);
     previous=millis();}
    }

    if((int)ps5.data.button.cross)
    {
     current=millis();
     if(current-previous>50){
     armangle-=1;
     armangle=constrain(armangle,70,145);
     armservo.write(armangle);
     Serial.println(armangle);
    previous=millis();}
    }
  }
}

void Calculation(int vx,int vy, int omega)
{
  m1=(-vx+vy+omega);
  m2=(-vx-vy-omega);
  m3=(-vx-vy+omega);
  m4=(-vx+vy-omega);
  maxraw=max(max(abs(m1),abs(m2)),max(abs(m3),abs(m4)));
 if(maxraw>100)
 {
  float scale=100/maxraw;
  m1=(int)(m1*scale);
  m2=(int)(m2*scale);
  m3=(int)(m3*scale);
  m4=(int)(m4*scale);
 }
 curr=millis();
 if(curr-prev>=1000)
 {
 Serial.print("m1=");Serial.print(m1);
 Serial.print("m2=");Serial.print(m2);
 Serial.print("m3=");Serial.println(m3);
 Serial.print("m4=");Serial.println(m4);
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