// Timer.c
// Runs on TM4C123
// Provide pulse measurement functions for Timer0, Timer1, Timer2 and Timer3,

// Daniel Valvano, Jonathan Valvano
// January 7, 2017 

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// HC-SR04 Ultrasonic Range Finder 
// J9X  Trigger0 to PB7 output (10us pulse)
// J9X  Echo0    to PB6 T0CCP0
// J10X Trigger1 to PB5 output (10us pulse)
// J10X Echo1    to PB4 T1CCP0
// J11X Trigger2 to PB3 output (10us pulse)
// J11X Echo2    to PB2 T3CCP0
// J12X Trigger3 to PC5 output (10us pulse)
// J12XEcho3    to PF4 T2CCP0

// Ping))) Ultrasonic Range Finder 
// J9Y  Trigger/Echo0 to PB6 T0CCP0
// J10Y Trigger/Echo1 to PB4 T1CCP0
// J11Y Trigger/Echo2 to PB2 T3CCP0
// J12Y Trigger/Echo3 to PF4 T2CCP0

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
// TIMER0 is Trigger PB7, echo PB6
#define TRIGGER0    (*((volatile uint32_t *)0x40005200))
#define TRIGGER0HIGH 0x80
#define ECHO0       (*((volatile uint32_t *)0x40005100))
#define ECHO0HIGH 0x40
// TIMER1 is Trigger PB5, echo PB4
#define TRIGGER1    (*((volatile uint32_t *)0x40005080))
#define TRIGGER1HIGH 0x20
#define ECHO1       (*((volatile uint32_t *)0x40005040))
#define ECHO1HIGH 0x10
// TIMER3 is Trigger PB3, echo PB2
#define TRIGGER3    (*((volatile uint32_t *)0x40005020))
#define TRIGGER3HIGH 0x08
#define ECHO3       (*((volatile uint32_t *)0x40005010))
#define ECHO3HIGH 0x04


// Subroutine to wait 10 usec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10us(void){uint32_t volatile time;
  time = 800/12;  // 10usec
  while(time){
    time--; // 12 cycles
  }
}
// Subroutine to wait 5 usec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait5us(void){uint32_t volatile time;
  time = 400/12;  // 5usec
  while(time){
    time--; // 12 cycles
  }
}
//---------------------Timer0----------------------------
uint32_t Timer0First,Timer0Done,Timer0Pulse;
//------------Timer0_Init------------
// Initialize Timer0A in edge time mode to request interrupts on
// the both edges of PB6 (T0CCP0).  The interrupt service routine
// acknowledges the interrupt records the time.
// PB7 GPIO output
// Input: none
// Output: none
void Timer0_Init(void){
  Timer0First = Timer0Done = Timer0Pulse = 0;
  SYSCTL_RCGCTIMER_R |= 0x01;// activate timer0
  SYSCTL_RCGCGPIO_R |= 0x02; // activate port B
  while((SYSCTL_PRGPIO_R&0x0002) == 0){};// ready?
  GPIO_PORTB_DIR_R |= 0x80;        // make PB7 output
  GPIO_PORTB_DIR_R &= ~0x40;       // make PB6 in
  GPIO_PORTB_AFSEL_R |= 0x40;      // enable alt funct on PB6
  GPIO_PORTB_AFSEL_R &= ~0x80;     // disable alt funct on PB7
  GPIO_PORTB_DEN_R |= 0xC0;        // enable PB6 as T0CCP0
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0x00FFFFFF)+0x07000000;
  GPIO_PORTB_AMSEL_R &= ~0xC0;     // disable analog functionality on PB6
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = TIMER_CFG_16_BIT; // configure for 16-bit timer mode
  TIMER0_TAMR_R = (TIMER_TAMR_TACMR|TIMER_TAMR_TAMR_CAP);   // 24-bit capture         
  TIMER0_CTL_R |= TIMER_CTL_TAEVENT_BOTH;// configure for both edges
  TIMER0_TAILR_R = TIMER_TAILR_M;  // max start value
  TIMER0_TAPR_R = 0xFF;            // activate prescale, creating 24-bit
  TIMER0_IMR_R |= TIMER_IMR_CAEIM; // enable capture match interrupt
  TIMER0_ICR_R = TIMER_ICR_CAECINT;// clear timer0A capture match flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 16-b, +edge timing, interrupts
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;        // enable interrupt 19 in NVIC
}
//------------Timer0_StartHCSR04------------
// start HCSR04 ultrasonic distance measurement
// 10us Pulse output on PB7 GPIO output
// Input: none
// Output: none
void Timer0_StartHCSR04(void){
  Timer0Done = 0;
  TRIGGER0 = TRIGGER0HIGH;
  DelayWait10us();
  DelayWait10us();
  TRIGGER0 = 0;
  TIMER0_ICR_R = TIMER_ICR_CAECINT;// clear timer0A capture match flag
}
//------------Timer0_StartPing------------
// start Ping))) ultrasonic distance measurement
// 1) Make PB6 GPIO output
// 2) 5us Pulse output on PB6 GPIO output
// 3) Make PB6 input capture Timer input
// Input: none
// Output: none
void Timer0_StartPing(void){
  GPIO_PORTB_AFSEL_R &= ~0x40;     // disable alt funct on PB6
  GPIO_PORTB_PCTL_R = GPIO_PORTB_PCTL_R&0xF0FFFFFF;
  GPIO_PORTB_DIR_R |= 0x40;        // make PB6 output
  Timer0Done = 0;
  ECHO0 = ECHO0HIGH;
  DelayWait5us();
  ECHO0 = 0;
  GPIO_PORTB_DIR_R &= ~0x40;       // make PB6 in
  GPIO_PORTB_AFSEL_R |= 0x40;      // enable alt funct on PB6
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xF0FFFFFF)+0x07000000;
  TIMER0_ICR_R = TIMER_ICR_CAECINT;// clear timer0A capture match flag
}
//------------Timer0_Read------------
// read ultrasonic distance measurement
// Input: none
// Output: 0 if not ready, pulse width in 12.5ns time if ready
uint32_t Timer0_Read(void){
  if(Timer0Done){
    return Timer0Pulse;
  }
  return 0;
}
void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_CAECINT;// acknowledge timer0A capture match
  if(ECHO0 == ECHO0HIGH){ // first
    Timer0First = TIMER0_TAR_R;
  }else{
    Timer0Pulse = (Timer0First - TIMER0_TAR_R)&0xFFFFFF;// 24 bits, 12.5ns resolution
    Timer0Done = 1;
  }
}
//-------------------------Timer1-------------------------------
uint32_t Timer1First,Timer1Done,Timer1Pulse;
//------------Timer1_Init------------
// Initialize Timer1A in edge time mode to request interrupts on
// the both edges of PB4 (T1CCP0).  The interrupt service routine
// acknowledges the interrupt records the time.
// PB5 GPIO output
// Input: none
// Output: none
void Timer1_Init(void){
  Timer1First = Timer1Done = Timer1Pulse = 0;
  SYSCTL_RCGCTIMER_R |= 0x02;// activate timer1
  SYSCTL_RCGCGPIO_R |= 0x02; // activate port B
  while((SYSCTL_PRGPIO_R&0x0002) == 0){};// ready?
  GPIO_PORTB_DIR_R |= 0x20;        // make PB5 output
  GPIO_PORTB_DIR_R &= ~0x10;       // make PB4 in
  GPIO_PORTB_AFSEL_R |= 0x10;      // enable alt funct on PB4
  GPIO_PORTB_AFSEL_R &= ~0x20;     // disable alt funct on PB5
  GPIO_PORTB_DEN_R |= 0x30;        // enable PB4 as T1CCP0
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFF00FFFF)+0x00070000;
  GPIO_PORTB_AMSEL_R &= ~0x30;     // disable analog functionality on PB5,4
  TIMER1_CTL_R &= ~TIMER_CTL_TAEN; // disable timer1A during setup
  TIMER1_CFG_R = TIMER_CFG_16_BIT; // configure for 16-bit timer mode
  TIMER1_TAMR_R = (TIMER_TAMR_TACMR|TIMER_TAMR_TAMR_CAP);   // 24-bit capture         
  TIMER1_CTL_R |= TIMER_CTL_TAEVENT_BOTH;// configure for both edges
  TIMER1_TAPR_R = 0xFF;            // activate prescale, creating 24-bit
  TIMER1_TAILR_R = TIMER_TAILR_M;  // max start value
  TIMER1_IMR_R |= TIMER_IMR_CAEIM; // enable capture match interrupt
  TIMER1_ICR_R = TIMER_ICR_CAECINT;// clear timer1A capture match flag
  TIMER1_CTL_R |= TIMER_CTL_TAEN;  // enable timer1A 16-b, +edge timing, interrupts
                                   // Timer0A=priority 2
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00004000; // bits 15-13
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
}
//------------Timer1_StartHCSR04------------
// start HCSR04 ultrasonic distance measurement
// 10us Pulse output on PB3 GPIO output
// Input: none
// Output: none
void Timer1_StartHCSR04(void){
  Timer1Done = 0;
  TRIGGER1 = TRIGGER1HIGH;
  DelayWait10us();
  DelayWait10us();
  TRIGGER1 = 0;
  TIMER1_ICR_R = TIMER_ICR_CAECINT;// clear timer0A capture match flag
}
//------------Timer1_StartPing------------
// start Ping))) ultrasonic distance measurement
// 1) Make PB4 GPIO output
// 2) 5us Pulse output on PB4 GPIO output
// 3) Make PB4 input capture Timer input
// Input: none
// Output: none
void Timer1_StartPing(void){
  GPIO_PORTB_AFSEL_R &= ~0x10;     // disable alt funct on PB4
  GPIO_PORTB_PCTL_R = GPIO_PORTB_PCTL_R&0xFFF0FFFF;
  GPIO_PORTB_DIR_R |= 0x10;        // make PB4 output
  Timer1Done = 0;
  ECHO1 = ECHO1HIGH;
  DelayWait5us();
  ECHO1 = 0;
  GPIO_PORTB_DIR_R &= ~0x10;       // make PB4 in
  GPIO_PORTB_AFSEL_R |= 0x10;      // enable alt funct on PB4
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFF0FFFF)+0x00070000;
  TIMER1_ICR_R = TIMER_ICR_CAECINT;// clear timer1A capture match flag
}
//------------Timer1_Read------------
// read ultrasonic distance measurement
// Input: none
// Output: 0 if not ready, pulse width in 12.5ns time if ready
uint32_t Timer1_Read(void){
  if(Timer1Done){
    return Timer1Pulse;
  }
  return 0;
}
void Timer1A_Handler(void){
  TIMER1_ICR_R = TIMER_ICR_CAECINT;// acknowledge timer1A capture match
  if(ECHO1 == ECHO1HIGH){ // first
    Timer1First = TIMER1_TAR_R;
  }else{
    Timer1Pulse = (Timer1First - TIMER1_TAR_R)&0xFFFFFF;// 24 bits, 12.5ns resolution
    Timer1Done = 1;
  }
}



//-------------------------Timer3-------------------------------
uint32_t Timer3First,Timer3Done,Timer3Pulse;
//------------Timer3_Init------------
// Initialize Timer3A in edge time mode to request interrupts on
// the both edges of PB2 (T3CCP0).  The interrupt service routine
// acknowledges the interrupt records the time.
// PB3 GPIO output
// Input: none
// Output: none
void Timer3_Init(void){
  Timer3First = Timer3Done = Timer3Pulse = 0;
  SYSCTL_RCGCTIMER_R |= 0x08;// activate timer3
  SYSCTL_RCGCGPIO_R |= 0x02; // activate port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};// ready?
  GPIO_PORTB_DIR_R |= 0x08;        // make PB3 output
  GPIO_PORTB_DIR_R &= ~0x04;       // make PB2 in
  GPIO_PORTB_AFSEL_R |= 0x04;      // enable alt funct on PB2
  GPIO_PORTB_AFSEL_R &= ~0x08;     // disable alt funct on PB3
  GPIO_PORTB_DEN_R |= 0x0C;        // enable PB2 as T3CCP0
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)+0x00000700;
  GPIO_PORTB_AMSEL_R &= ~0x0C;     // disable analog functionality on PB3,2
  TIMER3_CTL_R &= ~TIMER_CTL_TAEN; // disable timer3A during setup
  TIMER3_CFG_R = TIMER_CFG_16_BIT; // configure for 16-bit timer mode
  TIMER3_TAMR_R = (TIMER_TAMR_TACMR|TIMER_TAMR_TAMR_CAP);   // 24-bit capture         
  TIMER3_CTL_R |= TIMER_CTL_TAEVENT_BOTH;// configure for both edges
  TIMER3_TAILR_R = TIMER_TAILR_M;  // max start value
  TIMER3_TAPR_R = 0xFF;            // activate prescale, creating 24-bit
  TIMER3_IMR_R |= TIMER_IMR_CAEIM; // enable capture match interrupt
  TIMER3_ICR_R = TIMER_ICR_CAECINT;// clear timer3A capture match flag
  TIMER3_CTL_R |= TIMER_CTL_TAEN;  // enable timer3A 16-b, +edge timing, interrupts
  NVIC_PRI8_R = (NVIC_PRI8_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 51, interrupt number 35
  NVIC_EN1_R = 1<<(35-32);      // 9) enable IRQ 35 in NVIC
}
//------------Timer3_StartHCSR04------------
// start HCSR04 ultrasonic distance measurement
// 10us Pulse output on PB3 GPIO output
// Input: none
// Output: none
void Timer3_StartHCSR04(void){
  Timer3Done = 0;
  TRIGGER3 = TRIGGER3HIGH;
  DelayWait10us();
  DelayWait10us();
  TRIGGER3 = 0;
  TIMER3_ICR_R = TIMER_ICR_CAECINT;// clear timer3A capture match flag
}
//------------Timer3_StartPing------------
// start Ping))) ultrasonic distance measurement
// 1) Make PB2 GPIO output
// 2) 5us Pulse output on PB2 GPIO output
// 3) Make PB2 input capture Timer input
// Input: none
// Output: none
void Timer3_StartPing(void){
  GPIO_PORTB_AFSEL_R &= ~0x04;     // disable alt funct on PB2
  GPIO_PORTB_PCTL_R = GPIO_PORTB_PCTL_R&0xFFFFF0FF;
  GPIO_PORTB_DIR_R |= 0x04;        // make PB2 output
  Timer3Done = 0;
  ECHO3 = ECHO3HIGH;
  DelayWait5us();
  ECHO3 = 0;
  GPIO_PORTB_DIR_R &= ~0x04;       // make PB2 in
  GPIO_PORTB_AFSEL_R |= 0x04;      // enable alt funct on PB2
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFFF0FF)+0x00000700;
  TIMER3_ICR_R = TIMER_ICR_CAECINT;// clear timer3A capture match flag
}
//------------Timer3_Read------------
// read ultrasonic distance measurement
// Input: none
// Output: 0 if not ready, pulse width in 12.5ns time if ready
uint32_t Timer3_Read(void){
  if(Timer3Done){
    return Timer3Pulse;
  }
  return 0;
}
void Timer3A_Handler(void){
  TIMER3_ICR_R = TIMER_ICR_CAECINT;// acknowledge timer3A capture match
  if(ECHO3 == ECHO3HIGH){ // first
    Timer3First = TIMER3_TAR_R;
  }else{
    Timer3Pulse = (Timer3First - TIMER3_TAR_R)&0xFFFFFF;// 24 bits, 12.5ns resolution
    Timer3Done = 1;
  }
}

#define BUS 80000
//------------Cycles2milliInch------------
// convert time in bus cycles to distance in 0.001 in
// speed of sound is 13,560 in/sec
// Input: cycles 
// Output: distance in 0.001in
uint32_t Cycles2milliInch(uint32_t cycles){
// (x cycles)*(1 sec/80,000,000 cycles)*(13,560 in/sec)*(1,000 mil/1 in)*(0.5 round trip) =
// (x cycles)*6780)/BUS 
  return (cycles*6780)/BUS; 
}
//------------Cycles2millimeter------------
// convert time in bus cycles to distance in mm
// speed of sound is 340.29 m/sec
// Input: cycles 
// Output: distance in mm
uint32_t Cycles2millimeter(uint32_t cycles){
// (x cycles)*(1 sec/80,000,000 cycles)*(340.29 m/sec)*(1,000 mm/1 m)*(0.5 round trip) =
  return (cycles*170)/BUS;  
}
