#pragma once

#include <lib/stdtypes.h>

#define GDT_NULL_SEGMENT 0
#define GDT_CODE_SEGMENT 1
#define GDT_DATA_SEGMENT 2
#define GDT_STACK_SEGMENT 3
#define GDT_TSS_SEGMENT 4
#define GDT_ENTRIES_COUNT 5

/*
 * Global descriptor table entry
 */
struct GDT_entry_t {
    u16 limit_low: 16;
    u16 base_low: 16;
    u8 base_middle: 8;
    u8 type: 4; /* whether code (0b1010), data (0b0010) or stack (0b0110) */
    u8 s: 1; /* whether system descriptor */
    u8 dpl: 2; /* privilege level */
    u8 p: 1; /* whether segment prensent */
    u8 limit_high: 4;
    u8 a: 1; /* reserved for operation system */
    u8 zero: 1; /* zero */
    u8 db: 1; /* whether 16 or 32 segment */
    u8 g: 1; /* granularity */
    u8 base_high: 8;
} attribute(packed);

/*
 * Global descriptor table pointer
 */
struct GDT_pointer_t {
    u16 limit;
    u32 base;
} attribute(packed);

/*
 * Api
 */
extern void gdt_init();