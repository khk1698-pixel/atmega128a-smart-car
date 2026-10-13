/*
 * uart1.c
 *
 * Created: 2023-03-20 오전 9:50:21
 *  Author: kccistc
 */ 
#define  F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>   // for interrupt
#include <string.h>   // strncmp strcpy등
#include "uart1.h"
/*
1. 전송속도: 9600bps : 9600 / 10bit(st/stop포함) ==> 960자(1글자 송.수신 소요시간: 약 1ms)
2. 비동기(start/stop bit가지고 data구분), data 8bit none parity)
3. RX(수신) interrupt 활성화  
*/

void init_uart1(void)
{
	UBRR1H = 0x00;
	UBRR1L = 207;   // 9600bps P219 표 9-9
	
	UCSR1A |= (1 << U2X1);  // 2배속 통신 
	UCSR1C |= 0x06;   // ASYNC(비동기: st/stop으로 data구분) 8 data, none parity
	
	// RXEN1 : UART1로 부터 수신이 가능 하도록 설정
	// TXEN1 : UART1로 부터 송신 가능 하도록 설정
	// RXCIE1: UART1로 부터 1 byte가 들어 오면 rx(수신) interrupt를 띄어 달라
	UCSR1B = ( (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1));
}

// UART1로 부터 1byte가 들어 오면 이곳으로 진입 한다. 
// ISR(USART1_RX_Vect)
volatile unsigned char uart1_data;
extern void UART0_transmit(uint8_t data);
ISR(USART1_RX_vect)
{
	uart1_data = UDR1;   // uart1의 H/W register(UDR1)로 부터 1 byte를 읽어 들인다.	

	UART0_transmit(uart1_data);
} 

// UART1 1 byte를 전송 하는 함수
void UART1_transmit(uint8_t data)
{
	
	while ( !(UCSR1A & ( 1 << UDRE1)))  // data가 전송중이면 전송이 완료 될때까지 기다린다.
		;   // no operation (아무 action도 취하지 않는다)
	UDR1 = data; 
} 

static char word_table[][20] =
{
	"ledallon",
	"ledalloff"
};

void bt_command_processing()
{
	if (rx1_ready_flag >= 1)   // if (rx_ready_flag >= 1)
	{
		rx1_ready_flag=0;

		if (strncmp(rx1_buff+1, (char *) &word_table[0][1],7) == 0 )
		{
			PORTA = 0xff;
		}
		if (strncmp(rx1_buff+1, (char *) &word_table[1][1],7) == 0 )
		{
			PORTA = 0x00;
		}
	}
}