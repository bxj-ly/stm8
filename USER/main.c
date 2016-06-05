
 /**
 * This file is part of main.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include "stm8s_clk.h"
#include "uart.h"
#include "spi.h"
#include "nmealib.h"
#include "main.h"


u8 Rx_Buffer[BufferSize];
u8 data[BufferSize];


void Delay(u32 nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}

/**
 * Recieve a line of infomation from GPS.
 * Put it into buf array.
 *
 * @param buffer to fill from gps
 * @return length of recieved bytes
 */
u8 uart_Getline(u8 buf[]){
    u8 ch;
    u8 pt = 0;
    while (pt < BufferSize){
        while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);
        ch=UART1_ReceiveData8();
        if(ch == '\r' || ch == '\n'){
            if(pt != 0)
                break;
        }
        else{
            buf[pt]=ch;
            pt ++;
        }
    }
    return pt;
}


void main(void)
{
  u8 len = 0;
  /* Infinite loop */

  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  Uart_Init();
  SPI_Slave_Init();


//   __enable_interrupt(); 


   while (1)
   {
        len = uart_Getline(Rx_Buffer);
        nmea_parse_GGA(Rx_Buffer, len, false, data);
        Delay(0x00ff);

   }
  
}







