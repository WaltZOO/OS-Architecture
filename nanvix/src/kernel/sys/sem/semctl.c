// #include <nanvix/syscall.h>
// #include <nanvix/klib.h>
// #include <sys/sem.h>

PUBLIC int sys_semctl(int semid, int cmd, int val)
{
    kprintf("sys ctl");
    
    if (semid < 0 || semid >= sem_count)
        return -1;

    struct semaphore *sem = &semaphores[semid];

    switch (cmd)
    {
        case GETVAL:
            return sem->value;
        case SETVAL:
            sem->value = val;
            return 0;
        case IPC_RMID:
            
            return 0;
        default:
            return -1;
    }
}