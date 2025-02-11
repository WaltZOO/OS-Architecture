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

/* Pseudo code lottery */
	/*
	nb_ticket = 0;
	ticket_tab = malloc;

	for	p in process
		si p.nice == 0
			nice = un nb entre 0 et 100
		for i de nb_ticket a (nb_ticket + nice) :
			ticket_tab[i] = p.PID;
		end for
		nb_ticket += p.nice;
	end for

	winner = rand de 0 a nb_ticket

	for 
		if p.PID == ticket_tab[winner]
			next = p;
	end for

	free ticket tab

	switch process
	*/

/**
 * @brief Yields the processor.
 */
PUBLIC void yield(void)
{
	struct process *p;
	struct process *next = NULL; /* Next process to run */

	/* Allow current process to use its quantum before switching */
	if (curr_proc->state == PROC_RUNNING && curr_proc->counter > 0)
	{
		curr_proc->counter--;
		return; // Continue running the current process
	}

	/* Re-schedule process if it used its time */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	last_proc = curr_proc;

	/* Check alarm timers */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		if (!IS_VALID(p))
			continue;

		if ((p->alarm) && (p->alarm < ticks))
		{
			p->alarm = 0;
			sndsig(p, SIGALRM);
		}
	}

	/* Lottery Scheduling */
	int nb_ticket = 0;
	int ticket_tab[100]; 

	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		if (p->state != PROC_READY)
			continue;

		// Assign tickets if not assigned
		if (p->nice > 20 && nb_ticket < 100)
		{
			p->nice = ticks % 21;
		}
		else if (nb_ticket >= 100)
		{
			p->nice = 0;
		}

		for (int i = nb_ticket; i < (nb_ticket + p->nice); i++)
		{
			ticket_tab[i] = p->pid;
		}

		nb_ticket += p->nice;
	}

	/* Select a process only if tickets exist */
	next = FIRST_PROC;
	if (nb_ticket > 0)
	{
		int winner = ticks % nb_ticket;

		for (p = FIRST_PROC; p <= LAST_PROC; p++)
		{
			if (p->pid == ticket_tab[winner] && p->state == PROC_READY)
			{
				next = p;
				break;
			}
		}
	}

	/* Switch to next process */
	next->priority = PRIO_USER;
	next->state = PROC_RUNNING;
	next->counter = PROC_QUANTUM; // Assign full quantum

	if (curr_proc != next)
		switch_to(next);
}
