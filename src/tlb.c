/*
 * rv32emu is freely redistributable under the MIT License. See the file
 * "LICENSE" for information on usage and redistribution of this file.
 */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "riscv.h"
#include "tlb.h"

uint32_t vpn, offset;
#define get_vpn_and_offset()                                        \
    do {                                                            \
        vpn = get_vpn_by_vaddr_n_level(vaddr, tlb->level[i]);        \
        offset = tlb->level[i] == 1 ? vaddr & MASK(RV_PG_SHIFT + 10) \
                                   : vaddr & MASK(RV_PG_SHIFT);     \
    } while (0)

#define get_vpn_by_vaddr_n_level(vaddr, level)                               \
    ({                                                                       \
        level == 1 ? (vaddr & ~MASK(RV_PG_SHIFT + 10)) >> (RV_PG_SHIFT + 10) \
                   : (vaddr & ~MASK(RV_PG_SHIFT)) >> (RV_PG_SHIFT);          \
    })

bool tlb_lookup(tlb_t *tlb, tlb_type_t type, uint32_t vaddr, uint32_t *addr, uint32_t access)
{
    struct list_head *tlb_list =
        type == iTLB ? &tlb->itlb_list : &tlb->dtlb_list;
    tlb_entry_t *entry = NULL;

    for(int i = 0; i < tlb->cap; i++){
        get_vpn_and_offset();
        if(tlb->vpn[i] == vpn && tlb->access[i] == access){
        	*addr = tlb->ppn[i] | offset;
        	return true;
        }
    }

    /* TLB miss */
    return false;
}

/* FIFO flavor */
void tlb_evict(tlb_t *tlb, tlb_type_t type)
{
    //struct list_head *tlb_list =
    //    type == iTLB ? &tlb->itlb_list : &tlb->dtlb_list;
    //uint32_t *tlb_size = type == iTLB ? &tlb->itlb_size : &tlb->dtlb_size;

    //assert(!list_empty(tlb_list));

    //tlb_entry_t *first_entry = list_first_entry(tlb_list, tlb_entry_t, list);
    //assert(first_entry);
    //list_del_init(&first_entry->list);
    //free(first_entry);
    //(*tlb_size)--;
    for(int i = 0; i < tlb->cap - 1; i++){
    	tlb->vpn[i] = tlb->vpn[i + 1];
    	tlb->level[i] = tlb->level[i + 1];
    	tlb->ppn[i] = tlb->ppn[i + 1];
    }
    tlb->size--;
}

FORCE_INLINE void _tlb_refill(tlb_t *tlb, tlb_type_t type, tlb_entry_t *entry)
{
    struct list_head *tlb_list =
        type == iTLB ? &tlb->itlb_list : &tlb->dtlb_list;
    uint32_t *tlb_size = type == iTLB ? &tlb->itlb_size : &tlb->dtlb_size;

    list_add_tail(&entry->list, tlb_list);
    (*tlb_size)++;
}

void tlb_refill(tlb_t *tlb,
                tlb_type_t type,
                uint32_t vaddr,
                uint32_t ppn,
                int level,
		uint32_t access)
{
    uint32_t tlb_size = type == iTLB ? tlb->itlb_size : tlb->dtlb_size;
    uint32_t tlb_capacity =
        type == iTLB ? tlb->itlb_capacity : tlb->dtlb_capacity;

    /* TLB is full, so evict */
    //if (tlb_size == tlb_capacity)
    //    tlb_evict(tlb, type);

    if (tlb->size == tlb->cap)
        tlb_evict(tlb, type);

    //tlb_entry_t *new_entry = calloc(1, sizeof(tlb_entry_t));
    //assert(new_entry);
    ////INIT_LIST_HEAD(&new_entry->list);

    ///*
    // * Ignore the offset for both vpn and ppn since
    // * the offset might changes within the same page access.
    // */
    //new_entry->vpn = get_vpn_by_vaddr_n_level(vaddr, level);
    //new_entry->ppn = ppn;
    //new_entry->level = level;

    //_tlb_refill(tlb, type, new_entry);
    tlb->vpn[tlb->size] = get_vpn_by_vaddr_n_level(vaddr, level);
    tlb->ppn[tlb->size] = ppn;
    tlb->level[tlb->size] = level;
    tlb->access[tlb->size] = access;
    tlb->size++;
    assert(tlb->size <= tlb->cap);
}

FORCE_INLINE void _tlb_flush(tlb_t *tlb,
                             tlb_type_t type,
                             uint32_t asid,
                             uint32_t vaddr)
{
    struct list_head *tlb_list =
        type == iTLB ? &tlb->itlb_list : &tlb->dtlb_list;
    tlb_entry_t *entry = NULL;

    for(int i = 0; i < tlb->cap; i++){
    	tlb->vpn[i] = -1;
    }
}

void tlb_flush(tlb_t *tlb, uint32_t asid, uint32_t vaddr)
{
    _tlb_flush(tlb, iTLB, asid, vaddr);
    _tlb_flush(tlb, dTLB, asid, vaddr);
}

FORCE_INLINE void tlb_init(tlb_t *tlb, tlb_type_t type, int tlb_init_capacity)
{
    struct list_head *tlb_list =
        type == iTLB ? &tlb->itlb_list : &tlb->dtlb_list;
    uint32_t *tlb_size = type == iTLB ? &tlb->itlb_size : &tlb->dtlb_size;
    uint32_t *tlb_capacity =
        type == iTLB ? &tlb->itlb_capacity : &tlb->dtlb_capacity;

    INIT_LIST_HEAD(tlb_list);
    *tlb_size = 0;
    *tlb_capacity = tlb_init_capacity;
}

tlb_t *tlb_new(uint32_t tlb_size)
{
    tlb_t *tlb = calloc(1, sizeof(tlb_t));
    assert(tlb);

    //tlb_init(tlb, iTLB, tlb_size);
    //tlb_init(tlb, dTLB, tlb_size);

    tlb->size = 0;
    tlb->cap = 64;

    return tlb;
}

FORCE_INLINE void _tlb_delete(tlb_t *tlb, tlb_type_t type)
{
    struct list_head *tlb_list =
        type == iTLB ? &tlb->itlb_list : &tlb->dtlb_list;
    tlb_entry_t *entry = NULL;

#ifdef __HAVE_TYPEOF
    list_for_each_entry (entry, tlb_list, list)
#else
    list_for_each_entry (entry, tlb_list, list, tlb_entry_t)
#endif
    {
        list_del(&entry->list);
        /* FIXME: poweroff / poweroff -f cause segmentation fault */
        free(entry);
    }
}

void tlb_delete(tlb_t *tlb)
{
    _tlb_delete(tlb, iTLB);
    _tlb_delete(tlb, dTLB);
    free(tlb);
}
