#ifndef LOCOMOTION_H
#define LOCOMOTION_H

#include <Arduino.h>
#include <ps5Controller.h>

#define p1 17
#define p2 18
#define p3 19
#define p4 16

#define pwm1 25
#define pwm2 26
#define pwm3 22
#define pwm4 23

void bot_run();

extern unsigned long prev;
extern int lx, ly;
extern int vx, vy, omega;
extern float m1, m2, m3, m4, maxraw;

extern int ROT_SPEED;   // rotation speed

void Calculation(int vx,int vy, int omega);
void Motor(float m, int p, int pwm);




#endif