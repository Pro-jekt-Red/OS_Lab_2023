// User-level IPC library routines

#include <env.h>
#include <lib.h>
#include <mmu.h>
#include <drivers/dev_rtc.h>

// Send val to whom.  This function keeps trying until
// it succeeds.  It should panic() on any error other than
// -E_IPC_NOT_RECV.
//
// Hint: use syscall_yield() to be CPU-friendly.
void ipc_send(u_int whom, u_int val, const void *srcva, u_int perm) {
	int r;
	while ((r = syscall_ipc_try_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
		syscall_yield();
	}
	user_assert(r == 0);
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int ipc_recv(u_int *whom, void *dstva, u_int *perm) {
	int r = syscall_ipc_recv(dstva);
	if (r != 0) {
		user_panic("syscall_ipc_recv err: %d", r);
	}

	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}

u_int get_time(u_int *us) {
	u_int s;
	syscall_read_dev(&s, DEV_RTC_ADDRESS + DEV_RTC_TRIGGER_READ, 1);
	syscall_read_dev(&s, DEV_RTC_ADDRESS + DEV_RTC_SEC, 4);
	syscall_read_dev(us, DEV_RTC_ADDRESS + DEV_RTC_USEC, 4);
	return s;
}
void usleep(u_int us) {
	u_int start_us;
	u_int start_s = get_time(&start_us);
	while (1) {
		u_int time_us;
	    u_int time_s = get_time(&time_us);
		if (1000000ll * time_s + time_us >= 1000000ll * start_s + start_us + us) {
			return;
		} else {
			syscall_yield();
		}
	}
}
