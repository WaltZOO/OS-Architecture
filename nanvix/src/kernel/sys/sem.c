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
#include <nanvix/hal.h>
#include <nanvix/klib.h>
#include <nanvix/pm.h>
#include <unistd.h>
#include <errno.h>
#include <sys/sem.h>

// #define MAX_SEM 100

// struct semaphore
// {
//     int value;
//     unsigned key;
//     struct process **waiting_queue;
// };

static struct semaphore sem_tab[MAX_SEM];

PUBLIC int sem_check(unsigned key)
{
    struct semaphore current_semaphore = sem_tab[0];
    for (int i = 0; i < MAX_SEM; i++)
    {
        current_semaphore = sem_tab[i];
        if (current_semaphore.key == key)
            return i;
    }
    return -1;
}

/*
Create a semaphore with only 1 place
 */
PUBLIC int sem_create(unsigned key)
{
    struct semaphore current_semaphore = sem_tab[0];
    for (int i = 0; i < MAX_SEM; i++)
    {
        current_semaphore = sem_tab[i];
        if (current_semaphore.key == 0)
        {
            struct process *queue[MAX_SEM];
            struct semaphore sem =
                {
                    1,
                    key,
                    queue};
            sem_tab[i] = sem;
            return i;
        }
        
    }
    return -1;
}

PUBLIC int get_sem_value(int semid)
{
    // semid < 0 already checked in semctl
    if (semid > MAX_SEM || sem_tab[semid].value == -1)
        return -1;
    return sem_tab[semid].value;
}

PUBLIC int set_sem_value(int semid, int val)
{
    // semid < 0 already checked in semctl
    if (semid > MAX_SEM || sem_tab[semid].value == -1)
        return -1;
    sem_tab[semid].value = val;
    return 0;
}

PUBLIC int destroy(int semid)
{
    // semid < 0 already checked in semctl
    if (semid > MAX_SEM || sem_tab[semid].value == -1)
        return -1;
    sem_tab[semid].value = -1;
    sem_tab[semid].key = 0;
    return 0;
}

PUBLIC int up(int semid)
{
    // semid < 0 already checked in semctl
    if (semid < 0 || semid > MAX_SEM || sem_tab[semid].value == -1)
        return -1;
    disable_interrupts();
    sem_tab[semid].value++;
    if (sem_tab[semid].value <= 0){
        enable_interrupts();
        wakeup_one(sem_tab[semid].waiting_queue);
        return 0;
    }
    enable_interrupts();
    return 0;
}

PUBLIC int down(int semid)
{
    // semid < 0 already checked in semctl
    if (semid > MAX_SEM || sem_tab[semid].value == -1)
        return -1;
    disable_interrupts();
    sem_tab[semid].value--;

    if (sem_tab[semid].value < 0)
    {
        // block the current process
        enable_interrupts();
        sleep(sem_tab[semid].waiting_queue, 1);
        return 0;
    }
    enable_interrupts();
    return 0;
    
}