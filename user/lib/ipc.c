// User-level IPC library routines

#include <env.h>
#include <lib.h>
#include <mmu.h>

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


// void ipc_dfs_send(struct Env *e, u_int val, void * srcva, u_int perm) {
// 	ipc_send(e->env_id, val, srcva, perm);
// 	if (e->env_son != NULL) {
// 		ipc_dfs_send(e->env_son, val, srcva, perm);
// 	}
// 	if (e->env_bro != NULL) {
// 		ipc_dfs_send(e->env_bro, val, srcva, perm);
// 	}
// }

void ipc_broadcast(u_int val, void * srcva, u_int perm) {
	struct Env *curenv = envs + ENVX(syscall_getenvid());
    struct Env *e;
	struct Env_sched_list env_sched_list;
	syscall_getenvs(&env_sched_list);
    TAILQ_FOREACH(e, &env_sched_list, env_sched_link) {
        struct Env *tmp = e;
        u_int fa = 0;
        while (tmp->env_parent_id) {
			debugf("now %x, fa %x\n", tmp->env_id, tmp->env_parent_id);
            fa = tmp->env_parent_id;
            if (fa == curenv->env_id){
				debugf("%x try send\n", e->env_id);
                ipc_send(e->env_id, val, srcva, perm);
                break;
            }
            tmp = envs + ENVX(fa);
        }
    }
}
