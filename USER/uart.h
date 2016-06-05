 /**
 * This file is part of uart.
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

#ifndef __UART_H
#define __UART_H
#include "stm8s_uart1.h"

void Uart_Init(void);
void UART1_SendByte(u8 data);
void UART1_SendString(u8* Data,u16 len);
u8 UART1_ReceiveByte(void);

#endif