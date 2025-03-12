/*
 * Copyright(C) 2011-2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 *
 * This file is part of Nanvix.
 *
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <nanvix/const.h>
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
