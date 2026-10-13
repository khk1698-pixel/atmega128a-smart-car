/*
 * led.c
 *
 * Created: 2023-03-20 오전 11:50:37
 *  Author: kccistc
 */ 
#include "led.h"

void init_led()
{
	AUTO_RUN_LED_PORT_DDR |= 1 << AUTO_RUN_LED_PIN;
}