/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Samuel Holland <samuel@sholland.org>
 */

#include <platform_override.h>
#include <sbi/riscv_asm.h>
#include <sbi/sbi_bitops.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <thead_c9xx.h>

static int bl808_early_init(bool cold_boot, const struct fdt_match *match)
{
	csr_set(THEAD_C9XX_CSR_MXSTATUS, BIT(22));
	csr_set(THEAD_C9XX_CSR_MHCR, 0x117f);
	csr_set(THEAD_C9XX_CSR_MHINT, 0x304);

	return 0;
}

static const struct fdt_match bl808_match[] = {
	{ .compatible = "bflb,bl808" },
	{ },
};

const struct platform_override bl808 = {
	.match_table	= bl808_match,
	.early_init	= bl808_early_init,
};
