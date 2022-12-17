/*
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <sbi/riscv_io.h>
#include <sbi/sbi_bitops.h>
#include <sbi/sbi_console.h>
#include <sbi_utils/serial/bflb-uart.h>

#define UART_TX_CFG			0x00
#define UART_TX_CFG_TX_BIT_CNT_P		GENMASK(12, 11)
#define UART_TX_CFG_TX_BIT_CNT_D		GENMASK(10,  8)
#define UART_TX_CFG_TX_FRM_EN			BIT(2)
#define UART_TX_CFG_TX_EN			BIT(0)
#define UART_RX_CFG			0x04
#define UART_RX_CFG_RX_BIT_CNT_D		GENMASK(10,  8)
#define UART_RX_CFG_RX_EN			BIT(0)
#define UART_BIT_PRD			0x08
#define UART_BIT_PRD_RX_BIT_PRD			GENMASK(31, 16)
#define UART_BIT_PRD_TX_BIT_PRD			GENMASK(15,  0)
#define UART_FIFO_CFG1			0x84
#define UART_FIFO_CFG1_RX_FIFO_CNT		GENMASK(13,  8)
#define UART_FIFO_CFG1_TX_FIFO_CNT		GENMASK( 5,  0)
#define UART_FIFO_WDATA			0x88
#define UART_FIFO_RDATA			0x8c

static volatile void *uart_base;

static int bflb_uart_rx_avail(void)
{
	u32 val = readl(uart_base + UART_FIFO_CFG1);

	return EXTRACT_FIELD(val, UART_FIFO_CFG1_RX_FIFO_CNT);
}

static int bflb_uart_tx_avail(void)
{
	u32 val = readl(uart_base + UART_FIFO_CFG1);

	return EXTRACT_FIELD(val, UART_FIFO_CFG1_TX_FIFO_CNT);
}

static void bflb_uart_putc(char ch)
{
	while (!bflb_uart_tx_avail())
		;

	writel(ch, uart_base + UART_FIFO_WDATA);
}

static int bflb_uart_getc(void)
{
	if (!bflb_uart_rx_avail())
		return -1;

	return readl(uart_base + UART_FIFO_RDATA);
}

static struct sbi_console_device bflb_console = {
	.name = "bflb_uart",
	.console_putc = bflb_uart_putc,
	.console_getc = bflb_uart_getc
};

int bflb_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	u32 val;

	uart_base = (volatile void *)base;

	val = INSERT_FIELD(0, UART_TX_CFG_TX_BIT_CNT_P, 2) |
	      INSERT_FIELD(0, UART_TX_CFG_TX_BIT_CNT_D, 7) |
	      UART_TX_CFG_TX_FRM_EN |
	      UART_TX_CFG_TX_EN;
	writel(val, uart_base + UART_TX_CFG);

	val = INSERT_FIELD(0, UART_RX_CFG_RX_BIT_CNT_D, 7) |
	      UART_RX_CFG_RX_EN;
	writel(val, uart_base + UART_RX_CFG);

	if (in_freq >= baudrate) {
		u32 period = in_freq / baudrate - 1;

		val = INSERT_FIELD(0, UART_BIT_PRD_RX_BIT_PRD, period) |
		      INSERT_FIELD(0, UART_BIT_PRD_TX_BIT_PRD, period);
		writel(val, uart_base + UART_BIT_PRD);
	}

	sbi_console_set_device(&bflb_console);

	return 0;
}
