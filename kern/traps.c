#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_reserved(void);
extern void handle_ov(void);

void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
    [0] = handle_int,
    [2 ... 3] = handle_tlb,
#if !defined(LAB) || LAB >= 4
    [1] = handle_mod,
    [8] = handle_sys,
#endif
    [12] = handle_ov,
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %2d", (tf->cp0_cause >> 2) & 0x1f);
}

void do_ov(struct Trapframe *tf) {
	// 你需要在此处实现问题描述的处理要求
    curenv->env_ov_cnt++;
    u_int *EPC = tf->cp0_epc;
    u_long pa = va2pa(curenv->env_pgdir, EPC);
    u_int *kva = KADDR(pa);
    // printk("*kva = %d, *EPC = %d\n", *kva, *EPC);
    if ((*EPC >> 26) == 8) { // addi
        tf->regs[(*kva >> 16) & 31] = tf->regs[(*kva >> 21) & 31] / 2 + (*kva >> 1 & ((1 << 15) - 1));
        tf->cp0_epc += 4;
    }
    else if ((*EPC & ((1 << 11) - 1)) == 32) { // add
        // *kva += 1;
        tf->regs[(*kva >> 11) & 31] = (u_int)tf->regs[(*kva >> 21) & 31] + (u_int)tf->regs[(*kva >> 16) & 31];
        tf->cp0_epc += 4;
        printk("add ov handled\n");
    }
    else if ((*EPC & ((1 << 11) - 1)) == 34) { // sub
        // *kva += 1;
        tf->regs[(*kva >> 11) & 31] = (u_int)tf->regs[(*kva >> 21) & 31] - (u_int)tf->regs[(*kva >> 16) & 31];
        tf->cp0_epc += 4;
        printk("sub ov handled\n");
    }
}
