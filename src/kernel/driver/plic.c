#include "driver/plic.h"
#include "riscv.h"

#define PLIC_PRIORITY (PLIC_BASE_ADDR + 0x0)
#define PLIC_PENDING (PLIC_BASE_ADDR + 0x1000)
#define PLIC_MENABLE(hart) (PLIC_BASE_ADDR + 0x2000 + (hart)*0x100)
#define PLIC_SENABLE(hart) (PLIC_BASE_ADDR + 0x2080 + (hart)*0x100)
#define PLIC_MPRIORITY(hart) (PLIC_BASE_ADDR + 0x200000 + (hart)*0x2000)
#define PLIC_SPRIORITY(hart) (PLIC_BASE_ADDR + 0x201000 + (hart)*0x2000)
#define PLIC_MCLAIM(hart) (PLIC_BASE_ADDR + 0x200004 + (hart)*0x2000)
#define PLIC_SCLAIM(hart) (PLIC_BASE_ADDR + 0x201004 + (hart)*0x2000)

#ifndef PLIC_MODE
#error PLIC_MODE IS NOT DETERMINED!
#endif

typedef enum _plic_irq_mode_t{
    S_MODE=0x01, M_MODE=0x10,
}plic_irq_mode_t;

static plic_instance_t plic_instances[IRQN_MAX];

void plic_init(void) {
    int i;
    // set NULL for every callback
    for (i = 0; i < IRQN_MAX; i++) {
        plic_instances[i].callback = 0;
    }
    // clear priority
    for (i = 1; i < IRQN_MAX; i++) {
        plic_set_priority(i, 0);
    }
    
}

void _plic_irq_disable(plic_irq_t source, plic_irq_mode_t mode);
void _plic_set_threshold(uint32_t threshould, plic_irq_mode_t mode);
void plic_init_hart(void) {
    int i;

    /* Disable all interrupts for the current core. */
    for (i = 1; i < IRQN_MAX; i++) {
        _plic_irq_disable(i, S_MODE | M_MODE);
    }
    /* Set the threshold to zero. */
    _plic_set_threshold(0, S_MODE | M_MODE);


    /*
     * A successful claim will also atomically clear the corresponding
     * pending bit on the interrupt source. A target can perform a claim
     * at any time, even if the EIP is not set.
     */
    // i = 0;
    // while (plic->targets.target[core_id * 2 + PLIC_CTX].claim_complete > 0 && i < 100)
    // {
    //     /* This loop will clear pending bit on the interrupt source */
    //     i++;
    // }
}

void plic_set_priority(plic_irq_t source, uint32_t priority) {
    *(uint32_t*)(PLIC_PRIORITY + source*4) = priority;
}

void _plic_irq_enable(plic_irq_t source, plic_irq_mode_t mode) {
    uint64_t core_id = cpuid();
    int offset = source / 32;
    int bit = source % 32;
    if (mode & S_MODE) {
        *(uint32_t*)(PLIC_SENABLE(core_id) + 4 * offset) |= (1 << bit);
    }
    if (mode & M_MODE) {
        *(uint32_t*)(PLIC_MENABLE(core_id) + 4 * offset) |= (1 << bit);
    }
}

void plic_irq_enable(plic_irq_t source) {
    _plic_irq_enable(source, PLIC_MODE);
}

void _plic_irq_disable(plic_irq_t source, plic_irq_mode_t mode) {
    uint64_t core_id = cpuid();
    int offset = source / 32;
    int bit = source % 32;
    if (mode & S_MODE) {
        *(uint32*)(PLIC_SENABLE(core_id) + 4 * offset) &= ~(1 << bit);
    }
    if (mode & M_MODE) {
        *(uint32*)(PLIC_MENABLE(core_id) + 4 * offset) &= ~(1 << bit);
    }
}

void plic_irq_disable(plic_irq_t source) {
    _plic_irq_disable(source, PLIC_MODE);
}

void _plic_set_threshold(uint32_t threshould, plic_irq_mode_t mode) {
    uint64_t core_id = cpuid();
    if (mode & S_MODE) {
        *(uint32*)PLIC_SPRIORITY(core_id) = threshould;
    }
    if (mode & M_MODE) {
        *(uint32*)PLIC_MPRIORITY(core_id) = threshould;
    }
}

void plic_set_threshold(uint32_t threshould) {
    _plic_set_threshold(threshould, PLIC_MODE);
}

int _plic_claim(plic_irq_mode_t mode) {
    uint64_t core_id = cpuid();
    int irq = 0;
    if (mode & S_MODE) {
        irq = *(uint32*)PLIC_SCLAIM(core_id);
    }
    if (mode & M_MODE) {
        irq = *(uint32*)PLIC_MCLAIM(core_id);
    }
    return irq;
}

int plic_claim() {
    return _plic_claim(PLIC_MODE);
}

void _plic_complete(plic_irq_t source, plic_irq_mode_t mode) {
    uint64_t core_id = cpuid();
    if (mode & S_MODE) {
        *(uint32*)PLIC_SCLAIM(core_id) = source;
    }
    if (mode & M_MODE) {
        *(uint32*)PLIC_MCLAIM(core_id) = source;
    }
    
}

void plic_complete(plic_irq_t source) {
    _plic_complete(source, PLIC_MODE);
}

void plic_register_handler(plic_irq_t source, plic_irq_callback_t callback, void *ctx) {
    plic_instances[source] = (const plic_instance_t){
        .callback = callback,
        .ctx = ctx
    };
}

#include "printf.h"
int handle_ext_irq() {
    int ret;
    /* Get current IRQ num */
    uint32_t int_num = plic_claim();
    if (int_num == 0) return 0;

    // printf("IRQ: %d\n", int_num);
    if (plic_instances[int_num].callback) {
        ret = plic_instances[int_num].callback(plic_instances[int_num].ctx);
    } else {
        printf("no handler for IRQ: %d...\n", int_num);
        ret = -1;
    }

    /* Perform IRQ complete */
    plic_complete(int_num);
    
    return ret;
}