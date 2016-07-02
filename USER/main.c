
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
#include<string.h>


u8 Rx_Buffer[BufferSize];
#if defined(NEED_RESOLVE_INFO)
u8 RxGGA[BufferSize];
#endif //defined(NEED_RESOLVE_INFO)

/**
 *  PQ Buffer solution
 *
 */
u8 DataP[BufferSize];
u8 DataQ[BufferSize];
u8 DataSelect = 0;

void changePutData(){
    DataSelect = (DataSelect == 0)? 1:0;
}
u8 * getPutData()
{
    if(DataSelect == 0)
        return DataP;
    else
        return DataQ;
}
u8 * getDataBuf()
{
    if(DataSelect == 0)
        return DataQ;
    else
        return DataP;
}




/**
* Delay
*/
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

//#define NMEA_PARSE_STUB_TEST 1
 
#if defined(NMEA_PARSE_STUB_TEST)

  const char *stubBuff[] = { 
      "$GPGGA,111609.14,5001.27,N,3613.06,E,3,08,0.0,10.2,M,0.0,M,0.0,0000*70\r\n",
      "$GPGSV,2,1,08,01,05,005,80,02,05,050,80,03,05,095,80,04,05,140,80*7f\r\n",
      "$GPGSV,2,2,08,05,05,185,80,06,05,230,80,07,05,275,80,08,05,320,80*71\r\n",
      "$GPGSA,A,3,01,02,03,04,05,06,07,08,00,00,00,00,0.0,0.0,0.0*3a\r\n",
      "$GPRMC,111609.14,A,5001.27,N,3613.06,E,11.2,0.0,261206,0.0,E*50\r\n",
      "$GPVTG,217.5,T,208.8,M,000.00,N,000.01,K*4C\r\n",
      "$GPRMC,173843,A,3349.896,N,11808.521,W,000.0,360.0,230108,013.4,E*69\r\n"
  };
  const char *stubBuff2[] = {
      "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76\r\n",
      "$GPGSA,A,3,10,07,05,02,29,04,08,13,,,,,1.72,1.03,1.38*0A\r\n",
      "$GPGSV,3,1,11,10,63,137,17,07,61,098,15,05,59,290,20,08,54,157,30*70\r\n",
      "$GPGSV,3,2,11,02,39,223,19,13,28,070,17,26,23,252,,04,14,186,14*79\r\n",
      "$GPGSV,3,3,11,29,09,301,24,16,09,020,,36,,,*76\r\n",
      "$GPRMC,092750.000,A,5321.6802,N,00630.3372,W,0.02,31.66,280511,,,A*43\r\n",
      "$GPGGA,092751.000,5321.6802,N,00630.3371,W,1,8,1.03,61.7,M,55.3,M,,*75\r\n",
      "$GPGSA,A,3,10,07,05,02,29,04,08,13,,,,,1.72,1.03,1.38*0A\r\n",
      "$GPGSV,3,1,11,10,63,137,17,07,61,098,15,05,59,290,20,08,54,157,30*70\r\n",
      "$GPGSV,3,2,11,02,39,223,16,13,28,070,17,26,23,252,,04,14,186,15*77\r\n",
      "$GPGSV,3,3,11,29,09,301,24,16,09,020,,36,,,*76\r\n", 
      "$GPRMC,092751.000,A,5321.6802,N,00630.3371,W,0.06,31.66,280511,,,A*45\r\n"
  }; 


  u8 GPGGA_Getline(u8 buf[]){
    return sizeof(buf);
  }    
#endif  /*defined(NMEA_PARSE_STUB_TEST)*/


void main(void)
{
  u8 len = 0;
  u8 * data;
  /* Infinite loop */
  
  
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  CLK_HSICmd(ENABLE);
  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
  CLK_ClockSwitchCmd(ENABLE); 

  Uart_Init();
  SPI_Slave_Init();

  BEEP_Init(BEEP_FREQUENCY_4KHZ);
  BEEP_Cmd(DISABLE);

   __enable_interrupt(); 

   while (1)
   {



#if defined(NMEA_PARSE_STUB_TEST)
        len = GPGGA_Getline((u8 *)stubBuff[0]);
        data = getPutData();
        nmea_parse_GGA((u8 *)stubBuff2[0], BufferSize, false, data);
        changePutData();
#else   /*defined(NMEA_PARSE_STUB_TEST)*/ 
        len = uart_Getline(Rx_Buffer);
        data = getPutData();
        if(nmea_parse_GGA(Rx_Buffer, len, false, data)){
  #if defined(NEED_RESOLVE_INFO)
          memset(RxGGA, 0x00, sizeof(RxGGA));     
          memcpy(RxGGA, Rx_Buffer, len);
  #else //defined(NEED_RESOLVE_INFO)
          memset(data, 0x00, sizeof(data)); 
          memcpy(data, Rx_Buffer, len);
  #endif //defined(NEED_RESOLVE_INFO)
          changePutData();          
        }

#endif        
        //Delay(0x00ff);

   }
  
}







