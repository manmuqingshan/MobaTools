#pragma once
//////////////////////////////////////// processor dependent defines and declarations //////////////////////////////////////////
//------------------------------- STM32 core ------------------------------------------------------------
#define max(var1,var2) ((var1>var2)?var1:var2)  // Arduino macro doesn't work
//vvvvvvvvvvvvvvvvvvvvvvvvvv STM32 processors vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
#define __STM32__
#define IS_32BIT
#define IRAM_ATTR       // delete in .cpp files, because it has no meaning for STM32 processors
#define DRAM_ATTR
#define MOTOSOFTLED32		// use 32-bit version of SoftLed class

#define CYCLETIME       1     // Cycle count in µs on 32Bit processors
#define MIN_STEP_CYCLE       minStepCycle // this is a variable in STM32
// Min. step time us (

#define TIMER_PRESCALER (F_CPU/2000000)  //  = 2MHz timer clock, 0.5us per tic
#define TICS_PER_MICROSECOND 2 //  = 0.5us
#define FAST_CLOCK	60000000	// CPU's with faster clock can have a smaller gap between ISR's
#define FAST2_CLOCK 200000000	// even faster CPU's (STM32H7xxx)
#define ISR_GAP 5				// gap between two ISR in µs for fast CPUs
#define ISR2_GAP 3				// gap between two ISR in µs for very fast CPUs
#define MIN_STEP_TIME1	40		// for Clock < FAST_CLOCK
#define MIN_STEP_TIME2	25		// for Clock < FAST2_CLOCK
#define MIN_STEP_TIME3	10		// for clock > FAST2_CLOCK
// select the timer used by MobaTools ( highest available general purpose timer )
#ifdef TIM4
	#define MT_TIMER TIM4     // Timer used by MobaTools
	#if MESSAGES>0
	#pragma message "using timer TIM4"
	#endif
#elif defined TIM3
	#define MT_TIMER TIM3     // Timer used by MobaTools
	#if MESSAGES>0
	#pragma message "using timer TIM3"
	#endif
#elif defined TIM2
	#define MT_TIMER TIM2     // Timer used by MobaTools
	#if MESSAGES>0
	#pragma message "using timer TIM2"
	#endif
#else
	#error "CPU not supported, no suitable timer"
#endif
#define STEP_CHN    1       // OCR channel for Stepper and Leds
#define SERVO_CHN   2       // OCR channel for Servos
#define GET_COUNT mtTimer.getCount()
#define SET_SPI_PINS	// SPI-SS pin can be set individually

extern bool timerInitialized;
void seizeTimer1();
#ifndef PIN_SPI_SS
#define PIN_SPI_SS PA4
#endif
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ STM32F4 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#define ARCHITECT_INCLUDE <stm32/MoToSTM32.h>

