/*******************************************************************************
 *                                                                             *
 *                                    i2s                                      *
 *                                                                             *
 *                                   i2s.c                                     *
 *                                                                             *
 *                                   Module                                    *
 *                                                                             *
 *                    Copyright (C) 2014 Ulrik Hørlyk Hjort                    *
 *                                                                             *
 *   i2s is free software;  you can  redistribute it                           *
 *   and/or modify it under terms of the  GNU General Public License           *
 *   as published  by the Free Software  Foundation;  either version 2,        *
 *   or (at your option) any later version.                                    *
 *   i2s is distributed in the hope that it will be                            *
 *   useful, but WITHOUT ANY WARRANTY;  without even the  implied warranty     *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                   *
 *   See the GNU General Public License for  more details.                     *
 *   You should have  received  a copy of the GNU General                      *
 *   Public License  distributed with Yolk.  If not, write  to  the  Free      *
 *   Software Foundation,  51  Franklin  Street,  Fifth  Floor, Boston,        *
 *   MA 02110 - 1301, USA.                                                     *
 *                                                                             *
 ******************************************************************************/



#include <avr/io.h>

#define F_CPU 16000000
#include <util/delay.h>
#include <avr/interrupt.h>

#define NOP() {__asm__ __volatile__ ("nop");}

#define I2S_SCK  PB7
#define I2S_WS   PB6
#define I2S_SD   PB5

#define DATA_LEN 10


uint8_t I2S_SCK_LOW = ~(1<<I2S_SCK);
uint8_t I2S_SCK_HIGH = (1<<I2S_SCK); 

uint8_t I2S_WS_LOW = ~(1<<I2S_WS);
uint8_t I2S_WS_HIGH = (1<<I2S_WS); 


uint8_t I2S_SD_LOW = ~(1<<I2S_SD);
uint8_t I2S_SD_HIGH = (1<<I2S_SD); 


const uint32_t delay_time = 31; 

const uint32_t bits_per_sample = 32;

uint8_t WS = 0;
uint8_t ws_val = 0;

uint32_t data =  0xDEADBEEF;
uint8_t data_bit;


uint8_t buf_index = 0;

uint32_t data_buf[DATA_LEN] = {0x00000000, // Left channel
			       0x00000001, // Right channel
			       0x00000002, // Left channel
			       0x00000003, // .
			       0x00000004, // .
			       0x00000005, // .
			       0x00000006, // .
			       0x00000007, // .
			       0x00000008, // .
			       0x00000009};// Right channel




void main(void) {

  DDRB |=1<<I2S_SCK;  // CLK
  DDRB |=1<<I2S_WS;  // WS
  DDRB |=1<<I2S_SD;  // Data



  while(1) {

    // Word select out:
    if (!WS) {
      if (ws_val == 0) {
	PORTB &= I2S_WS_LOW;
	ws_val = 1;
      } else {
      PORTB |= I2S_WS_HIGH;
	ws_val = 0;
      }
      data = data_buf[buf_index];
      buf_index++;
      buf_index %= DATA_LEN;
    }

    // Clock out:
    PORTB &= I2S_SCK_LOW;
    _delay_us(delay_time);

    PORTB |= I2S_SCK_HIGH; 
    _delay_us(delay_time);


   // data out:
    data_bit = (data >> (31-WS)) & 0x1;
      if (data_bit == 0) {
	PORTB &= I2S_SD_LOW;
      } else {
	PORTB |= I2S_SD_HIGH; 
      }

    WS++;
    WS %= bits_per_sample;

  }
    
}


