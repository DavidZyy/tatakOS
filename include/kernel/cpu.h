#ifndef _H_CPU_
#define _H_CPU_

#include "types.h"

/**
 * @brief Saved registers for kernel context switches.
 */
struct context {
    uint64 ra;
    uint64 sp;

    // callee-saved
    uint64 s0;
    uint64 s1;
    uint64 s2;
    uint64 s3;
    uint64 s4;
    uint64 s5;
    uint64 s6;
    uint64 s7;
    uint64 s8;
    uint64 s9;
    uint64 s10;
    uint64 s11;
};

/**
 * @brief Per-CPU state.
 */
struct cpu {
    /// @brief The process running on this cpu, or null.
    struct proc *proc;
    /// @brief swtch() here to enter scheduler().
    struct context context;
    /// @brief 
    struct pagevec *inactive_pvec;
    /// @brief 
    struct pagevec *active_pvec;
    /// @brief Depth of push_off() nesting.
    int noff;
    /// @brief Were interrupts enabled before push_off()?
    int intena;
};

extern struct cpu cpus[NUM_CORES];

#endif