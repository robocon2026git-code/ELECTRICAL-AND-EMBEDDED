#include <ps5Controller.h>
#include <ESP32Servo.h>

Servo extension;
Servo extension1;
#define extpin 24
#define extpin1 23
unsigned long current=0,previous=0;
int value;
int value1;
int fwd;
int bck;

void setup()
{
  Serial.begin(115200);
  pinMode(extpin,OUTPUT);
  extension.attach(extpin,1000,2000);
  extension.attach(extpin1,1000,2000);
  ps5.begin("");
}
void loop()
{
 if(ps5.isConnected())
  {
    fwd=ps5.R2Value();
    bck=ps5.L2Value();
    //FRONT
    if(fwd>5&&ps5.Up())
    {
      int value= map(fwd,0,255,1500,2000);
    }
    else if(bck>5&&ps5.Up())
    {
    value= map(bck,0,255,1500,1000);
    }
    extension.writeMicroseconds(value);
    current=millis();
    if(current-previous>1000)
    {
    Serial.print("Value:");Serial.println(value);
    previous=current;
    }
    //BACK
    if(fwd>5&&ps5.Down())
    {
      int value1= map(fwd,0,255,1500,2000);
    }
    else if(bck>5&&ps5.Down())
    {
    value1= map(bck,0,255,1500,1000);
    }
    extension1.writeMicroseconds(value1);
    current=millis();
    if(current-previous>1000)
    {
    Serial.print("Value1:");Serial.println(value1);
    previous=current;
    }
  }
  
 else
  {
    return;

  }
}