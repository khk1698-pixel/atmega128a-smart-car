/*
 * uart0.h
 *
 * Created: 2023-03-20 오전 9:49:45
 *  Author: kccistc
 */ 


#ifndef UART0_H_
#define UART0_H_
volatile unsigned char rx_buff[100];
volatile int i=0;
volatile uint8_t rx_ready_flag=0;  // \n을 만나면 1로 set된다.  uint8_t : unsigned char




#endif /* UART0_H_ */