/*
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef __SERIAL_BFLB_UART_H__
#define __SERIAL_BFLB_UART_H__

#include <sbi/sbi_types.h>

int bflb_uart_init(unsigned long base, u32 in_freq, u32 baudrate);

#endif
