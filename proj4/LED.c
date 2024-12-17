#include <softPwm.h>
#include <wiringPi.h>
#include <softPwm.h>

#include "LED.h"

void ledInit(void){
	softPwmCreate(LedPinRed,0,0);
	softPwmCreate(LedPinGreen,0,0);
	softPwmCreate(LedPinBlue,0,0);
	pinMode(LedPinRed,OUTPUT);
	pinMode(LedPinGreen,OUTPUT);
	pinMode(LedPinBlue,OUTPUT);

}
void ledColorSet(uchar r_val, uchar g_val, uchar b_val){
	softPwmWrite(LedPinRed, r_val);
	softPwmWrite(LedPinGreen, g_val);
	softPwmWrite(LedPinBlue, b_val);
}

void ledUpdate(uchar* r_val, uchar* g_val, uchar* b_val, int red_pressed, int green_pressed, int blue_pressed, int black_pressed){
	float mult = .7;
	float remainder = 1 - mult;
	float max = 255.0;
	if (red_pressed == Pressed){
	*r_val = (int) ((float)*r_val * mult + max * remainder);
	}
	if (green_pressed == Pressed){
	*g_val = (int) ((float)*g_val * mult + max * remainder);
	}
	if (blue_pressed == Pressed){
	*b_val = (int) ((float)*b_val * mult + max * remainder);
	}
	if (black_pressed == Pressed){
	*r_val = (uchar) (*r_val / 2); 
	*g_val = (uchar) (*g_val / 2);
	*b_val = (uchar) (*b_val / 2);
	}
}	
