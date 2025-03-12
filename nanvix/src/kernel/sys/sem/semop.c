#include <nanvix/syscall.h>
#include <nanvix/klib.h>
#include <sys/sem.h>

PUBLIC int sys_semop(int semid, int op)
{
    kprintf("sys op");
    // for (int i = 0; i < nb_keys; i++)
    // {
    //     if (key == key_tab[i])
    //         return key_tab[i].sid;
    // }

}