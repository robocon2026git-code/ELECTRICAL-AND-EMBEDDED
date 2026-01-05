#include <ps5Controller.h>
#define p1 5
#define p2 14
#define p3 4
#define p4 22
#define pwm1 25
#define pwm2 26
#define pwm3 27
#define pwm4 23
int x,y,w,vx=0,vy=0,omega=0;
unsigned long curr=0,prev=0;  
float m1,m2,m3,m4,maxraw;

int toggle = 0;

void Calculation(int vx,int vy, int omega);
void Motor(float m, int p, int pwm);
void mot_control_const();

void setup()
{
 Serial.begin(115200); 
 pinMode(p1,OUTPUT);
 pinMode(p2,OUTPUT);
 pinMode(p3,OUTPUT);
 pinMode(p4,OUTPUT);
 pinMode(pwm1,OUTPUT);
 pinMode(pwm2,OUTPUT);
 pinMode(pwm3,OUTPUT);
 pinMode(pwm4,OUTPUT);
 ps5.begin("90:B6:85:64:59:2B");
}

void loop()
{
  if(ps5.isConnected())
  {
    if(ps5.Up()){
      toggle = 0;
    }else if(ps5.Down()){
      toggle = 1;
    }

    if(toggle == 0){
      mot_control_const();
    }else if(toggle == 1){
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

  //  x=ps5.LStickY();
  //  y=ps5.LStickX();
  //  w=ps5.RStickX();
  //  if(abs(x)<20){x=0;}
  //  if(abs(y)<20){y=0;}
  //  if(abs(w)<20){w=0;}
  //  vx=(x*255)/127;
  //  vy=(y*255)/127;
  //  omega=(w*255)/127;
  //  Calculation(vx,vy,omega);
  //  Motor(m1,p1,pwm1);
  //  Motor(m2,p2,pwm2);
  //  Motor(m3,p3,pwm3);
  //  Motor(m4,p4,pwm4);

   }
}

void Calculation(int vx,int vy, int omega)
{
  m1=(-vx+vy+omega);
  m2=(-vx-vy-omega);
  m3=(-vx+vy-omega);
  m4=(-vx-vy+omega);
  maxraw=max(max(abs(m1),abs(m2)),max(abs(m3),abs(m4)));
 if(maxraw>255)
 {
  float scale=255/maxraw;
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

void mot_control_const(){
  if(ps5.Triangle()){
    digitalWrite(p1, LOW);
    digitalWrite(p2, LOW);
    digitalWrite(p3, LOW);
    digitalWrite(p4, LOW);

    analogWrite(pwm1,255);
    analogWrite(pwm2,255);
    analogWrite(pwm3,255);
    analogWrite(pwm4,255);
    Serial.println("Forward");
  }
  else if(ps5.Cross()){
    digitalWrite(p1, HIGH);
    digitalWrite(p2, HIGH);
    digitalWrite(p3, HIGH);
    digitalWrite(p4, HIGH);

    analogWrite(pwm1,255);
    analogWrite(pwm2,255);
    analogWrite(pwm3,255);
    analogWrite(pwm4,255);

    Serial.println("Backward");
  }
  else if(ps5.Circle()){    //Left
    digitalWrite(p1, HIGH);
    digitalWrite(p2, LOW);
    digitalWrite(p3, HIGH);
    digitalWrite(p4, LOW);
    
    analogWrite(pwm1,255);
    analogWrite(pwm2,255);
    analogWrite(pwm3,255);
    analogWrite(pwm4,255);

    Serial.println("Left");
  }
  else if(ps5.Square()){   //Right
    analogWrite(pwm1,255);
    analogWrite(pwm2,255);
    analogWrite(pwm3,255);
    analogWrite(pwm4,255);
    
    digitalWrite(p1, LOW);
    digitalWrite(p2, HIGH);
    digitalWrite(p3, LOW);
    digitalWrite(p4, HIGH);
    Serial.println("Right");
  }else{
    analogWrite(pwm1,0);
    analogWrite(pwm2,0);
    analogWrite(pwm3,0);
    analogWrite(pwm4,0);
  }
}