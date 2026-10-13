/*
 * ultrasonic.h
 *
 * Created: 2023-03-22 오전 10:18:15
 *  Author: kccistc
 */ 


#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_
#define  F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>   // for interrupt

#define TRIG 4
#define TRIG_PORT  PORTG
#define TRIG_DDR   DDRG

#define ECHO_PIN 4
#define ECHO_PORT  PINE
#define ECHO_DDR   DDRE


#endif /* ULTRASONIC_H_ */