/*
 * rv32emu is freely redistributable under the MIT License. See the file
 * "LICENSE" for information on usage and redistribution of this file.
 */

#pragma once

#include "utils.h"

typedef enum {
    iTLB,
    dTLB,
} tlb_type_t;

typedef struct {
    uint32_t vpn;
    uint32_t ppn;
    int level;
    struct list_head list;
} tlb_entry_t;

typedef struct {
    uint32_t dtlb_size;
    uint32_t dtlb_capacity;
    struct list_head dtlb_list;

    uint32_t itlb_size;
    uint32_t itlb_capacity;
    struct list_head itlb_list;

    uint32_t vpn[64];
    uint32_t level[64];
    uint32_t ppn[64];
    uint32_t access[64];
    uint32_t size;
    uint32_t cap;
} tlb_t;

/*
 * lookup translated gPA in TLB
 * return true and store gPA in addr if found else return false
 */
bool tlb_lookup(tlb_t *tlb, tlb_type_t type, uint32_t vaddr, uint32_t *addr, uint32_t access);

/* Refill the TLB for a page fault vaddr */
void tlb_refill(tlb_t *tlb,
                tlb_type_t type,
                uint32_t vaddr,
                uint32_t ppn,
                int level,
		uint32_t access);

/* flush iTLB and dTLB */
void tlb_flush(tlb_t *tlb, uint32_t asid, uint32_t vaddr);

/* create a TLB instance */
tlb_t *tlb_new(uint32_t tlb_size);

/* delete a TLB instance */
void tlb_delete(tlb_t *tlb);
