#include "S32K144.h" 															// Include peripheral declarations S32K144
#include "CAN.h"
#include "ClockInit.h"
#include <stdio.h>
#include <stdlib.h>
#include "TimerInterrupt.h"

void listener(void);
void speaker(void);
void example(void);
void print_data(void);

int all_on();
int all_off();
int tune_LED();

void WDOG_disable (void){
	WDOG->CNT = 0xD928C520; 													// Unlock watchdog
	WDOG->TOVAL = 0x0000FFFF; 													// Maximum timeout value
	WDOG->CS = 0x00002100; 														// Disable watchdog
}

void PORT_init (void) {
	PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK; 							// Enable clock for PORTD
	PORTD->PCR[16] = PORT_PCR_MUX(1); 											// Port D16: MUX = GPIO (to green LED)
	PTD->PDDR |= 1 << 16; 														// Port D16: Data direction = output
	PTD->PDOR |= 1 << 16;														// set port outputs to HIGH which disables LEDs
}

int main(void) {
	WDOG_disable();
	SOSC_init_8MHz(); 															// Initialize system oscillator for 8 MHz xtal
	SPLL_init_160MHz(); 														// Initialize SPLL to 160 MHz with 8 MHz SOSC
	NormalRUNmode_80MHz(); 														// Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash
	CAN0_init(); 																// Init FlexCAN0


	PORT_init(); 																// Configure ports
	start_systick(1);															// start counter at 1ms intervals

	//	CAN0_transmit_msg();


	while (1) {
		listener();
		speaker();
	}
}



void speaker() {
	if (all_on())
		if (tune_LED())
			all_off();

}


void listener() {
	if (count % 50 == 0) {
		if ((CAN0->IFLAG1 >> 4) & 1) { 											// If CAN 0 MB 4 flag is set (received msg), read MB4
			CAN0_receive_msg(4); 												// Read message
			print_data();
		}
		if ((CAN0->IFLAG1 >> 5) & 1) {
			CAN0_receive_msg(5);
			print_data();
		}
		if ((CAN0->IFLAG1 >> 6) & 1) {
			CAN0_receive_msg(6);
			print_data();
		}
		count++;
	}
}

void print_data(void) {
	uint8_t msg_type = (RxID >> 26) & 0x7;
	uint8_t device_ID = (RxID >> 18) & 0x00FF;

	if (msg_type == 0) {
		printf("ID: %d\tType: initialization\tADC: %d\tTest: ", device_ID, RxDATA[0]);

		if (RxDATA[1] == 0) printf("VREF HIGH\t");
		else if (RxDATA[1] == 1) printf("VREF LOW\t");
		else if (RxDATA[1] == 2) printf("Bandgap v\t");

		printf("\tVoltage: %d\n", (RxDATA[2] << 8) | RxDATA[3]);

	}
	else if (msg_type == 3) {
		printf("ID: %d\tType: Good state change\tBTN: %c\tBTN State: %d\tA1 State: %d\tA2 State: %d\tD1 State: %d\t",
				device_ID, RxDATA[0], RxDATA[1], RxDATA[2], RxDATA[3], RxDATA[4]);
		printf("Time: %.3lfs\n", ((uint32_t)(RxDATA[5] << 24 | RxDATA[6] << 16 | RxDATA[7] << 8 | RxDATA[8])) / 1000.0);

	}
	else if (msg_type == 4) {
		printf("ID: %d\tType: Bad state change\tBTN: %c\tBTN State: %d\tA1 State: %d\t",
				device_ID, RxDATA[0], RxDATA[1], RxDATA[2]);
		printf("A1 voltage: %.3lf\tA2 State: %d\t", (RxDATA[3] << 8 | RxDATA[4]) / 1000.0, RxDATA[5]);
		printf("A2 voltage: %.3lf\tD1 State: %d\t", (RxDATA[6] << 8 | RxDATA[7]) / 1000.0, RxDATA[8]);
		printf("Time: %.3lfs\n", ((uint32_t)(RxDATA[9] << 24 | RxDATA[10] << 16 | RxDATA[11] << 8 | RxDATA[12])) / 1000.0);
	}
}


int all_on() {
	static int first = 1;
	static uint8_t sequence = 0;
	static uint8_t ftm[] = {0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 4, 3, 2, 1};
	static uint8_t ch[] = {2, 3, 5, 6, 7, 5, 3, 2, 1, 0, 4, 5, 7, 5, 5};
	static uint8_t state[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

	if (sequence > 14) return 1;

	if (count % 500 == 0) {
		if (((CAN0->IFLAG1 & 0x1) || first) && sequence < 15) {
			if (first) first = 0;
			CAN0_transmit_msg((unsigned char[]) {ftm[sequence], ch[sequence], state[sequence]}, 3, 1);
			sequence++;
		}
	}
	return 0;
}


int all_off() {
	static int first = 1;
	static uint8_t sequence = 0;
	static uint8_t ftm[] = {0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 4, 3, 2, 1};
	static uint8_t ch[] = {2, 3, 5, 6, 7, 5, 3, 2, 1, 0, 4, 5, 7, 5, 5};
	static uint8_t state[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	if (sequence > 14) return 1;

	if (count % 500 == 0) {
		if (((CAN0->IFLAG1 & 0x1) || first) && sequence < 15) {
			if (first) first = 0;
			CAN0_transmit_msg((unsigned char[]) {ftm[sequence], ch[sequence], state[sequence]}, 3, 1);
			sequence++;
		}
	}
	return 0;
}


int tune_LED() {
	static int first = 1;
	static uint8_t sequence = 0;
	static uint8_t ftm[] = {0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 4, 3, 2, 1};
	static uint8_t ch[] = {2, 3, 5, 6, 7, 5, 3, 2, 1, 0, 4, 5, 7, 5, 5};
	static uint8_t dc[] = {0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5};

	if (sequence > 14) return 1;

	if (count % 500 == 0) {
		if (((CAN0->IFLAG1 & 0x1) || first) && sequence < 15) {
			if (first) first = 0;
			CAN0_transmit_msg((unsigned char[]) {ftm[sequence], ch[sequence], dc[sequence]}, 3, 2);
			sequence++;
		}
	}
	return 0;
}
