#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <signal.h>

#define uchar unsigned char
#define ButtonPinRed 	3
#define ButtonPinGreen 	4
#define ButtonPinBlue 	5
#define ButtonPinBlack 	6
#define Pressed		1
#define UnPressed	0
#define __NR_wait 399

void sighandler(int signum){
	printf("BUTTONS : WAITING\n");
	syscall(__NR_wait);
}
int main(void){
	// When initialize wiring failed, print message to screen
	if(wiringPiSetup() == -1){
		printf("setup wiringPi failed !");
		return 1; 
	}
	int red_state = UnPressed, green_state = UnPressed, blue_state = UnPressed;
	int black_state = UnPressed;
	uchar initial_intensity =0;
	uchar red_intensity = initial_intensity;
	uchar green_intensity = initial_intensity;	
	uchar blue_intensity = 255;
	signal(SIGUSR1,sighandler);
	printf("STARTING BUTTONS/LED TASK WITH PID: %d\n",getpid());
	ledInit();
	ledColorSet(red_intensity, green_intensity, blue_intensity);
	pinMode(ButtonPinRed, INPUT);
	pinMode(ButtonPinGreen, INPUT);
	pinMode(ButtonPinBlue, INPUT);
	pinMode(ButtonPinBlack, INPUT);
	
	while(1){
		int r_p = 0, g_p = 0, b_p = 0, bla_p = 0;
		// Indicate that button has pressed down
		if(digitalRead(ButtonPinRed) != red_state){
			printf("...Red Pressed\n");
			red_state = !red_state;
			r_p = 1;
			printf("%d %d %d\n", red_intensity, green_intensity, blue_intensity);
		}
		if(digitalRead(ButtonPinGreen) != green_state){
			green_state = !green_state;
			g_p = 1;
		}
		if(digitalRead(ButtonPinBlue) != blue_state){
			blue_state = !blue_state;
			b_p = 1;
		}
		if (digitalRead(ButtonPinBlack) != black_state){
			black_state = !black_state;
			bla_p = 1;
		}
		ledUpdate(&red_intensity, &green_intensity, &blue_intensity, r_p, g_p, b_p, bla_p);
		ledColorSet(red_intensity, green_intensity, blue_intensity);
	//delay(200);
	}
	return 0;
}

