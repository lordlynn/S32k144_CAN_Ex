#ifndef TIMERINTERRUPT_H_
#define TIMERINTERRUPT_H_

/** How many Systick interrupts have happened since startup **/
extern uint32_t count;


/***********************************************************************
 * Initializes the systick module to interrupt consistently after the
 * 		supplied delay in ms. The count variable is incremented on every
 * 		interrupt.
 *
 * @param delay Time in ms to delay wait interrupts
 * @return void
 **********************************************************************/
void start_systick(double delay);


/** Creates a blocking delay in ms **/
void systick_delay(double delay);


#endif 																		/* TIMERINTERRUPT_H_ */
