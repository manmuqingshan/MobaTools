#pragma once
// STM32 specific defines for Cpp files

#if MESSAGES > 0
	#pragma message "MMoToSTM32.h includes"
#endif

#include<SPI.h>
extern uint8_t noStepISR_Cnt;   // Counter for nested StepISr-disable

extern HardwareTimer mtTimer;
extern TIM_HandleTypeDef *mtTimerHandle; 
extern uint32_t stepChanIT;
extern uint8_t minStepCycle;
void ISR_Stepper();
void seizeTimerAS();


static inline __attribute__((__always_inline__)) void _noStepIRQ() {
    //disable stepper timerchannel;
    __HAL_TIM_DISABLE_IT(mtTimerHandle, stepChanIT);
    noStepISR_Cnt++;
    #if defined COMPILING_MOTOSTEPPER_CPP
        //Serial.println(noStepISR_Cnt);
        SET_TP3;
    #endif
}
static inline __attribute__((__always_inline__)) void  _stepIRQ(bool force = false) {
    //timer_enable_irq(MT_TIMER, TIMER_STEPCH_IRQ) cannot be used, because this also clears pending irq's
    if ( force ) noStepISR_Cnt = 1; //enable IRQ immediately
    if ( noStepISR_Cnt > 0 ) noStepISR_Cnt -= 1; // don't decrease if already 0 ( if enabling IRQ is called too often )
    if ( noStepISR_Cnt == 0 ) {
        #if defined COMPILING_MOTOSTEPPER_CPP
            CLR_TP3;
        #endif
		__HAL_TIM_ENABLE_IT(mtTimerHandle, stepChanIT);
	}
    //Serial.println(noStepISR_Cnt);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
#if defined COMPILING_MOTOSERVO_CPP
// Values for Servo: -------------------------------------------------------
constexpr uint8_t INC_PER_MICROSECOND = 8;		// one speed increment is 0.125 µs
constexpr uint8_t  COMPAT_FACT = 1; // no compatibility mode for stm32F4                       
// defaults for macros that are not defined in architecture dependend includes
constexpr uint8_t INC_PER_TIC = INC_PER_MICROSECOND / TICS_PER_MICROSECOND;
#define time2tic(pulse)  ( (pulse) *  INC_PER_MICROSECOND )
#define tic2time(tics)  ( (tics) / INC_PER_MICROSECOND )
#define AS_Speed2Inc(speed) (speed)
//-----------------------------------------------------------------

void ISR_Servo( void );


static inline __attribute__((__always_inline__)) void enableServoIsrAS() {
	mtTimer.attachInterrupt(SERVO_CHN, ISR_Servo);
    __HAL_TIM_ENABLE_IT(mtTimerHandle, mtTimer.getIT(SERVO_CHN) ); // needed?
}

static inline __attribute__((__always_inline__)) void setServoCmpAS(uint16_t cmpValue) {
	// Set compare-Register for next servo IRQ
	mtTimer.setCaptureCompare(SERVO_CHN, cmpValue );
}	

#endif // COMPILING_MOTOSERVO_CPP

/////////////////////////////////////////////////////////////////////////////////////////////////
#if defined COMPILING_MOTOSOFTLED32_CPP
static inline __attribute__((__always_inline__)) void enableSoftLedIsrAS() {
		mtTimer.attachInterrupt(STEP_CHN, ISR_Stepper); // Attach interrupt callback which will be called upon compare match event of specified channel        mtTimer.attach_interrupt(MT_TIMER, mtTimer.STEPCH_IRQ, (voidFuncPtr)ISR_Stepper );
    //__HAL_TIM_ENABLE_IT(mtTimerHandle, mtTimer.getIT(STEP_CHN) );
}

#endif // COMPILING_MOTOSOFTLED_CPP

//////////////////////////////////////////////////////////////////////////////////////////////////
#if defined COMPILING_MOTOSTEPPER_CPP
static byte spiSS;
#ifdef USE_SPI2
	SPIClass mtSPI(PB15,PB14,PB13,PB12); // defaults for SPI2 ( an all boards )
	#undef PIN_SPI_SS
	#define PIN_SPI_SS PB12
	#define SPI mtSPI
#endif

#ifdef ARDUINO_GENERIC_G030F6PX
	//Std SPI-Pins of GENERIC_G030F6PX don't work for WeAct STM32G030F6P6
	SPIClass mtSPI(PA7,PA6,PA5); // defaults for SPI1 ( NOT an all boards )
	#pragma message "SPI-Pins for STM32G030F6Px: MOSI=PA7,MISO=PA6,CLK=PA5,SS=PA4"
	#undef PIN_SPI_SS
	#define PIN_SPI_SS PA4
	#define SPI mtSPI
#endif



static inline __attribute__((__always_inline__)) void enableStepperIsrAS() {
	SET_TP2;
	mtTimer.attachInterrupt(STEP_CHN, ISR_Stepper); // Attach interrupt callback which will be called upon compare match event of specified channel        mtTimer.attach_interrupt(MT_TIMER, mtTimer.STEPCH_IRQ, (voidFuncPtr)ISR_Stepper );
    //__HAL_TIM_ENABLE_IT(mtTimerHandle, mtTimer.getIT(STEP_CHN) );
	CLR_TP2;
}

static uint8_t spiInitialized = false;
static inline __attribute__((__always_inline__)) void initSpiAS(byte ssPin = PIN_SPI_SS, byte clkPin = 255, byte mosiPin = 255 ) {
	// only SS-Pin can be really set
	(void)clkPin; (void)mosiPin; // to supress warning about unused parameters {
	DB_PRINT("ssPin=%d, spiSS=%d, PB12=%d,PA15=%d\n", ssPin,spiSS, PB12, PA15 );
    if ( spiInitialized ) return;
	SET_TP1;
	spiSS = ssPin;
    // initialize SPI hardware.
    // MSB first, default Clk Level is 0, shift on leading edge
	//TODO - initialize Samd SPI
	SPI.begin();	// Default SPI interface ( instantiated by default )
	pinMode(spiSS,OUTPUT);
	digitalWrite(spiSS,HIGH);
    spiInitialized = true;  
	CLR_TP1;
}

static inline __attribute__((__always_inline__)) void startSpiWriteAS( uint8_t spiData[] ) {
 	// TODO write step pattern over SPI
	// Actual without IRQ
	digitalWrite(spiSS,LOW);
	if ( MoToStepper::spi34Used() ) {
		SPI.transfer16( spiData[1]<<8 | spiData[0] );
	} else {
		SPI.transfer( spiData[0] );
	}
	digitalWrite(spiSS,HIGH);
   #ifdef USE_SPI2
	#else
	#endif
}    
    

#endif // COMPILING_MOTOSTEPPER_CPP

