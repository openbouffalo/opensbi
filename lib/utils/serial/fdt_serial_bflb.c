/*
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/serial/bflb-uart.h>

static int serial_bflb_init(void *fdt, int nodeoff,
			    const struct fdt_match *match)
{
	struct platform_uart_data uart = { 0 };
	int rc;

	rc = fdt_parse_uart_node(fdt, nodeoff, &uart);
	if (rc)
		return rc;

	return bflb_uart_init(uart.addr, uart.freq, uart.baud);
}

static const struct fdt_match serial_bflb_match[] = {
	{ .compatible = "bflb,bl808-uart" },
	{ }
};

struct fdt_serial fdt_serial_bflb = {
	.match_table = serial_bflb_match,
	.init = serial_bflb_init
};
