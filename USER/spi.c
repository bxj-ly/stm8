 /**
 * This file is part of spi.
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
#include "spi.h"
#include "stm8s_gpio.h"

void SPI_Slave_Init(void)
{
	SPI_DeInit();
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, ENABLE);
	
	GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_OUT_PP_LOW_FAST);
	OPT->OPT2 |= 0x02; /*set nss opt*/
	GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT);

   	SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_2, SPI_MODE_SLAVE,\
            SPI_CLOCKPOLARITY_HIGH, SPI_CLOCKPHASE_2EDGE, \
            SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 0x07);

   	
	SPI_ITConfig(SPI_IT_TXE ,ENABLE);
    SPI_ITConfig(SPI_IT_RXNE ,ENABLE);
	SPI_Cmd(ENABLE);
    SPI_ClearITPendingBit(SPI_IT_RXNE); 
}



    

