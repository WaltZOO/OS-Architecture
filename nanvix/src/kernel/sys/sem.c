#include <nanvix/const.h>
#include <nanvix/hal.h>
#include <nanvix/klib.h>
#include <nanvix/pm.h>


struct semaphore
{
    int value;
    struct process *waiting_queue;
};


PUBLIC int sys_sem()
{
    unsigned key_tab[100];
    int nb_keys;
}