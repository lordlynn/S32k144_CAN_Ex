
#ifndef CAN_H_
#define CAN_H_

#define TARGET_DEVICE_ID 1

uint32_t RxCODE; 									/* Received message buffer code */
uint32_t RxID; 										/* Received message ID */
uint32_t RxLENGTH; 									/* Received message number of data bytes */
unsigned char RxDATA[64]; 							/* Received message data (max of 64 bytes) */
uint32_t RxTIMESTAMP; 								/* Received message time */
int BRS;											// flag is 1 if BRS is enabled

void CAN0_init(void);
void CAN0_BRSen(void);
void CAN0_BRSdis(void);

void CAN0_transmit_msg(const unsigned char data[], int len, uint8_t msg_type);
void CAN0_receive_msg(int buff);
void port_init(void);

#endif /* CAN_H_ */
