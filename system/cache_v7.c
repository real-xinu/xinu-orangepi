/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 *
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 * Aneesh V <aneesh@ti.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/* Taken from U-boot sources 2015.01
 * arch/arm/cpu/armv7/cache_v7.c
 */

#include <xinu.h>

#ifndef __weak
#define __weak
#endif

/* Copied here from U-boot
 *  arch/arm/include/asm/utils.h
 */

static inline long log_2_n_round_up(unsigned long n)
{
        long log2n = -1;
        unsigned long temp = n;

        while (temp) {
                log2n++;
                temp >>= 1;
        }

        if (n & (n - 1))
                return log2n + 1; /* not power of 2 - round up */
        else
                return log2n; /* power of 2 */
}

static inline long log_2_n_round_down(unsigned long n)
{
        long log2n = -1;
        unsigned long temp = n;

        while (temp) {
                log2n++;
                temp >>= 1;
        }

        return log2n;
}

#define ARMV7_DCACHE_INVAL_ALL		1
#define ARMV7_DCACHE_CLEAN_INVAL_ALL	2
#define ARMV7_DCACHE_INVAL_RANGE	3
#define ARMV7_DCACHE_CLEAN_INVAL_RANGE	4

#ifndef CONFIG_SYS_DCACHE_OFF
/*
 * Write the level and type you want to Cache Size Selection Register(CSSELR)
 * to get size details from Current Cache Size ID Register(CCSIDR)
 */
static void set_csselr(unsigned long level, unsigned long type)
{
	unsigned long csselr = level << 1 | type;

	/* Write to Cache Size Selection Register(CSSELR) */
	asm volatile ("mcr p15, 2, %0, c0, c0, 0" : : "r" (csselr));
}

static unsigned long get_ccsidr(void)
{
	unsigned long ccsidr;

	/* Read current CP15 Cache Size ID Register */
	asm volatile ("mrc p15, 1, %0, c0, c0, 0" : "=r" (ccsidr));
	return ccsidr;
}

static unsigned long get_clidr(void)
{
	unsigned long clidr;

	/* Read current CP15 Cache Level ID Register */
	asm volatile ("mrc p15,1,%0,c0,c0,1" : "=r" (clidr));
	return clidr;
}

static void v7_inval_dcache_level_setway(unsigned long level, unsigned long num_sets,
					 unsigned long num_ways, unsigned long way_shift,
					 unsigned long log2_line_len)
{
	int way, set;
	unsigned long setway;

	/*
	 * For optimal assembly code:
	 *	a. count down
	 *	b. have bigger loop inside
	 */
	for (way = num_ways - 1; way >= 0 ; way--) {
		for (set = num_sets - 1; set >= 0; set--) {
			setway = (level << 1) | (set << log2_line_len) |
				 (way << way_shift);
			/* Invalidate data/unified cache line by set/way */
			asm volatile ("	mcr p15, 0, %0, c7, c6, 2"
					: : "r" (setway));
		}
	}

	/* DSB to make sure the operation is complete */
	dsb ();
}

static void v7_clean_inval_dcache_level_setway(unsigned long level, unsigned long num_sets,
					       unsigned long num_ways, unsigned long way_shift,
					       unsigned long log2_line_len)
{
	int way, set;
	unsigned long setway;

	/*
	 * For optimal assembly code:
	 *	a. count down
	 *	b. have bigger loop inside
	 */
	for (way = num_ways - 1; way >= 0 ; way--) {
		for (set = num_sets - 1; set >= 0; set--) {
			setway = (level << 1) | (set << log2_line_len) |
				 (way << way_shift);
			/*
			 * Clean & Invalidate data/unified
			 * cache line by set/way
			 */
			asm volatile ("	mcr p15, 0, %0, c7, c14, 2"
					: : "r" (setway));
		}
	}

	/* DSB to make sure the operation is complete */
	dsb ();
}

static void v7_maint_dcache_level_setway(unsigned long level, unsigned long operation)
{
	unsigned long ccsidr;
	unsigned long num_sets, num_ways, log2_line_len, log2_num_ways;
	unsigned long way_shift;

	set_csselr(level, ARMV7_CSSELR_IND_DATA_UNIFIED);

	ccsidr = get_ccsidr();

	log2_line_len = ((ccsidr & CCSIDR_LINE_SIZE_MASK) >>
				CCSIDR_LINE_SIZE_OFFSET) + 2;
	/* Converting from words to bytes */
	log2_line_len += 2;

	num_ways  = ((ccsidr & CCSIDR_ASSOCIATIVITY_MASK) >>
			CCSIDR_ASSOCIATIVITY_OFFSET) + 1;
	num_sets  = ((ccsidr & CCSIDR_NUM_SETS_MASK) >>
			CCSIDR_NUM_SETS_OFFSET) + 1;
	/*
	 * According to ARMv7 ARM number of sets and number of ways need
	 * not be a power of 2
	 */
	log2_num_ways = log_2_n_round_up(num_ways);

	way_shift = (32 - log2_num_ways);
	if (operation == ARMV7_DCACHE_INVAL_ALL) {
		v7_inval_dcache_level_setway(level, num_sets, num_ways,
				      way_shift, log2_line_len);
	} else if (operation == ARMV7_DCACHE_CLEAN_INVAL_ALL) {
		v7_clean_inval_dcache_level_setway(level, num_sets, num_ways,
						   way_shift, log2_line_len);
	}
}

static void v7_maint_dcache_all(unsigned long operation)
{
	unsigned long level, cache_type, level_start_bit = 0;
	unsigned long clidr = get_clidr();

	for (level = 0; level < 7; level++) {
		cache_type = (clidr >> level_start_bit) & 0x7;
		if ((cache_type == ARMV7_CLIDR_CTYPE_DATA_ONLY) ||
		    (cache_type == ARMV7_CLIDR_CTYPE_INSTRUCTION_DATA) ||
		    (cache_type == ARMV7_CLIDR_CTYPE_UNIFIED))
			v7_maint_dcache_level_setway(level, operation);
		level_start_bit += 3;
	}
}

static void v7_dcache_clean_inval_range(unsigned long start, unsigned long stop, unsigned long line_len)
{
	unsigned long mva;

	/* Align start to cache line boundary */
	start &= ~(line_len - 1);
	for (mva = start; mva < stop; mva = mva + line_len) {
		// KYU printf ( "-------- Invalidate %d at %08x\n", line_len, mva );
		/* DCCIMVAC - Clean & Invalidate data cache by MVA to PoC */
		asm volatile ("mcr p15, 0, %0, c7, c14, 1" : : "r" (mva));
	}
}

static void v7_dcache_inval_range(unsigned long start, unsigned long stop, unsigned long line_len)
{
	unsigned long mva;

	/*
	 * If start address is not aligned to cache-line do not
	 * invalidate the first cache-line
	 */
	if (start & (line_len - 1)) {
		printf("ERROR: %s - start address is not aligned - 0x%08x\n",
			__func__, start);
		printf ( "KYU --- line_len is: %d\n", line_len );
		/* move to next cache line */
		start = (start + line_len - 1) & ~(line_len - 1);
	}

	/*
	 * If stop address is not aligned to cache-line do not
	 * invalidate the last cache-line
	 */
	if (stop & (line_len - 1)) {
		printf("ERROR: %s - stop address is not aligned - 0x%08x\n",
			__func__, stop);
		/* align to the beginning of this cache line */
		stop &= ~(line_len - 1);
	}

	for (mva = start; mva < stop; mva = mva + line_len) {
		/* DCIMVAC - Invalidate data cache by MVA to PoC */
		asm volatile ("mcr p15, 0, %0, c7, c6, 1" : : "r" (mva));
	}
}

static void v7_dcache_maint_range(unsigned long start, unsigned long stop, unsigned long range_op)
{
	unsigned long line_len, ccsidr;

	ccsidr = get_ccsidr();
	line_len = ((ccsidr & CCSIDR_LINE_SIZE_MASK) >>
			CCSIDR_LINE_SIZE_OFFSET) + 2;
	/* Converting from words to bytes */
	line_len += 2;
	/* converting from log2(linelen) to linelen */
	line_len = 1 << line_len;

	switch (range_op) {
	case ARMV7_DCACHE_CLEAN_INVAL_RANGE:
		v7_dcache_clean_inval_range(start, stop, line_len);
		break;
	case ARMV7_DCACHE_INVAL_RANGE:
		v7_dcache_inval_range(start, stop, line_len);
		break;
	}

	/* DSB to make sure the operation is complete */
	dsb ();
}

/* Invalidate TLB */
static void v7_inval_tlb(void)
{
	/* Invalidate entire unified TLB */
	asm volatile ("mcr p15, 0, %0, c8, c7, 0" : : "r" (0));
	/* Invalidate entire data TLB */
	asm volatile ("mcr p15, 0, %0, c8, c6, 0" : : "r" (0));
	/* Invalidate entire instruction TLB */
	asm volatile ("mcr p15, 0, %0, c8, c5, 0" : : "r" (0));

	/* Full system DSB - make sure that the invalidation is complete */
	dsb ();

	/* Full system ISB - make sure the instruction stream sees it */
	isb ();
}

void invalidate_dcache_all(void)
{
	v7_maint_dcache_all(ARMV7_DCACHE_INVAL_ALL);

// 	v7_outer_cache_inval_all();
}

/*
 * Performs a clean & invalidation of the entire data cache
 * at all levels
 */
void flush_dcache_all(void)
{
	v7_maint_dcache_all(ARMV7_DCACHE_CLEAN_INVAL_ALL);

// 	v7_outer_cache_flush_all();
}

/*
 * Invalidates range in all levels of D-cache/unified cache used:
 * Affects the range [start, stop - 1]
 */
void invalidate_dcache_range(unsigned long start, unsigned long stop)
{
	v7_dcache_maint_range(start, stop, ARMV7_DCACHE_INVAL_RANGE);

// 	v7_outer_cache_inval_range(start, stop);
}

/*
 * Flush range(clean & invalidate) from all levels of D-cache/unified
 * cache used:
 * Affects the range [start, stop - 1]
 */
void flush_dcache_range(unsigned long start, unsigned long stop)
{
	v7_dcache_maint_range(start, stop, ARMV7_DCACHE_CLEAN_INVAL_RANGE);

// 	v7_outer_cache_flush_range(start, stop);
}

void arm_init_before_mmu(void)
{
// 	v7_outer_cache_enable();
	invalidate_dcache_all();
	v7_inval_tlb();
}

void mmu_page_table_flush(unsigned long start, unsigned long stop)
{
	flush_dcache_range(start, stop);
	v7_inval_tlb();
}

/*
 * Flush range from all levels of d-cache/unified-cache used:
 * Affects the range [start, start + size - 1]
 */
void  flush_cache(unsigned long start, unsigned long size)
{
	flush_dcache_range(start, start + size);
}
#else /* #ifndef CONFIG_SYS_DCACHE_OFF */
void invalidate_dcache_all(void)
{
}

void flush_dcache_all(void)
{
}

void invalidate_dcache_range(unsigned long start, unsigned long stop)
{
}

void flush_dcache_range(unsigned long start, unsigned long stop)
{
}

void arm_init_before_mmu(void)
{
}

void  flush_cache(unsigned long start, unsigned long size)
{
}

void mmu_page_table_flush(unsigned long start, unsigned long stop)
{
}

void arm_init_domains(void)
{
}
#endif /* #ifndef CONFIG_SYS_DCACHE_OFF */



/* ======================================================================================================= */
/* ======================================================================================================= */
/* ======================================================================================================= */
/* ======================================================================================================= */

#ifdef not_any_more

/* Invalidate entire I-cache and branch predictor array */
void invalidate_icache_all(void)
{
	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm volatile ("mcr p15, 0, %0, c7, c5, 0" : : "r" (0));

	/* Invalidate entire branch predictor array */
	asm volatile ("mcr p15, 0, %0, c7, c5, 6" : : "r" (0));

	/* Full system DSB - make sure that the invalidation is complete */
	// CP15DSB;
	dsb ();

	/* ISB - make sure the instruction stream sees it */
	// CP15ISB;
	isb ();
}

#endif /* not_any_more */

/* THE END */

