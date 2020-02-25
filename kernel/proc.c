/******************************************************************************/
/* Important Spring 2019 CSCI 402 usage information:                          */
/*                                                                            */
/* This fils is part of CSCI 402 kernel programming assignments at USC.       */
/*         53616c7465645f5fd1e93dbf35cbffa3aef28f8c01d8cf2ffc51ef62b26a       */
/*         f9bda5a68e5ed8c972b17bab0f42e24b19daa7bd408305b1f7bd6c7208c1       */
/*         0e36230e913039b3046dd5fd0ba706a624d33dbaa4d6aab02c82fe09f561       */
/*         01b0fd977b0051f0b0ce0c69f7db857b1b5e007be2db6d42894bf93de848       */
/*         806d9152bd5715e9                                                   */
/* Please understand that you are NOT permitted to distribute or publically   */
/*         display a copy of this file (or ANY PART of it) for any reason.    */
/* If anyone (including your prospective employer) asks you to post the code, */
/*         you must inform them that you do NOT have permissions to do so.    */
/* You are also NOT permitted to remove or alter this comment block.          */
/* If this comment block is removed or altered in a submitted file, 20 points */
/*         will be deducted.                                                  */
/******************************************************************************/

#include "kernel.h"
#include "config.h"
#include "globals.h"
#include "errno.h"

#include "util/debug.h"
#include "util/list.h"
#include "util/string.h"
#include "util/printf.h"

#include "proc/kthread.h"
#include "proc/proc.h"
#include "proc/sched.h"
#include "proc/proc.h"

#include "mm/slab.h"
#include "mm/page.h"
#include "mm/mmobj.h"
#include "mm/mm.h"
#include "mm/mman.h"

#include "vm/vmmap.h"

#include "fs/vfs.h"
#include "fs/vfs_syscall.h"
#include "fs/vnode.h"
#include "fs/file.h"

proc_t *curproc = NULL; /* global */
static slab_allocator_t *proc_allocator = NULL;

static list_t _proc_list;
static proc_t *proc_initproc = NULL; /* Pointer to the init process (PID 1) */

void
proc_init()
{
        list_init(&_proc_list);
        proc_allocator = slab_allocator_create("proc", sizeof(proc_t));
        KASSERT(proc_allocator != NULL);
}

proc_t *
proc_lookup(int pid)
{
        proc_t *p;
        list_iterate_begin(&_proc_list, p, proc_t, p_list_link) {
                if (p->p_pid == pid) {
                        return p;
                }
        } list_iterate_end();
        return NULL;
}

list_t *
proc_list()
{
        return &_proc_list;
}

size_t
proc_info(const void *arg, char *buf, size_t osize)
{
        const proc_t *p = (proc_t *) arg;
        size_t size = osize;
        proc_t *child;

        KASSERT(NULL != p);
        KASSERT(NULL != buf);

        iprintf(&buf, &size, "pid:          %i\n", p->p_pid);
        iprintf(&buf, &size, "name:         %s\n", p->p_comm);
        if (NULL != p->p_pproc) {
                iprintf(&buf, &size, "parent:       %i (%s)\n",
                        p->p_pproc->p_pid, p->p_pproc->p_comm);
        } else {
                iprintf(&buf, &size, "parent:       -\n");
        }

#ifdef __MTP__
        int count = 0;
        kthread_t *kthr;
        list_iterate_begin(&p->p_threads, kthr, kthread_t, kt_plink) {
                ++count;
        } list_iterate_end();
        iprintf(&buf, &size, "thread count: %i\n", count);
#endif

        if (list_empty(&p->p_children)) {
                iprintf(&buf, &size, "children:     -\n");
        } else {
                iprintf(&buf, &size, "children:\n");
        }
        list_iterate_begin(&p->p_children, child, proc_t, p_child_link) {
                iprintf(&buf, &size, "     %i (%s)\n", child->p_pid, child->p_comm);
        } list_iterate_end();

        iprintf(&buf, &size, "status:       %i\n", p->p_status);
        iprintf(&buf, &size, "state:        %i\n", p->p_state);

#ifdef __VFS__
#ifdef __GETCWD__
        if (NULL != p->p_cwd) {
                char cwd[256];
                lookup_dirpath(p->p_cwd, cwd, sizeof(cwd));
                iprintf(&buf, &size, "cwd:          %-s\n", cwd);
        } else {
                iprintf(&buf, &size, "cwd:          -\n");
        }
#endif /* __GETCWD__ */
#endif

#ifdef __VM__
        iprintf(&buf, &size, "start brk:    0x%p\n", p->p_start_brk);
        iprintf(&buf, &size, "brk:          0x%p\n", p->p_brk);
#endif

        return size;
}

size_t
proc_list_info(const void *arg, char *buf, size_t osize)
{
        size_t size = osize;
        proc_t *p;

        KASSERT(NULL == arg);
        KASSERT(NULL != buf);

#if defined(__VFS__) && defined(__GETCWD__)
        iprintf(&buf, &size, "%5s %-13s %-18s %-s\n", "PID", "NAME", "PARENT", "CWD");
#else
        iprintf(&buf, &size, "%5s %-13s %-s\n", "PID", "NAME", "PARENT");
#endif

        list_iterate_begin(&_proc_list, p, proc_t, p_list_link) {
                char parent[64];
                if (NULL != p->p_pproc) {
                        snprintf(parent, sizeof(parent),
                                 "%3i (%s)", p->p_pproc->p_pid, p->p_pproc->p_comm);
                } else {
                        snprintf(parent, sizeof(parent), "  -");
                }

#if defined(__VFS__) && defined(__GETCWD__)
                if (NULL != p->p_cwd) {
                        char cwd[256];
                        lookup_dirpath(p->p_cwd, cwd, sizeof(cwd));
                        iprintf(&buf, &size, " %3i  %-13s %-18s %-s\n",
                                p->p_pid, p->p_comm, parent, cwd);
                } else {
                        iprintf(&buf, &size, " %3i  %-13s %-18s -\n",
                                p->p_pid, p->p_comm, parent);
                }
#else
                iprintf(&buf, &size, " %3i  %-13s %-s\n",
                        p->p_pid, p->p_comm, parent);
#endif
        } list_iterate_end();
        return size;
}

static pid_t next_pid = 0;

/**
 * Returns the next available PID.
 *
 * Note: Where n is the number of running processes, this algorithm is
 * worst case O(n^2). As long as PIDs never wrap around it is O(n).
 *
 * @return the next available PID
 */
static int
_proc_getid()
{
        proc_t *p;
        pid_t pid = next_pid;
        while (1) {
failed:
                list_iterate_begin(&_proc_list, p, proc_t, p_list_link) {
                        if (p->p_pid == pid) {
                                if ((pid = (pid + 1) % PROC_MAX_COUNT) == next_pid) {
                                        return -1;
                                } else {
                                        goto failed;
                                }
                        }
                } list_iterate_end();
                next_pid = (pid + 1) % PROC_MAX_COUNT;
                return pid;
        }
}

/*
 * The new process, although it isn't really running since it has no
 * threads, should be in the PROC_RUNNING state.
 *
 * Don't forget to set proc_initproc when you create the init
 * process. You will need to be able to reference the init process
 * when reparenting processes to the init process.
 */
proc_t *
proc_create(char *name)
{
	proc_t *process;
	process = (proc_t*) slab_obj_alloc(proc_allocator);
	process->p_pagedir = pt_create_pagedir();
	//memset(process, 0, sizeof(proc_t));
	// KASSERT(process != NULL);
		
	//initialisng the process feilds
	process->p_pid = _proc_getid();
	//kassert according guideline
	KASSERT(PID_IDLE != process->p_pid || list_empty(&_proc_list)); /* pid can only be PID_IDLE if this is the first process */
	dbg(DBG_PRINT, "(GRADING1A 2.a)\n");
    	KASSERT(PID_INIT != process->p_pid || PID_IDLE == curproc->p_pid); /* pid can only be PID_INIT if the running process is the "idle" process */
    	dbg(DBG_PRINT, "(GRADING1A 2.a)\n");
	strncpy(process->p_comm, name, PROC_NAME_LEN);
	list_init(&process->p_threads);
	list_init(&process->p_children);
	if(process->p_pid == PID_IDLE) {
		curproc = process;
		process->p_pproc = NULL;
		dbg(DBG_PRINT, "(GRADING1A)\n");
	}
	else {
		process->p_pproc = curproc;
		dbg(DBG_PRINT, "(GRADING1A)\n");
	}
	process->p_status = 0;
	process->p_state = PROC_RUNNING;
	sched_queue_init(&process->p_wait);
	list_insert_tail(&_proc_list, &process->p_list_link);
	//list_link_init(&process->p_list_link);
	//list_link_init(&process->p_child_link);
	if(process->p_pproc != NULL) {
		list_insert_tail(&process->p_pproc->p_children, &process->p_child_link);
		dbg(DBG_PRINT, "(GRADING1A)\n");
	} 
	//TODO initiase kernel 3 variables
	int i = 0;
	for (i=0; i<NFILES; i++){
		process->p_files[i] = NULL;
		dbg(DBG_PRINT, "(GRADING2B)\n");
	}
	process->p_cwd = vfs_root_vn;
	if(process->p_cwd != NULL){
		vref(process->p_cwd);
		dbg(DBG_PRINT, "(GRADING2B)\n");
	}
	if (process->p_pid == PID_INIT) {
		proc_initproc = process;
		dbg(DBG_PRINT, "(GRADING1A)\n");
	}
		dbg(DBG_PRINT, "(GRADING1A)\n");
        return process;
}

/**
 * Cleans up as much as the process as can be done from within the
 * process. This involves:
 *    - Closing all open files (VFS)
 *    - Cleaning up VM mappings (VM)
 *    - Waking up its parent if it is waiting
 *    - Reparenting any children to the init process
 *    - Setting its status and state appropriately
 *
 * The parent will finish destroying the process within do_waitpid (make
 * sure you understand why it cannot be done here). Until the parent
 * finishes destroying it, the process is informally called a 'zombie'
 * process.
 *
 * This is also where any children of the current process should be
 * reparented to the init process (unless, of course, the current
 * process is the init process. However, the init process should not
 * have any children at the time it exits).
 *
 * Note: You do _NOT_ have to special case the idle process. It should
 * never exit this way.
 *
 * @param status the status to exit the process with
 */

//helper function to reparent the child process

void reparent_to_init_process(proc_t *process) {

	proc_t *child;
	list_iterate_begin(&process->p_children, child, proc_t, p_child_link) {
		child->p_pproc = proc_initproc;
		list_remove(&child->p_child_link);
		list_insert_tail(&proc_initproc->p_children, &child->p_child_link);
		
	}
	list_iterate_end();
	dbg(DBG_PRINT, "(GRADING1C 6)\n");
	
}
void
proc_cleanup(int status)
{

	//TODO, closing all the vfs files and VPM are done in kernel 2
	//grading guideline
	 KASSERT(NULL != proc_initproc); /* "init" process must exist and proc_initproc initialized */
	dbg(DBG_PRINT, "(GRADING1A 2.b)\n");
         KASSERT(1 <= curproc->p_pid); /* this process must not be "idle" process */
		dbg(DBG_PRINT, "(GRADING1A 2.b)\n");
         KASSERT(NULL != curproc->p_pproc); /* this process must have a parent when this function is entered */
		dbg(DBG_PRINT, "(GRADING1A 2.b)\n");


	//wake up the parent if the parent is sleeping
	if (!list_empty(&curproc->p_pproc->p_wait.tq_list)) {
		sched_wakeup_on(&curproc->p_pproc->p_wait);
		dbg(DBG_PRINT, "(GRADING1A)\n");
	}

	//reparent it's children to init process
	
	if (!list_empty(&curproc->p_children) && curproc->p_pid != 1) {
		reparent_to_init_process(curproc);
		dbg(DBG_PRINT, "(GRADING1C 6)\n");
	}

	//setting its status and state appropriately
	curproc->p_state = PROC_DEAD;
	curproc->p_status = status;
	//destroy the pagetable
	int i = 0;
	vput(curproc->p_cwd);
	for(i = 0; i< NFILES; i++){
		if(curproc->p_files[i] !=NULL && curproc->p_files[i]->f_refcount > 0){
			do_close(i);
			dbg(DBG_PRINT, "(GRADING2B)\n");
		}
		dbg(DBG_PRINT, "(GRADING2B)\n");
	}
	KASSERT(NULL != curproc->p_pproc); /* this process must still have a parent when this function returns */
	dbg(DBG_PRINT, "(GRADING1A 2.b)\n");
	list_remove(&curproc->p_list_link);


	//TODO
        KASSERT(KT_EXITED == curthr->kt_state); /* the thread in this process should be in the KT_EXITED state when this function returns */
	dbg(DBG_PRINT, "(GRADING1A 2.b)\n");
       dbg(DBG_PRINT, "(GRADING1A)\n");

	sched_switch();
	
}

/*
 * This has nothing to do with signals and kill(1).
 *
 * Calling this on the current process is equivalent to calling
 * do_exit().
 *
 * In Weenix, this is only called from proc_kill_all.
 */
void
proc_kill(proc_t *p, int status)
{
        
	// Calling this on the current process is equivalent to calling
	if (p == curproc) {
		dbg(DBG_PRINT, "(GRADING1E 1)\n");
		do_exit(status);
		
	}
	else {
	//when the process dies, all the threads are cancelled, children are reparented and status is set PROC_DEAD
		kthread_t *proc_thread;
		list_iterate_begin(&p->p_threads, proc_thread, kthread_t, kt_plink) {
			if(proc_thread->kt_state != KT_EXITED) {
				kthread_cancel(proc_thread, 0);
				dbg(DBG_PRINT, "(GRADING1C 8)\n");
			}
		  	//kthread_destroy(proc_thread, 0);	
			dbg(DBG_PRINT, "(GRADING1C 8)\n");
		}

		list_iterate_end();
		
		//p->p_state = PROC_DEAD;
		p->p_status = status;
		//reparent it's children to init process
	//	if (!list_empty(&p->p_children)) {
	//		reparent_to_init_process(p);
	//	}

	//		
	//	//wake up the parent if the parent is sleeping
	//	if (!list_empty(&p->p_pproc->p_wait.tq_list)) {
	//		sched_wakeup_on(&p->p_pproc->p_wait);
	//	}

	//	p->p_state = PROC_DEAD;
	//	p->p_status = status;
	//	list_remove(&p->p_list_link);
	//	list_remove(&p->p_child_link);
	//	pt_destroy_pagedir(p->p_pagedir);
	//	slab_obj_free(proc_allocator, p);
	}
		dbg(DBG_PRINT, "(GRADING1C 8)\n");
}

/*
 * Remember, proc_kill on the current process will _NOT_ return.
 * Don't kill direct children of the idle process.
 *
 * In Weenix, this is only called by sys_halt.
 */
void
proc_kill_all()
{
	proc_t *process;
	list_iterate_begin(&_proc_list, process, proc_t, p_list_link) {

		if(process->p_pid != PID_IDLE && process->p_pid != PID_INIT && process->p_pproc->p_pid != PID_IDLE) {
			dbg(DBG_PRINT, "(GRADING1C 9)\n");
			proc_kill(process, 0);
			
		}
	dbg(DBG_PRINT, "(GRADING1C 9)\n");
	} list_iterate_end();
	
}

/*
 * This function is only called from kthread_exit.
 *
 * Unless you are implementing MTP, this just means that the process
 * needs to be cleaned up and a new thread needs to be scheduled to
 * run. If you are implementing MTP, a single thread exiting does not
 * necessarily mean that the process should be exited.
 */
void
proc_thread_exited(void *retval)
{	dbg(DBG_PRINT, "(GRADING1A)\n");
	proc_cleanup(0);
	
}

/* If pid is -1 dispose of one of the exited children of the current
 * process and return its exit status in the status argument, or if
 * all children of this process are still running, then this function
 * blocks on its own p_wait queue until one exits.
 *
 * If pid is greater than 0 and the given pid is a child of the
 * current process then wait for the given pid to exit and dispose
 * of it.
 *
 * If the current process has no children, or the given pid is not
 * a child of the current process return -ECHILD.
 *
 * Pids other than -1 and positive numbers are not supported.
 * Options other than 0 are not supported.
 */
pid_t
do_waitpid(pid_t pid, int options, int *status)
{
	KASSERT(pid == -1 || pid > 0);
	KASSERT(options == 0);

	if (list_empty(&curproc->p_children)) {
		dbg(DBG_PRINT, "(GRADING1C 2)\n");
		return -ECHILD;
		
	}
	if (pid == -1) {
		do {
			proc_t *children;
			kthread_t *proc_thread;
			list_iterate_begin(&curproc->p_children, children, proc_t, p_child_link) {
				if(children->p_state == PROC_DEAD) {
					*status = children->p_status;
					KASSERT(NULL != children);    /* must have found a dead child process */
					dbg(DBG_PRINT, "(GRADING1A 2.c)\n");
					KASSERT(-1 == pid || children->p_pid == pid);    /* if the pid argument is not -1, then pid must be the process ID of the found dead child process */
					dbg(DBG_PRINT, "(GRADING1A 2.c)\n");
					KASSERT(NULL != children->p_pagedir);     /* this process should have a valid pagedir before you destroy it */
					dbg(DBG_PRINT, "(GRADING1A 2.c)\n");
					//list_iterate_begin(&curproc->p_threads, proc_thread, kthread_t, kt_plink) {
					//	kthread_destroy(proc_thread);
					//}
					//list_iterate_end();
					list_remove(&children->p_child_link);
					pt_destroy_pagedir(children->p_pagedir);
					slab_obj_free(proc_allocator, children);
					dbg(DBG_PRINT, "(GRADING1A)\n");	
					return children->p_pid;
				}
				dbg(DBG_PRINT, "(GRADING1A)\n");
			} list_iterate_end();
			sched_sleep_on(&curproc->p_wait);
			dbg(DBG_PRINT, "(GRADING1A)\n");	
		   } while(1);
		   
	}
	else if (pid > 0) {
			proc_t *children;	
			kthread_t *proc_thread;
			list_iterate_begin(&curproc->p_children, children, proc_t, p_child_link) {
				if(pid == children->p_pid) {
					// if(children->p_state == PROC_DEAD) {
					// 	*status = children->p_status;
					// 	KASSERT(NULL != children);
					// 	dbg(DBG_PRINT, "(GRADING1A 2.c)\n");
					// 	KASSERT(-1 == pid || children->p_pid == pid);
					// 	dbg(DBG_PRINT, "(GRADING1A 2.c)\n");
					// 	KASSERT(NULL != children->p_pagedir);
					// 	dbg(DBG_PRINT, "(GRADING1A 2.c)\n");
					// 	list_remove(&children->p_child_link);
					// 	pt_destroy_pagedir(children->p_pagedir);
					// 	slab_obj_free(proc_allocator, children);
					// 	return  children->p_pid;g
					// }
					// else {
						while(1) {
							if (children->p_state == PROC_DEAD && pid == children->p_pid) {
								*status = children->p_status;
								KASSERT(NULL != children);
								dbg(DBG_PRINT, "(GRADING1A 2.c)\n");
								KASSERT(-1 == pid || children->p_pid == pid);
								dbg(DBG_PRINT, "(GRADING1A 2.c)\n");
								KASSERT(NULL != children->p_pagedir);
								dbg(DBG_PRINT, "(GRADING1A 2.c)\n");
								//list_iterate_begin(&curproc->p_threads, proc_thread, kthread_t, kt_plink) {
								//	kthread_destroy(proc_thread);
								//}
								//list_iterate_end();
								list_remove(&children->p_child_link);
								pt_destroy_pagedir(children->p_pagedir);
								slab_obj_free(proc_allocator, children);
								dbg(DBG_PRINT, "(GRADING1C 1)\n");	
								return  children->p_pid;
							}
						sched_sleep_on(&curproc->p_wait);
						dbg(DBG_PRINT, "(GRADING1C 6)\n");
						}
						
					// }
					}
			dbg(DBG_PRINT, "(GRADING1C 1)\n");
			} list_iterate_end();
			dbg(DBG_PRINT, "(GRADING1C 1)\n");
		return -ECHILD;

	}

	//can never reach to this place because of KASSERT on the beginning of the function
	return -ECHILD;
}

/*
 * Cancel all threads and join with them (if supporting MTP), and exit from the current
 * thread.
 *
 * @param status the exit status of the process
 */
void
do_exit(int status)
{
	curproc->p_status = status;
	kthread_t *thread = NULL;
	dbg(DBG_TEMP, "kthread initialised(dbg check) %p\n", thread);
	list_iterate_begin(&curproc->p_threads, thread, kthread_t, kt_plink) {
		dbg(DBG_PRINT, "(GRADING1C 1)\n");
		kthread_exit(0);
	}list_iterate_end();
		
}
