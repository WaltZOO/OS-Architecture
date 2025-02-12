/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis <davidsondfgl@hotmail.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <nanvix/clock.h>
#include <nanvix/const.h>
#include <nanvix/hal.h>
#include <nanvix/pm.h>
#include <signal.h>

/**
 * @brief Schedules a process to execution.
 *
 * @param proc Process to be scheduled.
 */
PUBLIC void sched(struct process *proc)
{
	proc->state = PROC_READY;
	proc->counter = 0;
}

/**
 * @brief Stops the current running process.
 */
PUBLIC void stop(void)
{
	curr_proc->state = PROC_STOPPED;
	sndsig(curr_proc->father, SIGCHLD);
	yield();
}

/**
 * @brief Resumes a process.
 *
 * @param proc Process to be resumed.
 *
 * @note The process must stopped to be resumed.
 */
PUBLIC void resume(struct process *proc)
{
	/* Resume only if process has stopped. */
	if (proc->state == PROC_STOPPED)
		sched(proc);
}

/**
 * @brief Yields the processor.
 */
PUBLIC void yield(void)
{
	struct process *p;    /* Working process.     */
	struct process *next; /* Next process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;

		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}

	/* Choose a process to run next. */

	/*
	pseudo-code:

	pour p in process
		si p->nice < NICE_MAX / 2
			queue1.add(p)
		else
			queue2.add(p)
	fin pour
	pour p in queue1
		priority
	fin pour
	pour p in queue2
		FIFO
	fin pour	
	*/

	next = IDLE;
	struct process *queue1[50];
	struct process *queue2[50];

	int taille1 = 0;
	int taille2 = 0;
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip non-ready process. */
		if (p->state != PROC_READY)
			continue;

		// Put all the process with nice < NZERO in queue1
		if(p->nice < NZERO){
			queue1[taille1] = p;
			taille1++;
		}

		// Put all the process with nice > NZERO in queue2
		else if (p-> nice > NZERO){
			queue2[taille2] = p;
			taille2++;
		}
	}

	// Priority scheduling for queue1
	next = IDLE;
	for (int i = 0; i < taille1; i++)
	{
		p = queue1[i];

		if ((next == IDLE || p->nice < next->nice || (p->nice == next->nice && p->counter > next->counter)))
		{
			next = p;
		}
		else
			p->counter++;
	}
	
	// First Come First Served scheduling for queue2
	if(next == IDLE){ // No process was found in queue1
		if(taille2 != 0)
			// We take the first element of the tab (the first came)
			// Always 0 because we build the list at each yield
			next = queue2[0]; 
	}

		/* Switch to next process. */
		next->priority = PRIO_USER;
	next->state = PROC_RUNNING;
	next->counter = PROC_QUANTUM;
	if (curr_proc != next)
		switch_to(next);
}
