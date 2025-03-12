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
#include <errno.h>

#define MAX_SEM 100

struct semaphore
{
    int value;
    unsigned key;
    struct process **waiting_queue;
    int queue_length;
};

static struct semaphore sem_tab[MAX_SEM];

PUBLIC sem_check(unsigned key)
{
    struct semaphore current_semaphore = sem_tab[0];
    for (int i = 0; i < MAX_SEM; i++)
    {
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
        if (current_semaphore.key == NULL)
        {
            struct process *queue;
            struct semaphore sem =
                {
                    1,
                    key,
                    queue,
                    0};
            sem_tab[i] = sem;
            return i;
        }
    }
    return -1;
}

PUBLIC int get_sem_value(int semid){
    // semid < 0 already checked in semctl
    if (semid > MAX_SEM || sem_tab[semid].key == NULL)
        return -1;
    return sem_tab[semid].value;
}

PUBLIC int set_sem_value(int semid, int val)
{
    // semid < 0 already checked in semctl
    if (semid > MAX_SEM || sem_tab[semid].key == NULL)
        return -1;
    sem_tab[semid].value = val;
}

PUBLIC int destroy(int semid)
{
    // semid < 0 already checked in semctl
    if (semid > MAX_SEM || sem_tab[semid].key == NULL)
        return -1;
    sem_tab[semid].value = NULL;
    sem_tab[semid].key = NULL;
    sem_tab[semid].waiting_queue = NULL;
    sem_tab[semid].queue_length = NULL;
}

PUBLIC void up(int semid)
{
    // semid < 0 already checked in semctl
    if (semid > MAX_SEM || sem_tab[semid].key == NULL)
        return -1;
    if (sem_tab[semid].value == 0)
    {
        wakeup_one(sem_tab[semid].waiting_queue);
        sem_tab[semid].queue_length--;
    }
    else
        sem_tab[semid].value++;
}

PUBLIC void down(int semid)
{
    // semid < 0 already checked in semctl
    if (semid > MAX_SEM || sem_tab[semid].key == NULL)
        return -1;
    if (sem_tab[semid].value > 0)
        sem_tab[semid].value--;
    
    else
    {
        sem_tab[semid].waiting_queue[sem_tab[semid].queue_length] = get_current_process();
        sem_tab[semid].queue_length++;
        sleep(sem_tab[semid].waiting_queue, 1);
    }
}