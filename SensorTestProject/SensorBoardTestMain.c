// SensorBoardTestMain.c
// Runs on LM4F120/TM4C123
// Test the functions in ST7735.c by printing basic
// patterns to the LCD.
//    16-bit color, 128 wide by 160 high LCD
// Daniel Valvano, Jonathan Valvano
// January 11, 2017 

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

// hardware connections
// **********ST7735 TFT and SDC*******************
// ST7735
// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) connected to PB0
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

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

#include <stdio.h>
#include <stdint.h>
#include "ST7735.h"
#include "PLL.h"
#include "board.h"
#include "Timer.h"
#include "ADCSWTrigger.h"
#include "../inc/tm4c123gh6pm.h"
void Timer0_Init(void);
void Timer0_Start(void);
void EnableInterrupts(void);

void DelayWait10ms(uint32_t n);

uint32_t PingType,IRdata[4],Cycle0,Cycle1,Cycle2,Cycle3;
int main(void){ int i;   // sensor board test main
  PLL_Init(Bus80MHz);    // set system clock to 80 MHz
  Board_Init();          // switches LEDS on LaunchPad
  Timer0_Init();
  Timer1_Init();
  Timer3_Init();
  ST7735_InitR(INITR_REDTAB);
  ST7735_DrawString(0,0,"Sensor Board Test",ST7735_YELLOW);
  ST7735_DrawString(0,1,"IR on pin PE3-0",ST7735_YELLOW);
  ST7735_DrawString(0,2,"Which ultrasonic?",ST7735_WHITE);
  ST7735_DrawString(0,3,"SW1 4-pin HCSR04",ST7735_WHITE);
  ST7735_DrawString(0,4,"SW2 3-pin Ping)))",ST7735_WHITE);
  while((Board_Input()&SW2)&&(Board_Input()&SW1)){ // advance when SW1 or SW2 pressed
    DelayWait10ms(1);
  }
  if((Board_Input()&SW1)==0){
    PingType = 0;
    ST7735_DrawString(0,2,"Use 4-pin HCSR04   ",ST7735_WHITE);
  }else{
    PingType = 1;
    ST7735_DrawString(0,2,"Use 3-pin Ping)))  ",ST7735_WHITE);
  }
  while(Board_Input()!=(SW1|SW2)){ // wait for release
    DelayWait10ms(1);
  }

  ADC_Init3210();

  ST7735_DrawString(0, 3,"J5/A3/PE3=       mm",ST7735_WHITE);
  ST7735_DrawString(0, 4,"J6/A2/PE2=       mm",ST7735_WHITE);
  ST7735_DrawString(0, 5,"J7/A1/PE1=       mm",ST7735_WHITE);
  ST7735_DrawString(0, 6,"J8/A0/PE0=       mm",ST7735_WHITE);
  ST7735_DrawString(0,11,"J9 /Ping0=       mm",ST7735_WHITE);
  ST7735_DrawString(0,12,"J10/Ping1=       mm",ST7735_WHITE);
  ST7735_DrawString(0,13,"J11/Ping2=       mm",ST7735_WHITE);
  EnableInterrupts();
  while(1){
    if(PingType){
      Timer0_StartPing();
      Timer1_StartPing();
      Timer3_StartPing();
    }else{
      Timer0_StartHCSR04();
      Timer1_StartHCSR04();
      Timer3_StartHCSR04();
    }
    DelayWait10ms(50);
    ADC_In3210(IRdata);
    for(i=0;i<4;i++){
      ST7735_SetCursor(11,i+3); 
      ST7735_OutUDec4(ADC2millimeter(IRdata[i]));
      ST7735_OutString("  ");
    }
    ST7735_SetCursor(11,11); 
    Cycle0 = Timer0_Read();
    if(Cycle0){
      ST7735_OutUDec5(Cycles2millimeter(Cycle0)); 
    }
    ST7735_SetCursor(11,12); 
    Cycle1 = Timer1_Read();
    if(Cycle1){
      ST7735_OutUDec5(Cycles2millimeter(Cycle1)); 
    }
    ST7735_SetCursor(11,13); 
    Cycle3 = Timer3_Read();
    if(Cycle3){
      ST7735_OutUDec5(Cycles2millimeter(Cycle3)); 
    }
  }
}


// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
      time--;
    }
    n--;
  }
}


