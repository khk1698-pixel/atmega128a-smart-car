/*
 * led.h
 *
 * Created: 2023-03-20 오전 11:50:22
 *  Author: kccistc
 */ 


#ifndef LED_H_
#define LED_H_
#define  F_CPU 16000000UL
#include <avr/io.h>

#define AUTO_RUN_LED_PORT PORTG
#define AUTO_RUN_LED_PORT_DDR DDRG
#define AUTO_RUN_LED_PIN 3


#endif /* LED_H_ */