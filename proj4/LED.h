
#ifndef LED_H
#define LED_H


#define uchar unsigned char
#define LedPinRed	0
#define LedPinGreen	1
#define LedPinBlue	2
#define Pressed		1
#define UnPressed	0

void ledInit(void);


void ledColorSet(uchar r_val, uchar g_val, uchar b_val);


void ledUpdate(uchar* r_val, uchar* g_val, uchar* b_val, int red_pressed, int green_pressed, int blue_pressed, int black_pressed);

#endif
