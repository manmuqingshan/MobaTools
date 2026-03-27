#pragma once
//////////////////////////////////////// processor dependent defines and declarations //////////////////////////////////////////
//------------------------------- STM32 core ------------------------------------------------------------
#define max(var1,var2) ((var1>var2)?var1:var2)
//vvvvvvvvvvvvvvvvvvvvvvvvvv STM32 processors vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
#define __STM32__
#define IS_32BIT
#define IRAM_ATTR       // delete in .cpp files, because it has no meaning for STM32 processors
#define DRAM_ATTR
#define MOTOSOFTLED32		// use 32-bit version of SoftLed class

#define CYCLETIME       1     // Cycle count in µs on 32Bit processors
#ifndef MIN_STEP_CYCLE
	#define MIN_STEP_CYCLE       25      // Min. irq-periode in us (
#endif								

#define TIMER_PRESCALER (F_CPU/2000000)  //  = 2MHz timer clock, 0.5us per tic
#define TICS_PER_MICROSECOND 2 //  = 0.5us

#define MT_TIMER TIM4     // Timer used by MobaTools
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

