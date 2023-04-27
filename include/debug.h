#include <env.h>
#include <pmap.h>
#include <printk.h>

#define TAILQ_DEBUG(INFO, elm, temp_elm, head, field)                                              \
    do {                                                                                           \
        struct Env *temp_elm;                                                                      \
        printk(#INFO " %x %s\nNow envs: ", elm->env_id,                                            \
               (TAILQ_EMPTY(head) ? "EMPTY" : "NOT EMPTY"));                                       \
        TAILQ_FOREACH (temp_elm, head, field) {                                                    \
            printk("id: %x ", temp_elm->env_id);                                                   \
        }                                                                                          \
        printk("\n");                                                                              \
    } while (0)