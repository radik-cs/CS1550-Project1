#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/stddef.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/cs1550.h>

static DEFINE_RWLOCK(sem_rwlock);
static LIST_HEAD(sem_list);
static long sem_id_counter = 0;
// iterate sem_list to get cs1550_sem whose sem_id equals given sem_id 
static struct cs1550_sem *get_sem_by_id(long sem_id) {
	struct cs1550_sem *sem = NULL;
	read_lock(&sem_rwlock);
	list_for_each_entry (sem, &sem_list, list) {
		if (sem->sem_id == sem_id) {
			break;
		}
	}
	read_unlock(&sem_rwlock);
    if (sem->sem_id == sem_id) {
	    return sem;
    }
	return NULL;
}
/**
 * Creates a new semaphore. The long integer value is used to
 * initialize the semaphore's value.
 *
 * The initial `value` must be greater than or equal to zero.
 *
 * On success, returns the identifier of the created
 * semaphore, which can be used with up() and down().
 *
 * On failure, returns -EINVAL or -ENOMEM, depending on the
 * failure condition.
 */
SYSCALL_DEFINE1(cs1550_create, long, value)
{
	struct cs1550_sem *sem;
	if (value < 0) {
		return EINVAL;
	}
    sem = kmalloc(sizeof(struct cs1550_sem), GFP_KERNEL);
    if (sem == NULL) {
	    return ENOMEM;
    }
    sem->sem_id = sem_id_counter++;
    sem->value = value;
    spin_lock_init(&sem->lock);
    INIT_LIST_HEAD(&sem->list);
    INIT_LIST_HEAD(&sem->waiting_tasks);
    write_lock(&sem_rwlock);
    list_add(&sem->list, &sem_list);
    write_unlock(&sem_rwlock);
    return sem->sem_id;
}

/**
 * Performs the down() operation on an existing semaphore
 * using the semaphore identifier obtained from a previous call
 * to cs1550_create().
 *
 * This decrements the value of the semaphore, and *may cause* the
 * calling process to sleep (if the semaphore's value goes below 0)
 * until up() is called on the semaphore by another process.
 *
 * Returns 0 when successful, or -EINVAL or -ENOMEM if an error
 * occurred.
 */
SYSCALL_DEFINE1(cs1550_down, long, sem_id)
{
    struct cs1550_sem *sem;
    struct cs1550_task *task_node;

	sem = get_sem_by_id(sem_id);
    if (sem == NULL) {
	    return EINVAL;
    }
    // if sem value is zero, put current process into sleep state by putting it into 
    // waiting list and scheduling other process to run
    if (sem->value == 0) {
	    write_lock(&sem_rwlock);
	    task_node = kmalloc(sizeof(struct cs1550_task), GFP_KERNEL);
	    INIT_LIST_HEAD(&task_node->list);
	    task_node->task = current;
	    list_add_tail(&task_node->list, &sem->waiting_tasks);
        write_unlock(&sem_rwlock);
	    set_current_state(TASK_INTERRUPTIBLE);
	    schedule(); 
    }
	spin_lock(&sem->lock);
	sem->value--;
	spin_unlock(&sem->lock);
	return 0;
}

/**
 * Performs the up() operation on an existing semaphore
 * using the semaphore identifier obtained from a previous call
 * to cs1550_create().
 *
 * This increments the value of the semaphore, and *may cause* the
 * calling process to wake up a process waiting on the semaphore,
 * if such a process exists in the queue.
 *
 * Returns 0 when successful, or -EINVAL if the semaphore ID is
 * invalid.
 */
SYSCALL_DEFINE1(cs1550_up, long, sem_id)
{
    struct cs1550_sem *sem;

	sem = get_sem_by_id(sem_id);
    if (sem == NULL) {
	    return EINVAL;
    }
	spin_lock(&sem->lock);
	sem->value++;
	spin_unlock(&sem->lock);
	write_lock(&sem_rwlock);
    // if waiting tasks is not empty, pop up a task from waiting tasks list and wake up the task.
    if (!list_empty(&sem->waiting_tasks))
    {
	    struct cs1550_task *task = list_first_entry(&sem->waiting_tasks, struct cs1550_task, list);
	    list_del(&task->list);
	    wake_up_process(task->task);
    }
	write_unlock(&sem_rwlock);
	return 0;
}

/**
 * Removes an already-created semaphore from the system-wide
 * semaphore list using the identifier obtained from a previous
 * call to cs1550_create().
 *
 * Returns 0 when successful or -EINVAL if the semaphore ID is
 * invalid or the semaphore's process queue is not empty.
 */
SYSCALL_DEFINE1(cs1550_close, long, sem_id)
{
    struct cs1550_sem *sem;
    struct cs1550_task *task;

    sem = get_sem_by_id(sem_id);
    if (sem == NULL) {
	    return EINVAL;
    }
    write_lock(&sem_rwlock);
    // iterate over waiting task list to free each waiting task
	for (task = list_first_entry(&sem->waiting_tasks, struct cs1550_task, list);
	     !list_entry_is_head(task, &sem->waiting_tasks, list);)
         {
		struct cs1550_task *next = list_next_entry(task, list);
		kfree(task);
		task = next;
	}
    // free semaphore
    list_del(&sem->list);
    kfree(sem);
    write_unlock(&sem_rwlock);
    return 0;
}
