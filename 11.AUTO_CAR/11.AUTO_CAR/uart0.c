/*
 * uart0.c
 *
 * Created: 2023-03-20 오전 9:50:04
 *  Author: kccistc
 */ 
#define  F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>   // for interrupt
#include <string.h>   // strncmp strcpy등
#include "uart0.h"
/*
1. 전송속도: 9600bps : 9600 / 10bit(st/stop포함) ==> 960자(1글자 송.수신 소요시간: 약 1ms)
2. 비동기(start/stop bit가지고 data구분), data 8bit none parity)
3. RX(수신) interrupt 활성화  
*/

void init_uart0(void)
{
	UBRR0H = 0x00;
	UBRR0L = 207;   // 9600bps P219 표 9-9
	
	UCSR0A |= (1 << U2X0);  // 2배속 통신 
	UCSR0C |= 0x06;   // ASYNC(비동기: st/stop으로 data구분) 8 data, none parity
	
	// RXEN0 : UART0로 부터 수신이 가능 하도록 설정
	// TXEN0 : UART0로 부터 송신 가능 하도록 설정
	// RXCIE0: UART0로 부터 1 byte가 들어 오면 rx(수신) interrupt를 띄어 달라
	UCSR0B = ( (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0));
}

// UART0로 부터 1byte가 들어 오면 이곳으로 진입 한다. 
// ISR(USART0_RX_Vect)
ISR(USART0_RX_vect)
{
	volatile unsigned char data;
	
	data = UDR0;   // uart0의 H/W register(UDR0)로 부터 1 byte를 읽어 들인다.	
 
	if (data == '\r' || data == '\n')
	{
		rx_buff[i] = '\0';   // 문장의 끝을 가리키는 NULL
		i=0;
		rx_ready_flag=1;   // \n을 만났다는 indicator
		                   // !!!!! 개선점 !!!!
						   // circular queuq(환형 queue)로 프로그램을 작성 해야 한다.
						   // 현재와 같이 프로그램을짜면 comport master에서 빠르게 전송시
						   // 덮어 쓸 활률이 있다. 
	}
	else
	{
		rx_buff[i++] = data; 		
	}
} 

// UART0 1 byte를 전송 하는 함수
void UART0_transmit(uint8_t data)
{
	
	while ( !(UCSR0A & ( 1 << UDRE0)))  // data가 전송중이면 전송이 완료 될때까지 기다린다.
		;   // no operation (아무 action도 취하지 않는다)
	UDR0 = data; 
} 
void pc_command_processing()
{
	if (rx_ready_flag)   // if (rx_ready_flag >= 1)
	{
		rx_ready_flag=0;

		if (strncmp(rx_buff+1, "ledallon",strlen("ledallon")-1) == 0 )
		{
			PORTA = 0xff;
		}
		if (strncmp(rx_buff+1, "ledalloff", strlen("ledalloff")-1) == 0 )
		{
			PORTA = 0x00;
		}
	}
	
}