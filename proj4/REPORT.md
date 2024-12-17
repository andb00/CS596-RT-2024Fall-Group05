# GROUP 05 Report

### Description

We wrote 4 C Executables. reserve.c and cancel.c compile into 2 C scripts which set and cancel a reservation given a pid, and for reserve, two ints for the C and T of the reservation. When a reservation task exceeds its budget, the kernel sends a SIGUSR1. Our other 2 executables are meant to model real time tasks. SummationTask.c is a while loop adding a random number between 1 and 2 to an incrementer indefinitely. 

Buttons.c also has a while loop, which polls 4 buttons. It keeps states of each buttons and when they are toggled to pressed, a pressed flag is set to 1. At the end of the while loop, Buttons calls LED.h's LedColorSet function which updates an RGB Led with color values. LED.h and LED.c contains helper functions for initializing the GPIO for the LED, updating the LED, and a function to change color values given some button flags. The update function increases the intensity of red, green, and blue intensities when their respective button is clicked. The black button reduces the intensity of all 3.

Both real time tasks handle SIGUSR1 by calling the wait syscall and blocking until their period is over.

Our circuit design included 4 buttons, 1 rgb LED, and some resistors.

### Challenges

We faced numerous challenges. Brainstorming was difficult, and some of the good ideas were already done in the raphael-kit link. We couldn't get the LED circuit to work. It was working at one point, and then stopped. Since neither of us understand circuitry, this was confusing. We wanted to use Interprocess communication to have the buttons and LED be the 2 tasks, however shared memory simply didn't work, and pipes wouldn't work with polling. We addressed these challenges by pivoting since we couldn't fix them. 

### Contributions

Sawyer built the circuit, coded the button logic, and put it all together. Andrew thought of the idea for the LED task, coded LED.c and LED.h to simplify writing to GPIO, and wrote the SummationTask.
