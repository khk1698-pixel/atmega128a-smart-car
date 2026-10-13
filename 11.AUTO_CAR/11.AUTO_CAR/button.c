/*
 * button.c
 *
 * Created: 2023-03-15 오후 3:48:11
 *  Author: kccistc
 */ 
#include "button.h"   // 현재 directory에 있는 화일을 include 
#include "led.h"
int get_button(int button);
void init_button();

int button1_state=0;

void auto_mode_check(void)
{
	if(get_button(BUTTON1))
	{
		button1_state = !button1_state;   // manual <--> auto
	}
	if (button1_state)  // auto
	{
		AUTO_RUN_LED_PORT |= 1 << 3; // LED ON
	}
	else
	{
		AUTO_RUN_LED_PORT &= ~(1 << 3); // LED OFF
	}
}

void init_button()
{
	BUTTON_DDR &=  ~(1 << BUTTON1);
	// (1) (1 << BUTTON1 | 1 << BUTTON2 | 1 << BUTTON3) ==> 01110000
	// (2) ~(1 << BUTTON1 | 1 << BUTTON2 | 1 << BUTTON3) ==> 10001111
}
// 0: 아직 버튼을 누르지 않은 상태
// 1: 완전히 버튼을 누른 상태 
int get_button(int button)
{
	static unsigned char prev_state[8]={0,0,0,0,0,0,0,0};  
										// 함수를 빠져 나왔다 다시 진입 해도
	                                    // 전역 변수 처럼 변화된 값 그대로 유지
										
	unsigned char currnt_state;  // 현재의 버튼 상태
	
	currnt_state = BUTTON_PIN & (1 << button);  // 00010000
	
	// 처음 누른 상태  
	if (prev_state[button] == 0 && currnt_state != 0)
	{
		_delay_ms(60);   // noise가 지나 가기를 기다림
		prev_state[button]=1;    // 처음 누른 상태가 아님을 가리키는 의미에서 
		                 // prev_state를 1로 set
		return 0;        // 아직 진짜로 눌러진 상태가 아니다. 
		                 // 지금은 노이즈 상태 이다. 
	}      // 버튼을 눌렀다 뗀 상태 이다. 
	else if (prev_state[button] == 1 && currnt_state == 0)
	{
		prev_state[button] = 0;
		return 1;   // 완전히 스위치를 눌렀다 뗀것으로 인정 
	}
	
	return 0; // 아직 완전히 S/W를 울렀다 떼지 않은 상태 이다. 
}







