#ifndef STM32F4_DRIVER_H
#define STM32F4_DRIVER_H
//////////////////////////////////////// processor dependent defines and declarations //////////////////////////////////////////
    //--------------------------------------------------------------------------------------------------------------
#define max(var1,var2) ((var1>var2)?var1:var2)
//vvvvvvvvvvvvvvvvvvvvvvvvvv STM32 processors vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
#define __STM32__
#define IS_32BIT
#define IRAM_ATTR       // delete in .cpp files, because it has no meaning for STM32 processors
#define DRAM_ATTR
#define MOTOSOFTLED32		// use 32-bit version of SoftLed class

/*#include <libmaple/timer.h>
#include <libmaple/spi.h>
#include <libmaple/nvic.h>
*/

#define CYCLETIME       1     // Cycle count in µs on 32Bit processors

#define TIMER_PRESCALER (F_CPU/2000000)  //  = 2MHz timer clock, 0.5us per tic
#define TICS_PER_MICROSECOND 2 //  = 0.5us

#define MT_TIMER TIM4     // Timer used by MobaTools
#define STEP_CHN    1       // OCR channel for Stepper and Leds
#define SERVO_CHN   2       // OCR channel for Servos
#define GET_COUNT mtTimer.getCount()

extern bool timerInitialized;
void seizeTimer1();
#define USE_SPI2          // Use SPI1 if not defined
#ifndef PIN_SPI_SS
#define PIN_SPI_SS PA4
#endif
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ STM32F4 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#define ARCHITECT_INCLUDE <stm32/MoToSTM32.h>
#endif
