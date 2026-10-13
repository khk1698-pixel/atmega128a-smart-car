/*
 * ultrasonic.c
 *
 * Created: 2023-03-22 오전 10:18:27
 *  Author: kccistc
 */ 
#include "ultrasonic.h"

/*
#define TRIG 4
#define TRIG_PORT  PORTG
#define TRIG_DDR   DDRG

#define ECHO 4
#define ECHO_PORT  PINE
#define ECHO_DDR   DDRE
*/
extern volatile int ultrasonic_trigger_timer; // 1초마다 trigger펄스 발생

volatile int ultrasonic_distance=0;
volatile char cm[40];
// PE4: 초음파 ECHO 펄스  (상승.하강시 이곳으로 진입)
ISR(INT4_vect)
{
	if (ECHO_PORT & (1 << ECHO_PIN))   // 상승 에지
	{
		TCNT1 = 0;   // timer 1번의 HW register 64us마다 증가
	}
	else  // 하강 에지
	{
		// 에코핀의 펄스 길이를 us로 환산
		// (1) 1024 / F_CPU : timer1로 공급되는 동작 주파수
		// (2) TCNT1 * (1)
		// (3) 1000000 * (2) : 눈으로 확인 하기 위한 us값
		// 예) TCNT1 : 10 이 들어 있다고 가정
		// (1) 1024 / F_CPU  : 0.000064sec
		// (2) TCNT1 * 10 ==> 0.00064sec(640us)
		// (3) (2) * 1000000 : 640us
		// === 1cm : 왕복 58us소요  340m/s
		//  640us / 58 : 11cm
		 ultrasonic_distance = 1000000.0 * TCNT1 * 1024 / F_CPU;
		 sprintf(cm,"distance: %dcm", ultrasonic_distance/58);
	}
}


// PG4: trigger
// PE3 : echo (INT4) 외부인터럽트4번
void init_ultrasonic(void)
{
	TRIG_DDR |= 1 << TRIG;   // output mode로 설정
	ECHO_DDR &= ~(1 << ECHO_PIN);  // input mode
	// 0 1 : 어떠한 형태로 든지 신호 변화가 발생이 되면 INT요청(상승.하강 둘다 INT)
	// P288 P289 참고
	EICRB |= (0 << ISC41 | 1 << ISC40);
	// timer1 번 사용 16bit timer 
	// 16 timer 1번을 1024분주 해서 공급
	// 16MHZ 를 1024분주 : 16000000/1024 ==> 15625HZ (15.625KHZ)
	// 1펄스의 주기 : T = 1/f 1/15625 = 0.000064sec (64us)
	// P318 참고
	TCCR1B |= (1 << CS12 | 1 << CS10);   // 1024분주
	EIMSK |= 1 << INT4;   // 외부 INT4 (ECHO pin) 사용
}

void ultrasonic_distance_check()
{
	if (ultrasonic_trigger_timer >= 1000)  // 1초
	{
		ultrasonic_trigger_timer=0;
		printf("%s\n", cm);
		I2C_LCD_write_string_XY(1, 0, cm);
		ultrasonic_trigger();
	}
}

void ultrasonic_trigger()
{
	// 1. LOW
	TRIG_PORT &= ~(1 << TRIG);  // LOW
	// 2. 1us 유지
	_delay_us(1);
	// 3. HIGH
	TRIG_PORT |= (1 << TRIG);  // HIGH
	// 4. 3번 유지 (10us이상)
	_delay_us(15);
	// 5. LOW
	TRIG_PORT &= ~(1 << TRIG);  // LOW
}