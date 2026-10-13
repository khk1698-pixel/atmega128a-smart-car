/*
 * 06ULTRASONIC.c
 *
 * Created: 2023-03-17 오전 10:54:45
 * Author : kccistc
 */ 
#define  F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>   // for interrupt 
#include <stdio.h>   // printf scanf ....
#include <string.h>   // strcpy strncmp ....

extern int button1_state;
extern void UART0_transmit(uint8_t data);
// for printf
// stdin: input (키보드)
// stdout : 포준출력 장치(화면)
FILE OUTPUT = FDEV_SETUP_STREAM(
UART0_transmit, NULL, _FDEV_SETUP_WRITE);

#define  MOTOR_DRIVER_PORT      PORTF
#define  MOTOR_DRIVER_PORT_DDR  DDRF
#define  MOTOR_PWM_DDR  DDRB
#define  MOTOR_RIGHT_PORT_DDR 5  // OC1A
#define  MOTOR_LEFT_PORT_DDR  6  // OC1B

int main(void)
{
	init_led();
	init_button();
	init_uart0();
	init_uart1();
	init_time0();
	// init pwm motor driver
	init_pwm_motor_driver();
	init_ultrasonic();   // 초음파센서 관련된것은 수정 (3개로)
	
	stdout = &OUTPUT;   // fprintf(stdout, "test"); printf("test"); 
	sei();   // global interrupt enable

	while(1)
	{
		manual_mode_run();
		auto_mode_check();
		if (button1_state)
		{
			auto_drive();
		} 
	}
}

// 자율주행 프로그램 이곳에 programming 한다. 
void auto_drive(void)
{
	;
}
extern volatile unsigned char uart1_data;
void manual_mode_run(void)
{
	switch(uart1_data)
	{
	case 'F':  // 전진
		forward(500);  // 4us x 500 = 0.002sec(2ms)
		break;
	case 'B':   // 후진
		backward(500);
		break;
	case 'L':    // 좌회전
		turn_left(700);
		break;
	case 'R':    // 우회전 
		turn_right(700);
		break;
	case 'S':    // stop
		stop();
	break;
	default:
		break;
	}
}
void forward(int speed)
{
	MOTOR_DRIVER_PORT &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 );
	MOTOR_DRIVER_PORT |= ( 1 << 2 | 1 << 0); // 정회전 set

	OCR1A = speed;  // PB5 PWM left motor
	OCR1B = speed;  // PB6     right
}
void backward(int speed)
{
	MOTOR_DRIVER_PORT &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 );
	MOTOR_DRIVER_PORT |= ( 1 << 3 | 1 << 1); // 역회전 set

	OCR1A = speed;  // PB5 PWM left motor
	OCR1B = speed;  // PB6     right
}
void turn_left(int speed)
{
	MOTOR_DRIVER_PORT &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 );
	MOTOR_DRIVER_PORT |= ( 1 << 2 | 1 << 0); // 정회전 set

	OCR1A = 0;  // PB5 PWM left motor
	OCR1B = speed;  // PB6     right
}
void turn_right(int speed)
{
	MOTOR_DRIVER_PORT &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 );
	MOTOR_DRIVER_PORT |= ( 1 << 2 | 1 << 0); // 정회전 set

	OCR1A = speed;  // PB5 PWM left motor
	OCR1B = 0;  // PB6     right
}
void stop(int speed)
{
	OCR1A = 0;  // PB5 PWM left motor
	OCR1B = 0;  // PB6     right

	MOTOR_DRIVER_PORT &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 );
	MOTOR_DRIVER_PORT |= (1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 ); // stop
}


/*
  1. LEFT MOTOR
     PORTF.0 --> IN1 
     PORTF.1 --> IN2
  2. RIGHT MOTOR
     PORTF.2 --> IN3
     PORTF.3 --> IN4
     IN1      IN2
     IN3      IN4
     ===      ===
       0       1  : 역회전
       1       0  : 정회전
       1       1  : STOP
*/

void init_pwm_motor_driver()
{
	MOTOR_DRIVER_PORT_DDR |= (1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 );
	MOTOR_PWM_DDR |= (1 << MOTOR_RIGHT_PORT_DDR | 1 << MOTOR_LEFT_PORT_DDR );
/*
1. 분주: 64분주 16000000/64 : 250,000HZ(250KHZ)
2. 주기 T = 1/f 1/250000 --> 0.000004sec(4us)
3. 0.000004 x 256 --> 0.001024sec--->1.024ms
   0.000004 x 250 --> 0.001sec 1ms
4.  127 : 0.5ms
5. 0x3ff(1023) ==> 4ms (top을 조절 하기 위해 모드 14사용)
*/
	TCCR1B |= (1 << WGM13 | 1 << WGM12);  // 모드14 고속PWM ICR1
	TCCR1A |= (1 << WGM11 | 0 << WGM10);  //  P349 참고
	TCCR1A |= (1 << COM1A1 | 0 << COM1A0); // 비반전모드 : OCR: LOW TOP: HIGH
    TCCR1A |= (1 << COM1B1 | 0 << COM1B0); //   ,, 

	MOTOR_DRIVER_PORT &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 );
	MOTOR_DRIVER_PORT |= ( 1 << 2 | 1 << 0); // 정회전 set
	ICR1 = 0x3ff;  // 1023 --> 4ms
}
/*
1. 분주: 64분주 16000000/64 : 250,000HZ(250KHZ)
2. 주기 T = 1/f 1/250000 --> 0.000004sec(4us)
3. 8 bit Timer OV : 0.000004 x 256 --> 0.001024sec--->1.024ms
0.000004 x 250 --> 0.001sec 1ms
*/
// TCNT0 가 256이 되면 자동으로 이곳 진입 (1ms마다) 
int ms_count=0;   //ms를 재는 count
int sec_count=0;  // sec를 재는 count  
ISR(TIMER0_OVF_vect)
{
	TCNT0=6;
	
	if (++ms_count >= 1000)  // 1000ms --> 1sec
	{
		ms_count=0;
		sec_count++;
	}	
}

void init_time0()
{
	TCNT0=6;   // TCNT 6~256  : 정확히 1ms마다 timer 0 가 뜬다.
	TCCR0 |= (1 << CS02 | 0 << CS01 | 0 << CS00);   // 64분주 P296참조
	TIMSK |= (1 << TOIE0);   // timer 0 overflow interrupt enable 
}
