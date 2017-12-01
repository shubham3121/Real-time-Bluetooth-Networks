// Timer.h
// Runs on TM4C123
// Provide pulse measurement functions for Timer0, Timer1, Timer2 and Timer3,

// Daniel Valvano, Jonathan Valvano
// January 11, 2017 

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


// Subroutine to wait 10 usec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10us(void);

//------------Timer0_Init------------
// Initialize Timer0A in edge time mode to request interrupts on
// the both edges of PB6 (CCP0).  The interrupt service routine
// acknowledges the interrupt records the time.
// PB7 GPIO output
// Input: none
// Output: none
void Timer0_Init(void);

//------------Timer0_StartHCSR04------------
// start HCSR04 ultrasonic distance measurement
// 10us Pulse output on PB7 GPIO output
// Input: none
// Output: none
void Timer0_StartHCSR04(void);

//------------Timer0_StartPing------------
// start Ping))) ultrasonic distance measurement
// 1) Make PB6 GPIO output
// 2) 5us Pulse output on PB6 GPIO output
// 3) Make PB6 input capture Timer input
// Input: none
// Output: none
void Timer0_StartPing(void);

//------------Timer0_Read------------
// read ultrasonic distance measurement
// Input: none
// Output: 0 if not ready, pulse width in 12.5ns time if ready
uint32_t Timer0_Read(void);

//------------Timer1_Init------------
// Initialize Timer1A in edge time mode to request interrupts on
// the both edges of PB4 (T1CCP0).  The interrupt service routine
// acknowledges the interrupt records the time.
// PB5 GPIO output
// Input: none
// Output: none
void Timer1_Init(void);

//------------Timer1_StartHCSR04------------
// start HCSR04 ultrasonic distance measurement
// 10us Pulse output on PB5 GPIO output
// Input: none
// Output: none
void Timer1_StartHCSR04(void);

//------------Timer1_StartPing------------
// start Ping))) ultrasonic distance measurement
// 1) Make PB4 GPIO output
// 2) 5us Pulse output on PB4 GPIO output
// 3) Make PB4 input capture Timer input
// Input: none
// Output: none
void Timer1_StartPing(void);

//------------Timer1_Read------------
// read ultrasonic distance measurement
// Input: none
// Output: 0 if not ready, pulse width in 12.5ns time if ready
uint32_t Timer1_Read(void);

//------------Timer3_Init------------
// Initialize Timer3A in edge time mode to request interrupts on
// the both edges of PB2 (T3CCP0).  The interrupt service routine
// acknowledges the interrupt records the time.
// PB3 GPIO output
// Input: none
// Output: none
void Timer3_Init(void);

//------------Timer3_StartHCSR04------------
// start HCSR04 ultrasonic distance measurement
// 10us Pulse output on PB3 GPIO output
// Input: none
// Output: none
void Timer3_StartHCSR04(void);

//------------Timer3_StartPing------------
// start Ping))) ultrasonic distance measurement
// 1) Make PB2 GPIO output
// 2) 5us Pulse output on PB2 GPIO output
// 3) Make PB2 input capture Timer input
// Input: none
// Output: none
void Timer3_StartPing(void);

//------------Timer3_Read------------
// read ultrasonic distance measurement
// Input: none
// Output: 0 if not ready, pulse width in 12.5ns time if ready
uint32_t Timer3_Read(void);

//------------Cycles2milliInch------------
// convert time in bus cycles to distance in 0.001 in
// speed of sound is 13,560 in/sec
// Input: cycles 
// Output: distance in 0.001in
uint32_t Cycles2milliInch(uint32_t cycles);

//------------Cycles2millimeter------------
// convert time in bus cycles to distance in mm
// speed of sound is 340.29 m/sec
// Input: cycles 
// Output: distance in mm
uint32_t Cycles2millimeter(uint32_t cycles);
