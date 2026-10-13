/*
 * uart1.h
 *
 * Created: 2023-03-20 오전 9:50:44
 *  Author: kccistc
 */ 


#ifndef UART1_H_
#define UART1_H_

volatile unsigned char rx1_buff[100];
volatile int i_bt=0;   
volatile uint8_t rx1_ready_flag=0;  // \n을 만나면 1로 set된다.  uint8_t : unsigned char




#endif /* UART1_H_ */