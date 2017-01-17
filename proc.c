#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"


struct {
    struct spinlock lock;
    struct proc proc[NPROC];
} ptable;
struct spinlock mutex;
struct spinlock console;
/*
 queue implementation
 */
struct proc *runnableQ[NPROC];
int front = 0;
int rear = -1;
int itemCount = 0;

struct proc *Q_3[NPROC];
int front_3 = 0;
int rear_3 = -1;
int itemCount_3 = 0;

//Shradha changes begin
struct semaphore {
    int value;
    int active;
    struct spinlock lock;
};

struct semaphore sema[32];

struct proc *peek() {
    return runnableQ[front];
}

int isEmpty() {
    if (itemCount == 0)
        return 1;
    else
        return 0;
}

int isFull() {
    if (itemCount == NPROC)
        return 1;
    else
        return 0;
}

int size() {
    return itemCount;
}


int countP(int priority) {
    if (priority == 1)
        return itemCount_3;

    int count = 0;
    struct proc *p;


    acquire(&ptable.lock);
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->state != RUNNABLE)
            continue;

        if (p->priority == priority)
            count++;
    }
    release(&ptable.lock);

    return count;
}

void insert(struct proc *data) {

    if (isFull() == 0) {

        if (rear == NPROC - 1) {
            rear = -1;
        }

        runnableQ[++rear] = data;
        itemCount++;
    }
}

struct proc *removeData() {
    struct proc *data = runnableQ[front++];

    if (front == NPROC) {
        front = 0;
    }

    itemCount--;
    return data;
}

/*
 3queue implementation
 */



int isEmptyQ() {
    int e = 0;
    if (itemCount_3 == 0)
        e = 1;
    else
        e = 0;
    return e;

}

int isFullQ() {
    int f = 0;
    if (itemCount_3 == NPROC)
        f = 1;
    else
        f = 0;
    return f;

}

void insertQ(struct proc *data) {

    acquire(&mutex);

    if (data && data->state == RUNNABLE && data->priority == 1) {

        if (isFullQ() == 0) {

            if (rear_3 == NPROC - 1) {
                rear_3 = -1;
            }

            Q_3[++rear_3] = data;
            itemCount_3++;
        }
    }
    release(&mutex);
}

struct proc *removeDataQ() {
    acquire(&mutex);
    struct proc *data = 0;

    data = Q_3[front_3++];

    if (front_3 == NPROC) {
        front_3 = 0;
    }

    itemCount_3--;


    release(&mutex);

    return data;
}

static struct proc *initproc;

int nextpid = 1;

extern void forkret(void);

extern void trapret(void);

static void wakeup1(void *chan);

void
pinit(void) {
    initlock(&ptable.lock, "ptable");
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc *
allocproc(void) {
    struct proc *p;
    char *sp;

    acquire(&ptable.lock);

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
        if (p->state == UNUSED)
            goto found;

    release(&ptable.lock);
    return 0;
    found:
    p->state = EMBRYO;
    p->rtime = 0;
    p->priority = 2;
    p->ctime = ticks;
    p->pid = nextpid++;
    p->quantom_use = 0;

    release(&ptable.lock);

    // Allocate kernel stack.
    if ((p->kstack = kalloc()) == 0) {
        p->state = UNUSED;
        return 0;
    }
    sp = p->kstack + KSTACKSIZE;

    // Leave room for trap frame.
    sp -= sizeof *p->tf;
    p->tf = (struct trapframe *) sp;

    // Set up new context to start executing at forkret,
    // which returns to trapret.
    sp -= 4;
    *(uint *) sp = (uint) trapret;

    sp -= sizeof *p->context;
    p->context = (struct context *) sp;
    memset(p->context, 0, sizeof *p->context);
    p->context->eip = (uint) forkret;

    return p;
}


//Shradha changes begin
int
sem_init(int sem, int value) {
    acquire(&sema[sem].lock);

    if (sema[sem].active == 0) {
        sema[sem].active = 1;
        sema[sem].value = value;
    } else {
        return -1;
    }

    release(&sema[sem].lock);

    return 0;
}

int
sem_destroy(int sem) {
    acquire(&sema[sem].lock);
    sema[sem].active = 0;
    release(&sema[sem].lock);

    return 0;
}

int sem_wait(int sem, int count) {
    acquire(&sema[sem].lock);

    if (sema[sem].value >= count) {
        sema[sem].value = sema[sem].value - count;
    } else {
        while (sema[sem].value < count) {
            sleep(&sema[sem], &sema[sem].lock);
        }
        sema[sem].value = sema[sem].value - count;
    }

    release(&sema[sem].lock);

    return 0;
}

int sem_signal(int sem, int count) {
    acquire(&sema[sem].lock);

    sema[sem].value = sema[sem].value + count;
    wakeup(&sema[sem]);
    release(&sema[sem].lock);

    return 0;
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void) {
    initlock(&mutex, "salama");
    initlock(&console, "console");

    struct proc *p;
    extern char _binary_initcode_start[], _binary_initcode_size[];

    p = allocproc();

    initproc = p;
    if ((p->pgdir = setupkvm()) == 0)
        panic("userinit: out of memory?");
    inituvm(p->pgdir, _binary_initcode_start, (int) _binary_initcode_size);
    p->sz = PGSIZE;
    memset(p->tf, 0, sizeof(*p->tf));
    p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
    p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
    p->tf->es = p->tf->ds;
    p->tf->ss = p->tf->ds;
    p->tf->eflags = FL_IF;
    p->tf->esp = PGSIZE;
    p->tf->eip = 0;  // beginning of initcode.S

    safestrcpy(p->name, "initcode", sizeof(p->name));
    p->cwd = namei("/");

    // this assignment to p->state lets other cores
    // run this process. the acquire forces the above
    // writes to be visible, and the lock is also needed
    // because the assignment might not be atomic.
    acquire(&ptable.lock);

    p->state = RUNNABLE;
    if (policy == 1)
        insert(p);
    else if (policy == 3) {
        if (p->priority == 1 && p->state == RUNNABLE) {
            //cprintf("pppp : %d",p->priority);
            insertQ(p);
        }
        //TODO 3Q
    }

    release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n) {
    uint sz;

    sz = proc->sz;
    if (n > 0) {
        if ((sz = allocuvm(proc->pgdir, sz, sz + n)) == 0)
            return -1;
    } else if (n < 0) {
        if ((sz = deallocuvm(proc->pgdir, sz, sz + n)) == 0)
            return -1;
    }
    proc->sz = sz;
    switchuvm(proc);
    return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void) {
    int i, pid;
    struct proc *np;

    // Allocate process.
    if ((np = allocproc()) == 0) {
        return -1;
    }

    // Copy process state from p.
    if ((np->pgdir = copyuvm(proc->pgdir, proc->sz)) == 0) {
        kfree(np->kstack);
        np->kstack = 0;
        np->state = UNUSED;
        return -1;
    }
    np->sz = proc->sz;
    np->parent = proc;
    *np->tf = *proc->tf;

    // Clear %eax so that fork returns 0 in the child.
    np->tf->eax = 0;

    for (i = 0; i < NOFILE; i++)
        if (proc->ofile[i])
            np->ofile[i] = filedup(proc->ofile[i]);
    np->cwd = idup(proc->cwd);

    safestrcpy(np->name, proc->name, sizeof(proc->name));
    pid = np->pid;

    acquire(&ptable.lock);

    np->state = RUNNABLE;

    if (policy == 1)
        insert(np);
    else if (policy == 3) {
        if (np->priority == 1 && np->state == RUNNABLE) {
            insertQ(np);
            //cprintf("pppp :%d",np->priority);
        }
        //TODO 3Q
    }
    // insert(np);
    release(&ptable.lock);

    return pid;
}

int getpid() {
    return proc->pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void) {
    struct proc *p;
    int fd;

    if (proc == initproc)
        panic("init exiting");

    // Close all open files.
    for (fd = 0; fd < NOFILE; fd++) {
        if (proc->ofile[fd]) {
            fileclose(proc->ofile[fd]);
            proc->ofile[fd] = 0;
        }
    }

    begin_op();
    iput(proc->cwd);
    end_op();
    proc->cwd = 0;

    acquire(&ptable.lock);

    // Parent might be sleeping in wait().
    wakeup1(proc->parent);

    // Pass abandoned children to init.
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->parent == proc) {
            p->parent = initproc;
            if (p->state == ZOMBIE)
                wakeup1(initproc);
        }
    }

    // Jump into the scheduler, never to return.
    proc->state = ZOMBIE;
    proc->etime = ticks;
    sched();
    panic("zombie exit");
}

int
wait2(void) {
    struct proc *p;
    int havekids;

    acquire(&ptable.lock);
    for (;;) {
        // Scan through table looking for exited children.
        havekids = 0;
        for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
            if (p->parent != proc)
                continue;
            havekids = 1;
            if (p->state == ZOMBIE) {
                // Found one.
                char *wtime = 0, *rtime = 0;
                argptr(0, &wtime, sizeof(int));
                argptr(1, &rtime, sizeof(int));

                *wtime = (p->etime - p->ctime) - p->rtime;
                *rtime = p->rtime;
                int cid = p->cid;
                kfree(p->kstack);
                p->kstack = 0;
                freevm(p->pgdir);
                p->pid = 0;
                p->parent = 0;
                p->name[0] = 0;
                p->killed = 0;
                p->state = UNUSED;
                release(&ptable.lock);
                return cid;
            }
        }
        // No point waiting if we don't have any children.
        if (!havekids || proc->killed) {
            release(&ptable.lock);
            return -1;
        }
        // Wait for children to exit.  (See wakeup1 call in proc_exit.)
        sleep(proc, &ptable.lock);  //DOC: wait-sleep
    }
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.

int
wait(void) {
    struct proc *p;
    int havekids, pid;

    acquire(&ptable.lock);
    for (;;) {
        // Scan through table looking for exited children.
        havekids = 0;
        for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
            if (p->parent != proc)
                continue;
            havekids = 1;
            if (p->state == ZOMBIE) {
                // Found one.
                pid = p->pid;
                kfree(p->kstack);
                p->kstack = 0;
                freevm(p->pgdir);
                p->pid = 0;
                p->parent = 0;
                p->name[0] = 0;
                p->killed = 0;
                p->state = UNUSED;
                release(&ptable.lock);
                return pid;
            }
        }

        // No point waiting if we don't have any children.
        if (!havekids || proc->killed) {
            release(&ptable.lock);
            return -1;
        }

        // Wait for children to exit.  (See wakeup1 call in proc_exit.)
        sleep(proc, &ptable.lock);  //DOC: wait-sleep
    }
}

int getMinIndexIn2Priority(void) {
    struct proc *p;
    double minScore = 9999999.999999; //set to max value
    int indexOfProcess = -1;
    int minIndex = 999999; //set to max value
    double s;
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        indexOfProcess++;
        if (p->state != RUNNABLE || p->priority != 2) {
            continue;
        }

        s = 9999999.999999; //set to max value
        if (ticks - p->ctime != 0)
            s = (double) p->rtime / (double) ((double) ticks - (double) p->ctime);
        if (minScore > s) {
            minScore = s;
            minIndex = indexOfProcess;
        }
        minIndex = indexOfProcess;
    }

    return minIndex;
}

/**
 * Get index of high priority proccess
 * @return index of high priority proccess in proccess table
 */
int getMinIndex(void) {
    struct proc *p;
    double maxScore = -1; //set to min value
    int indexOfProcess = -1;
    int maxIndex = 0;
    long s;
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        indexOfProcess++;
        if (p->state != RUNNABLE) {
            continue;
        } else {
            s = 99999999; //set to max value
            if (p->rtime != 0)
                s = (ticks - p->ctime) / p->rtime;
            //cprintf("s is : %d\n" , s);
            if (maxScore < s) {
                maxScore = s;
                maxIndex = indexOfProcess;
            }
        }
    }

    return maxIndex;
}

/**
 * add runnable processes to runnableQ
 *//*
void addToRunnableQ() {
    struct proc *p;
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->state == RUNNABLE) {
            if (policy == 1)
                insert(p);
            else if (policy == 3) {
                insertQ(p, p->priority);
                //TODO 3Q
            }
        }
    }
}*/

void printQ() {
    cprintf("content of Q : < ");
    for (int i = front; i <= rear; i++) {
        if (i < rear) {
            cprintf("%d , ", runnableQ[i]->pid);
        } else {
            cprintf("%d >", runnableQ[i]->pid);
        }

    }

    cprintf("\n");
}


//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler(void) {
    for (;;) {
        // Enable interrupts on this processor.
        sti();
        // Loop over process table looking for process to run.
        if (policy == 1) { //for FRR policy
            struct proc *p;

            acquire(&ptable.lock);
            // addToRunnableQ();
            if (isEmpty() == 0) {
                acquire(&console);
                printQ();
                release(&console);
                p = removeData();
                proc = p;
                switchuvm(p);
                p->state = RUNNING;
                swtch(&cpu->scheduler, p->context);
                switchkvm();
                proc = 0;
            }
            release(&ptable.lock);

        } else if (policy == 2) { //for GRT policy
            struct proc *p;
            int indexOfProcess;
            int minIndex;

            acquire(&ptable.lock);
            minIndex = getMinIndex();
            indexOfProcess = -1;
            for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
                indexOfProcess++;
                if (p->state != RUNNABLE || indexOfProcess != minIndex)
                    continue;
                else {
                    // minP = &ptable.proc[minIndex];
                    // Switch to chosen process.  It is the process's job
                    // to release ptable.lock and then reacquire it
                    // before jumping back to us.
                    proc = p;
                    switchuvm(p);
                    p->state = RUNNING;
                    swtch(&cpu->scheduler, p->context);
                    switchkvm();
                    // Process is done running for now.
                    // It should have changed its p->state before coming back.
                    proc = 0;
                    minIndex = getMinIndex();
                    indexOfProcess = -1;
                    p = ptable.proc;
                }
            }
            release(&ptable.lock);
        } else if (policy == 3) { //  for 3Q policy
            struct proc *p;
//            acquire(&ptable.lock);
            if (countP(2) != 0) {
                struct proc *p;
                int indexOfProcess;
                int minIndex;

                acquire(&ptable.lock);
                minIndex = getMinIndexIn2Priority();
                indexOfProcess = -1;
                for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
                    indexOfProcess++;
                    if (p->state != RUNNABLE || indexOfProcess != minIndex)
                        continue;
                    else {
                        // minP = &ptable.proc[minIndex];
                        // Switch to chosen process.  It is the process's job
                        // to release ptable.lock and then reacquire it
                        // before jumping back to us.
                        proc = p;
                        switchuvm(p);
                        p->state = RUNNING;
                        swtch(&cpu->scheduler, p->context);
                        switchkvm();
                        // Process is done running for now.
                        // It should have changed its p->state before coming back.
                        proc = 0;
                        minIndex = getMinIndexIn2Priority();
                        indexOfProcess = -1;
                        p = ptable.proc;
                    }
                }
                release(&ptable.lock);
            } else {
                if (countP(1) != 0) {
                    acquire(&ptable.lock);
                    if (isEmptyQ() == 0) {
                        p = removeDataQ();
                        proc = p;
                        switchuvm(p);
                        p->state = RUNNING;
                        p->quantom_use = 0;
                        swtch(&cpu->scheduler, p->context);
                        switchkvm();
                        proc = 0;
                    }
                    release(&ptable.lock);
                } else {
                    if (countP(0) != 0) {
                        struct proc *p;
                        acquire(&ptable.lock);
                        for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
                            if (p->state != RUNNABLE || p->priority != 0)
                                continue;

                            // Switch to chosen process.  It is the process's job
                            // to release ptable.lock and then reacquire it
                            // before jumping back to us.
                            proc = p;
                            switchuvm(p);
                            p->state = RUNNING;
                            swtch(&cpu->scheduler, p->context);
                            switchkvm();

                            // Process is done running for now.
                            // It should have changed its p->state before coming back.
                            proc = 0;
                        }
                        release(&ptable.lock);
                    }
                }
            }
//            release(&ptable.lock);
        } else {
            struct proc *p;
            acquire(&ptable.lock);
            for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
                if (p->state != RUNNABLE)
                    continue;

                // Switch to chosen process.  It is the process's job
                // to release ptable.lock and then reacquire it
                // before jumping back to us.
                proc = p;
                switchuvm(p);
                p->state = RUNNING;
                swtch(&cpu->scheduler, p->context);
                switchkvm();

                // Process is done running for now.
                // It should have changed its p->state before coming back.
                proc = 0;
            }
            release(&ptable.lock);
        }
    }
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void) {
    int intena;

    if (!holding(&ptable.lock))
        panic("sched ptable.lock");
    if (cpu->ncli != 1)
        panic("sched locks");
    if (proc->state == RUNNING)
        panic("sched running");
    if (readeflags() & FL_IF)
        panic("sched interruptible");
    intena = cpu->intena;
    swtch(&proc->context, cpu->scheduler);
    cpu->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void) {
    acquire(&ptable.lock);  //DOC: yieldlock
    proc->state = RUNNABLE;
    if (policy == 1)
        insert(proc);
    else if (policy == 3) {
        if (proc->priority == 1 && proc->state == RUNNABLE) {
            insertQ(proc);
            //cprintf("pppp :%d",proc->priority);
        }
        //TODO 3Q
    }
    sched();
    release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void) {
    static int first = 1;
    // Still holding ptable.lock from scheduler.
    release(&ptable.lock);

    if (first) {
        // Some initialization functions must be run in the context
        // of a regular process (e.g., they call sleep), and thus cannot
        // be run from main().
        first = 0;
        iinit(ROOTDEV);
        initlog(ROOTDEV);
    }

    // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk) {
    if (proc == 0)
        panic("sleep");

    if (lk == 0)
        panic("sleep without lk");

    // Must acquire ptable.lock in order to
    // change p->state and then call sched.
    // Once we hold ptable.lock, we can be
    // guaranteed that we won't miss any wakeup
    // (wakeup runs with ptable.lock locked),
    // so it's okay to release lk.
    if (lk != &ptable.lock) {  //DOC: sleeplock0
        acquire(&ptable.lock);  //DOC: sleeplock1
        release(lk);
    }

    // Go to sleep.
    proc->chan = chan;
    proc->state = SLEEPING;
    sched();

    // Tidy up.
    proc->chan = 0;

    // Reacquire original lock.
    if (lk != &ptable.lock) {  //DOC: sleeplock2
        release(&ptable.lock);
        acquire(lk);
    }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan) {
    struct proc *p;

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
        if (p->state == SLEEPING && p->chan == chan) {
            p->state = RUNNABLE;
            if (policy == 1)
                insert(p);
            else if (policy == 3) {
                if (p->priority == 1 && p->state == RUNNABLE) {
                    // cprintf("pppp :%d",p->priority);
                    insertQ(p);
                }
                //TODO 3Q
            }
        }
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan) {
    acquire(&ptable.lock);
    wakeup1(chan);
    release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid) {
    struct proc *p;

    acquire(&ptable.lock);
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->pid == pid) {
            p->killed = 1;
            // Wake process from sleep if necessary.
            if (p->state == SLEEPING) {
                p->state = RUNNABLE;
                if (policy == 1)
                    insert(p);
                else if (policy == 3) {
                    if (p->priority == 1 && p->state == RUNNABLE) {
                        //cprintf("pppp :%d",p->priority);
                        insertQ(p);
                    }
                    //TODO 3Q
                }
            }
            release(&ptable.lock);
            return 0;
        }
    }
    release(&ptable.lock);
    return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void) {
    static char *states[] = {
            [UNUSED]    "unused",
            [EMBRYO]    "embryo",
            [SLEEPING]  "sleep ",
            [RUNNABLE]  "runble",
            [RUNNING]   "run   ",
            [ZOMBIE]    "zombie"
    };
    int i;
    struct proc *p;
    char *state;
    uint pc[10];

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->state == UNUSED)
            continue;
        if (p->state >= 0 && p->state < NELEM(states) && states[p->state])
            state = states[p->state];
        else
            state = "???";
        cprintf("%d %s %s", p->pid, state, p->name);
        if (p->state == SLEEPING) {
            getcallerpcs((uint *) p->context->ebp + 2, pc);
            for (i = 0; i < 10 && pc[i] != 0; i++)
                cprintf(" %p", pc[i]);
        }
        cprintf("\n");
    }
}

int
nice() {

    /*   //cprintf("in nice syscall , priority is : %d\n" , proc->priority);
       if (proc && proc->priority > 0) {
           (proc->priority)--;
           return 0;
       } else
           return -1;*/
    if (proc->priority == 2) {
        proc->priority = 1;

    } else if (proc->priority == 1) {
        proc->priority = 0;
        int i;
        for (i = 0; i < itemCount_3; i++) {
            struct proc *proc_temp = removeDataQ();
            int pid_temp = proc_temp->pid;
            if (proc->pid == pid_temp) {
                continue;
            }
            insertQ(proc_temp);
        }
    } else if (proc->priority == 0) {
        proc->priority = 0;
    }

    return proc->priority;
}
