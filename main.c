
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/FPU.h"



int Clock;
int PWMcounter = 0,PWMcounterOV,PWMduty = 0;
float low_State;
float high_State;
float period;
float clockPeriod;
unsigned int high_Ticknum;
unsigned int low_Ticknum;
void Timer0IntHandler_1(void);
void Timer0IntHandler(void);
void Timer0Conf(void);
void PWMConf(unsigned int freq,unsigned int duty_Cycle);

/*
		Timer0 interrupt handler
		interrupt handler for low timing of the PWM signal
*/

void Timer0IntHandler(void){
	
	TimerIntClear(TIMER0_BASE,INT_TIMER0A);																								// Clear the interrupt flag
	GPIOPinWrite(GPIO_PORTF_BASE,(GPIO_PIN_1),(GPIO_PIN_1));															// Write to pin	
	IntMasterDisable();
	TimerLoadSet(TIMER0_BASE, TIMER_A, high_Ticknum);	
	TimerIntRegister(TIMER0_BASE,TIMER_A,Timer0IntHandler_1);		                         //assign timer0 interurpt to Timer0IntHandler_1 for low timing
	TimerEnable(TIMER0_BASE, TIMER_A);	
	IntMasterEnable();	
	PWMcounter++;                    //increase PWMcounter by 1
	
	if(PWMcounter >= PWMcounterOV){	 //if PWMcounter exceeds PWMcounterOV then increase duty cycle,and if the duty cycle is at max. reset duty cycle to 0
		
		if(PWMduty<100){	
			PWMduty = PWMduty + 5;
			PWMConf(100,PWMduty);
			PWMcounter = 0;
			}
		
		if(PWMduty>=100){
			PWMduty = 0;
			PWMConf(100,PWMduty);
			PWMcounter = 0;
			}
	}
}

/*
		Timer0 interrupt handler
		interrupt handler for low timing of the PWM signal
*/
void Timer0IntHandler_1(void){

  TimerIntClear(TIMER0_BASE,INT_TIMER0A);																							// Clear the interrupt flag
	GPIOPinWrite(GPIO_PORTF_BASE,(GPIO_PIN_1),~(GPIO_PIN_1));														// Write to pin
	IntMasterDisable();
	TimerLoadSet(TIMER0_BASE, TIMER_A, low_Ticknum);
	TimerIntRegister(TIMER0_BASE,TIMER_A,Timer0IntHandler);															//assign timer0 interurpt to Timer0IntHandler for high timing	
	TimerEnable(TIMER0_BASE, TIMER_A);
	IntMasterEnable();
	
}

int main(void)
{	
	FPUEnable();
	
	FPULazyStackingEnable();
	//Setting the clock frequency	
	SysCtlClockSet(SYSCTL_SYSDIV_3 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);  // Refer to tm4c123gh6pm data sheet page 223
																																													 // and TivaWare™ Peripheral Driver Library refrence page: 489 for more options
																																													 // In this case the clock freq is 66.67 MHz
	
	clockPeriod = (float)1/SysCtlClockGet();																								 // Calculate the time between clock ticks
	
	PWMConf(100,PWMduty);
	
	//Get the clock
	Clock = SysCtlClockGet();           //In case you want to confirm the clock frequency
	
	
	//Setup peripherals
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);																							// enable timer0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);																							// enable PORTF
	
	//set outputs
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, (GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_3)); 			// set pins of portf as output
	
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4 ,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPinWrite(GPIO_PORTF_BASE,(GPIO_PIN_1),GPIO_PIN_1);
	//Enable global interrupt
	IntMasterEnable();
	
	
	//Timer0 configuration
	Timer0Conf();
	
	while(1){
		;
		
		}
	}
	
	/*

	Function Name: confPWM
	Function Pupose: calculates the PWM parameters for given frequency and duty cycle.
									 Firstly, period of the PWM is calculated, high time and low time is calculated in seconds
									 for given frequency and duty cycle and these values are stored in low_State and high_State variables.
									 By using these variables and clockPeriod the number of clock ticks are calculated and stored in high_Ticknum and low_Ticknum.
	
*/

void PWMConf(unsigned int freq,unsigned int duty_Cycle){
	
	PWMcounterOV = freq/20;																		// PWM counter overflow number for dimming
	period = (float)1/freq;																		//convert frequency to period
	high_State =  period*((float)duty_Cycle/100);							//find high time in senconds for the given duty cycle
	low_State =  period*(1.0-(float)duty_Cycle/100);					//find low time in seconds for the given duty cycle
	high_Ticknum = high_State/clockPeriod;										//convert time to tick number.	
	low_Ticknum = low_State/clockPeriod;
	
}


void Timer0Conf(void){

	TimerConfigure(TIMER0_BASE,(TIMER_CFG_ONE_SHOT ));																//  one-shot down timer
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/500);															// load the value of the counter.Loading a timer with SysCtlClockGet() gives 1 second of delay 																																													// and in the same way SysCtlClockGet()/2 give half a second of delay.
	
	
	// setup the interrupt for timers
	
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);															//Timeout interrupt
	TimerIntRegister(TIMER0_BASE,TIMER_A,Timer0IntHandler);												// assign interrupt vector to a function
	TimerEnable(TIMER0_BASE, TIMER_A);																						// start timer
	
}

