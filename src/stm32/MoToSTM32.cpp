// STM32 HW-spcific Functions (ST-Microelectronics core)
#ifdef ARDUINO_ARCH_STM32

#define debugTP
//#define debugPrint
#include <MobaTools.h>

#if MESSAGES > 0
	#pragma message "compiling MoToSTM32.cpp"
#endif
HardwareTimer mtTimer(MT_TIMER);	// create MobaTools timer instance
TIM_HandleTypeDef *mtTimerHandle;	// needed to disable/enable stepper IRQ 
uint32_t stepChanIT;				// needed to disable/enable stepper IRQ
uint8_t noStepISR_Cnt = 0;   // Counter for nested StepISr-disable
uint16_t minTicDiff;		// is set here as variable

void stepperISR(nextCycle_t cyclesLastIRQ)  __attribute__ ((weak));
void softledISR(nextCycle_t cyclesLastIRQ)  __attribute__ ((weak));
nextCycle_t nextCycle;
static nextCycle_t cyclesLastIRQ = 1;  // cycles since last IRQ
void ISR_Stepper() {
    // Channel 1, used for stepper motor and softleds, starts every nextCycle us
    // nextCycle ist set in stepperISR and softledISR
    SET_TP1;
    nextCycle = ISR_IDLETIME  / CYCLETIME ;// min ist one cycle per IDLETIME
    if ( stepperISR ) stepperISR(cyclesLastIRQ);
    //============  End of steppermotor ======================================
    if ( softledISR ) softledISR(cyclesLastIRQ);
    // ======================= end of softleds =====================================
    // set compareregister to next interrupt time;
	uint16_t actCompare = mtTimer.getCaptureCompare(STEP_CHN);
	uint16_t add2Ocr = nextCycle * TICS_PER_MICROSECOND; // tics to add to current compare reg
	uint16_t minDiff = (mtTimer.getCount()+minTicDiff) - actCompare;
	if (  minDiff >= add2Ocr ) {
		// counter is already too far
        //CLR_TP2;
		add2Ocr = minDiff;
		nextCycle = add2Ocr / TICS_PER_MICROSECOND;
        //SET_TP2;
	}
	
    mtTimer.setCaptureCompare(STEP_CHN, actCompare+add2Ocr ) ;
    cyclesLastIRQ = nextCycle;
    CLR_TP1; // Oszimessung Dauer der ISR-Routine
}
////////////////////////////////////////////////////////////////////////////////////////////
void seizeTimerAS() {
    static bool timerInitialized = false;
        MODE_TP1;
        MODE_TP2;
        MODE_TP3;
        MODE_TP4;
    if ( !timerInitialized ) {
		SET_TP2;
        //mtTimer.init( MT_TIMER );
        mtTimer.pause();
        // IRQ-Priorität des timer interrupt auf lowest (15) setzen
        mtTimer.setInterruptPriority( 15, 15); // These long lasting IRQ's MUST be lowest priority
        mtTimer.setPrescaleFactor(TIMER_PRESCALER);    // = 0.5µs Tic
        mtTimer.setOverflow(0x10000);  //set the period (overflow at max)
        //mtTimer.setCaptureCompare( STEP_CHN, 400 );
        //mtTimer.setCaptureCompare(SERVO_CHN, FIRST_PULSE );
		stepChanIT = mtTimer.getIT(STEP_CHN);
        mtTimer.refresh();
        mtTimer.resume();
		mtTimerHandle = mtTimer.getHandle(); 
        timerInitialized = true;  
		
		// set the min gap between two ISR ( end - to start of next ) depending on the
		// clock frequency ( faster CPU's can have a smaller gap )
		if ( F_CPU > FAST_CLOCK ) {
			minTicDiff = TICS_PER_MICROSECOND * ISR_GAP;
		} else {
			minTicDiff = MIN_TIC_DIFF; // default
		}
		CLR_TP2;
    }
}


//void enableServoIsrAS() { }

/*
extern "C" {
// ------------------------  ISR for SPI-Stepper ------------------------
static int rxData;
#ifdef USE_SPI2
void __irq_spi2(void) {// STM32  spi2 irq vector
    rxData = spi_rx_reg(SPI2);            // Get dummy data (Clear RXNE-Flag)
    digitalWrite(BOARD_SPI2_NSS_PIN,HIGH);
}
#else
void __irq_spi1(void) {// STM32  spi1 irq vector
    //SET_TP4;
    rxData = spi_rx_reg(SPI1);            // Get dummy data (Clear RXNE-Flag)
    digitalWrite(BOARD_SPI1_NSS_PIN,HIGH);
    //CLR_TP4;
}
#endif
} // end of extern "C"
*/


#endif
