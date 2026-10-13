#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define BUTTON_DDR DDRC
#define BUTTON_PIN  PINC  // PORC값을 읽어 들이는 register
// 1: 5v 0: 0v
#define BUTTON1  7   // PORTC.7