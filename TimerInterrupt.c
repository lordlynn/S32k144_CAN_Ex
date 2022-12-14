#include "S32K144.h"
#include "TimerInterrupt.h"
//#include "fsl_core_cm4.h"

/** Pointer to Systick **/
#define SysTick ((SysTick_Type *) (0xE000E010UL))

/** Defines the systick registers **/
typedef struct {
	/** Systick control register **/
	__IO uint32_t CTRL;

	/** Systick max count value **/
	__IO uint32_t LOAD;

	/** Current Systick counter value **/
	__IO uint32_t VAL;

	/** Systick calibration **/
	__IO uint32_t CALIB;

} SysTick_Type;

/** How many Systick interrupts have happened since startup **/
uint32_t count = 0;

void start_systick(double delay) {
//	DISABLE_INTERRUPTS();

	SysTick->LOAD = delay * 40000; 												// Set SysTick timer load value. 40Mhz = 40000 cycles per ms

	SysTick->CTRL = 0x7; 										    			// Start SysTick timer with Interrupt using bus/core clock

//	ENABLE_INTERRUPTS();

}

/***********************************************************************
 * Called on every systick interrupt. Increments the value of count on
 * 		every interrupt as a means of keeping time since startup.
 *
 * @param void
 * @return void
 **********************************************************************/
void SysTick_Handler(void ) {
	count++;
}


void systick_delay(double delay) {
	uint32_t temp = count;
	while (count - temp < delay);

}
