/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _DRIVER_PLIC_H
#define _DRIVER_PLIC_H

#include "platform.h"

#ifdef PLIC_ENABLE_S
#define PLIC_CTX 1
#else
#define PLIC_CTX 0
#endif
/**
 * @file
 * @brief      The PLIC complies with the RISC-V Privileged Architecture
 *             specification, and can support a maximum of 1023 external
 *             interrupt sources targeting up to 15,872 core contexts.
 *
 * @note       PLIC RAM Layout
 *
 * | Address   | Description                     |
 * |-----------|---------------------------------|
 * |0x0C000000 | Reserved                        |
 * |0x0C000004 | source 1 priority               |
 * |0x0C000008 | source 2 priority               |
 * |...        | ...                             |
 * |0x0C000FFC | source 1023 priority            |
 * |           |                                 |
 * |0x0C001000 | Start of pending array          |
 * |...        | (read-only)                     |
 * |0x0C00107C | End of pending array            |
 * |0x0C001080 | Reserved                        |
 * |...        | ...                             |
 * |0x0C001FFF | Reserved                        |
 * |           |                                 |
 * |0x0C002000 | target 0 enables                |
 * |0x0C002080 | target 1 enables                |
 * |...        | ...                             |
 * |0x0C1F1F80 | target 15871 enables            |
 * |0x0C1F2000 | Reserved                        |
 * |...        | ...                             |
 * |0x0C1FFFFC | Reserved                        |
 * |           |                                 |
 * |0x0C200000 | target 0 priority threshold     |
 * |0x0C200004 | target 0 claim/complete         |
 * |...        | ...                             |
 * |0x0C201000 | target 1 priority threshold     |
 * |0x0C201004 | target 1 claim/complete         |
 * |...        | ...                             |
 * |0x0FFFF000 | target 15871 priority threshold |
 * |0x0FFFF004 | target 15871 claim/complete     |
 *
 */

/* For c++ compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/* IRQ number settings */
#define PLIC_NUM_SOURCES    (IRQN_MAX - 1)
#define PLIC_NUM_PRIORITIES (7)

/* supported source is platform-specific */
typedef enum _plic_irq plic_irq_t;

/**
 * @brief      Interrupt Source Priorities
 *
 *             Each external interrupt source can be assigned a priority by
 *             writing to its 32-bit memory-mapped priority register. The
 *             number and value of supported priority levels can vary by
 *             implementa- tion, with the simplest implementations having all
 *             devices hardwired at priority 1, in which case, interrupts with
 *             the lowest ID have the highest effective priority. The priority
 *             registers are all WARL.
 */
typedef struct _plic_source_priorities
{
    /* 0x0C000000: Reserved, 0x0C000004-0x0C000FFC: 1-1023 priorities */
    uint32_t priority[1024];
} __attribute__((packed, aligned(4))) plic_source_priorities_t;

/**
 * @brief       Interrupt Pending Bits
 *
 *              The current status of the interrupt source pending bits in the
 *              PLIC core can be read from the pending array, organized as 32
 *              words of 32 bits. The pending bit for interrupt ID N is stored
 *              in bit (N mod 32) of word (N/32). Bit 0 of word 0, which
 *              represents the non-existent interrupt source 0, is always
 *              hardwired to zero. The pending bits are read-only. A pending
 *              bit in the PLIC core can be cleared by setting enable bits to
 *              only enable the desired interrupt, then performing a claim. A
 *              pending bit can be set by instructing the associated gateway to
 *              send an interrupt service request.
 */
typedef struct _plic_pending_bits
{
    /* 0x0C001000-0x0C00107C: Bit 0 is zero, Bits 1-1023 is pending bits */
    uint32_t u32[32];
    /* 0x0C001080-0x0C001FFF: Reserved */
    uint8_t resv[0xF80];
} __attribute__((packed, aligned(4))) plic_pending_bits_t;

/**
 * @brief       Target Interrupt Enables
 *
 *              For each interrupt target, each device’s interrupt can be
 *              enabled by setting the corresponding bit in that target’s
 *              enables registers. The enables for a target are accessed as a
 *              contiguous array of 32×32-bit words, packed the same way as the
 *              pending bits. For each target, bit 0 of enable word 0
 *              represents the non-existent interrupt ID 0 and is hardwired to
 *              0. Unused interrupt IDs are also hardwired to zero. The enables
 *              arrays for different targets are packed contiguously in the
 *              address space. Only 32-bit word accesses are supported by the
 *              enables array in RV32 systems. Implementations can trap on
 *              accesses to enables for non-existent targets, but must allow
 *              access to the full enables array for any extant target,
 *              treating all non-existent interrupt source’s enables as
 *              hardwired to zero.
 */
typedef struct _plic_target_enables
{
    /* 0x0C002000-0x0C1F1F80: target 0-15871 enables */
    struct
    {
        uint32_t enable[32 * 2];/* Offset 0x00-0x7C: Bit 0 is zero, Bits 1-1023 is bits*/
    } target[15872 / 2];

    /* 0x0C1F2000-0x0C1FFFFC: Reserved, size 0xE000 */
    uint8_t resv[0xE000];
} __attribute__((packed, aligned(4))) plic_target_enables_t;

/**
 * @brief       PLIC Targets
 *
 *              Target Priority Thresholds The threshold for a pending
 *              interrupt priority that can interrupt each target can be set in
 *              the target’s threshold register. The threshold is a WARL field,
 *              where different implementations can support different numbers
 *              of thresholds. The simplest implementation has a threshold
 *              hardwired to zero.
 *
 *              Target Claim Each target can perform a claim by reading the
 *              claim/complete register, which returns the ID of the highest
 *              priority pending interrupt or zero if there is no pending
 *              interrupt for the target. A successful claim will also
 *              atomically clear the corresponding pending bit on the interrupt
 *              source. A target can perform a claim at any time, even if the
 *              EIP is not set. The claim operation is not affected by the
 *              setting of the target’s priority threshold register.
 *
 *              Target Completion A target signals it has completed running a
 *              handler by writing the interrupt ID it received from the claim
 *              to the claim/complete register. This is routed to the
 *              corresponding interrupt gateway, which can now send another
 *              interrupt request to the PLIC. The PLIC does not check whether
 *              the completion ID is the same as the last claim ID for that
 *              target. If the completion ID does not match an interrupt source
 *              that is currently enabled for the target, the completion is
 *              silently ignored.
 */
typedef struct _plic_target
{
    /* 0x0C200000-0x0FFFF004: target 0-15871 */
    struct {
        uint32_t priority_threshold;/* Offset 0x000 */
        uint32_t claim_complete;    /* Offset 0x004 */
        uint8_t resv[0xFF5];        /* Offset 0x008, Size 0xFF8 */
    } target[15872]; // 原版这里直接将S级覆盖掉了
} __attribute__((packed, aligned(4))) plic_target_t;

/**
 * @brief       Platform-Level Interrupt Controller
 *
 *              PLIC is Platform-Level Interrupt Controller. The PLIC complies
 *              with the RISC-V Privileged Architecture specification, and can
 *              support a maximum of 1023 external interrupt sources targeting
 *              up to 15,872 core contexts.
 */
typedef struct _plic
{
    /* 0x0C000000-0x0C000FFC */
    plic_source_priorities_t source_priorities;
    /* 0x0C001000-0x0C001FFF */
    const plic_pending_bits_t pending_bits;
    /* 0x0C002000-0x0C1FFFFC */
    plic_target_enables_t target_enables;
    /* 0x0C200000-0x0FFFF004 */
    plic_target_t targets;
} __attribute__((packed, aligned(4))) plic_t;

extern volatile plic_t *const plic;

/**
 * @brief       Definitions for the interrupt callbacks
 */
typedef int (*plic_irq_callback_t)(void *ctx);

/**
 * @brief       Definitions for IRQ table instance
 */
typedef struct _plic_instance_t
{
    plic_irq_callback_t callback;
    void *ctx;
} plic_instance_t;

typedef struct _plic_callback_t
{
    plic_irq_callback_t callback;
    void *ctx;
    uint32_t priority;
} plic_interrupt_t;

/**
 * @brief       Initialize PLIC external interrupt
 *
 * @note        This function will set MIP_MEIP. The MSTATUS_MIE must set by user.
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
void plic_init(void);

/**
 * @brief       Initialize for hart
 *  
 */
void plic_init_hart(void);

/**
 * @brief       Enable PLIC external interrupt
 *
 * @param[in]   irq_number      external interrupt number
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */

int plic_irq_enable(plic_irq_t irq_number);

/**
 * @brief       Disable PLIC external interrupt
 *
 * @param[in]   irq_number  The external interrupt number
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int plic_irq_disable(plic_irq_t irq_number);

/**
 * @brief       Set IRQ priority
 *
 * @param[in]   irq_number      The external interrupt number
 * @param[in]   priority        The priority of external interrupt number
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int plic_set_priority(plic_irq_t irq_number, uint32_t priority);

/**
 * @brief       Get IRQ priority
 *
 * @param[in]   irq_number          The external interrupt number
 *
 * @return      The priority of external interrupt number
 */
uint32_t plic_get_priority(plic_irq_t irq_number);

/**
 * @brief       Claim an IRQ
 *
 * @return      The current IRQ number
 */
uint32_t plic_irq_claim(void);

/**
 * @brief       Complete an IRQ
 *
 * @param[in]   source      The source IRQ number to complete
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int plic_irq_complete(uint32_t source);

/**
 * @brief       Register user callback function by IRQ number
 *
 * @param[in]   irq             The irq
 * @param[in]   callback        The callback
 * @param       ctx             The context
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
void plic_irq_register(plic_irq_t irq, plic_irq_callback_t callback, void *ctx);

/**
 * @brief       Deegister user callback function by IRQ number
 *
 * @param[in]   irq     The irq
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
void plic_irq_deregister(plic_irq_t irq);

/**
 * @brief       Deegister user callback function by IRQ number
 *
 * @param[in]   irq     The irq
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
void plic_irq_unregister(plic_irq_t irq);

/**
 * @brief       Get IRQ table, Usage:
 *              plic_instance_t (*plic_instance)[IRQN_MAX] = plic_get_instance();
 *              ... plic_instance[x][y] ...;
 *
 * @return      the point of IRQ table
 */
plic_instance_t (*plic_get_instance(void))[IRQN_MAX];

/* For c++ compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_PLIC_H */
