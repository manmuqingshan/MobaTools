// STM32F1 HW-spcific Functions
#ifdef ARDUINO_ARCH_STM32F1
#define bool int
#include <MobaTools.h>
//#define debugTP
//#define debugPrint
#include <utilities/MoToDbg.h>

//#warning "HW specfic - STM32F1 ---"

uint8_t noStepISR_Cnt = 0;   // Counter for nested StepISr-disable

void stepperISR(int32_t cyclesLastIRQ)  __attribute__ ((weak));
void softledISR(uint32_t cyclesLastIRQ)  __attribute__ ((weak));
nextCycle_t nextCycle;
static nextCycle_t cyclesLastIRQ = 1;  // cycles since last IRQ
void ISR_Stepper() {
    // Timer4 Channel 1, used for stepper motor and softleds, starts every nextCycle us
    // nextCycle ist set in stepperISR and softledISR
    SET_TP1;
    nextCycle = ISR_IDLETIME  / CYCLETIME ;// min ist one cycle per IDLETIME
    if ( stepperISR ) stepperISR(cyclesLastIRQ);
    //============  End of steppermotor ======================================
    if ( softledISR ) softledISR(cyclesLastIRQ);
    // ======================= end of softleds =====================================
    // set compareregister to next interrupt time;
	// next ISR must be at least MIN_STEP_CYCLE/4 beyond actual counter value ( time between to ISR's )
	int minOCR = timer_get_count(MT_TIMER);
	int nextOCR = timer_get_compare(MT_TIMER, STEP_CHN);
	if ( minOCR < nextOCR ) minOCR += TIMER_OVL_TICS; // timer had overflow already
    minOCR = minOCR + ( (MIN_STEP_CYCLE/4) * TICS_PER_MICROSECOND ); // minimumvalue for next OCR
	nextOCR = nextOCR + ( nextCycle * TICS_PER_MICROSECOND );
	if ( nextOCR < minOCR ) {
		// time till next ISR ist too short, set to mintime and adjust nextCycle
        SET_TP2;
		nextOCR = minOCR;
		nextCycle = ( nextOCR - timer_get_compare(MT_TIMER, STEP_CHN)  ) / TICS_PER_MICROSECOND;
        CLR_TP2;
	}
    if ( nextOCR > (uint16_t)TIMER_OVL_TICS ) nextOCR -= TIMER_OVL_TICS;
    timer_set_compare( MT_TIMER, STEP_CHN, nextOCR ) ;
    cyclesLastIRQ = nextCycle;
    CLR_TP1; // Oszimessung Dauer der ISR-Routine
}
////////////////////////////////////////////////////////////////////////////////////////////
void seizeTimerAS() {
    static bool timerInitialized = false;
    if ( !timerInitialized ) {
		uint8_t timer_type = GPT_TIMER;
		tindex = FspTimer::get_available_timer(timer_type);
		if (tindex < 0) {
			tindex = FspTimer::get_available_timer(timer_type, true);
		}
		if (tindex < 0) return false;
		}
		// compute pointer to active timer registers
		gptRegP = (R_GPT0_Type *)((uint8_t *)gpt0RegP + (0x100 * tindex));
		// compute ISR event numbers CMPA and CMPB
		//icuEventOvf = evGPT0_OVF + (tindex * evGPT_OFSET);
		icuEventCmpA = evGPT0_CCMPA + (tindex * evGPT_OFSET);
		icuEventCmpB = evGPT0_CCMPB + (tindex * evGPT_OFSET);

		MoTo_timer.begin(TIMER_MODE_PERIODIC, timer_type, tindex, 60000, 20000, TIMER_SOURCE_DIV_16)) 
		gptRegP->GTBER = 0x3;  // no Buffer operation

		MoTo_timer.setup_capture_a_irq(5, IRQ_CmpA)) 
		MoTo_timer.setup_capture_b_irq(5, IRQ_CmpB))

		// determin ICU-Index for GPT-Interrupts. this is needed to reset the correct IRQ-Flag 
		// within the ISR
		for (byte i = 1; i < 32; i++) {
		uint32_t tmp;
		tmp = (uint32_t)(icuRegP->IELSR[i]);
		if (icuRegP->IELSR_b[i].IELS == icuEventCmpA) icuIxCmpA = i;
		if (icuRegP->IELSR_b[i].IELS == icuEventCmpB) icuIxCmpB = i;
		DB_PRINT ( "IrqIx=%d, Value= 0X%lX, Event=0X%X", i, tmp, (uint32_t)(icuRegP->IELSR_b[i].IELS));
		DB_PRINT( "OvfIx=%d, CmpaIx=%d, CmpbIx=%d", icuIxOvf, icuIxCmpA, icuIxCmpB);

		MoTo_timer.open())
		/*gptRegP->GTADTRA = 20000;
		gptRegP->GTPR = 50000;
		*/

		MoTo_timer.start())
		gptRegP->GTBER = 0x3;  // no Buffer operation

		gptRegP->GTPR = 60000;			// set max timer count ( 20ms loop time )
		gptRegP->GTCCR[0] = 20000;		// initial cmp for stepper/Softled
		gptRegP->GTCCR[1] = 10000;		// initial cmp for servos
		
		timerInitialized = true;
	}
}


void enableServoIsrAS() {
}

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



#endif
