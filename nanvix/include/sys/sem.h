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

#ifndef SEM_H_
#define SEM_H_

/**
 * @brief Comand values for semaphores.
 */
/**@{*/
#define GETVAL 0   /**< Returns the value of a semaphore. */
#define SETVAL 1   /**< Sets the value of a semaphore.    */
#define IPC_RMID 3 /**< Destroys a semaphore.            */
/**@}*/

/* Forward definitions. */

#define MAX_SEM 100

struct semaphore
{
    int value;
    unsigned key;
    struct process **waiting_queue;
};


extern int semget(unsigned);
extern int semctl(int, int, int);
extern int semop(int, int);

extern int sem_check(unsigned key);
extern int sem_create(unsigned key);
extern int get_sem_value(int semid);
extern int set_sem_value(int semid, int val);
extern int destroy(int semid);
extern int up(int semid);
extern int down(int semid);

#endif /* SEM_H_ */
