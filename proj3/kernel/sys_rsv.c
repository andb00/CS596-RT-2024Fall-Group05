

#include "linux/syscalls.h"
#include "rsv.h"

// red black tree for logn look up and insertion
// slower than an hashtable but easier to implement
//

void set_priority(struct task_struct *p, int priority) {
  struct sched_param param;
  param.sched_priority = priority;

  sched_setscheduler(p, SCHED_FIFO, &param);
  printk(KERN_INFO "set %d 's priority to %d\n", p->pid, priority);
}
SYSCALL_DEFINE3(set_rsv, pid_t, pid, struct timespec *, C, struct timespec *,T) {
  struct rsv *ptr = NULL;
  struct task_struct *task = NULL;
  struct timespec *kC = NULL, *kT = NULL;
  ktime_t time;
  struct timer_data *timer_data = NULL;
  // C is execution time
  // T is period/deadline
  printk(KERN_INFO "set_rsv called\n");

  if (C == NULL || T == NULL) {
    return -1;
  }
  if (pid < 0) {
    return -1;
  }
  if (pid == 0) {
    // current is TCB of the calling process
    pid = current->pid;
  }
  // reallocate and copy timespecs since can be deallocated by user
  kC = (struct timespec *)kmalloc(sizeof(struct timespec), GFP_KERNEL);
  kT = (struct timespec *)kmalloc(sizeof(struct timespec), GFP_KERNEL);

  if (kC == NULL || kT == NULL) {
    // memory leak if 1 is allocated
    printk(KERN_INFO "kalloc for timespecs failed\n");
    return -1;
  }
  kC->tv_sec = C->tv_sec;
  kC->tv_nsec = C->tv_nsec;
  kT->tv_sec = T->tv_sec;
  kT->tv_nsec = T->tv_nsec;

  ptr = (struct rsv *)kzalloc(sizeof(struct rsv), GFP_KERNEL);
  if (ptr == NULL) {
    kfree(kC);
    kfree(kT);
    printk(KERN_INFO "kalloc failed\n");
    return -1;
  }
  ptr->pid = pid;
  ptr->C = kC;
  ptr->T = kT;

  int r = rsv_insert(get_rb_root(), ptr);

  if (r != 0) {
    kfree(kC);
    kfree(kT);
    kfree(ptr);
    printk(KERN_INFO "insert failed, pid already exists\n");
    return -1;
  }
  task = task_from_pid(pid);

  if (task == NULL) {
    kfree(kC);
    kfree(kT);
    kfree(ptr);
    printk(KERN_INFO "Task with pid %d not found\n", pid);
    return -1;
  }
  printk("inserting pid %d into priority list\n", pid);
  struct priority_node *pri_node = plist_insert(get_plist(), kT, pid);
  if (pri_node == NULL) {
    kfree(kC);
    kfree(kT);
    kfree(ptr);
    printk(KERN_INFO "Couldn't insert into priority list\n");
    return -1;
  }
  set_priority(task, pri_node->priority);

  start_task_time_accu(task);

  time = timespec_to_ktime(*kT);

  timer_data = start_timer(pid, time);
  if (timer_data == NULL) {
    kfree(kC);
    kfree(kT);
    kfree(ptr);
    return -1;
  }
  ptr->t_data = timer_data;
  // send_sig(SIGSTOP, task, 1);
  return 0;
}

SYSCALL_DEFINE1(cancel_rsv, pid_t, pid) {
  int rv = 0;
  struct task_struct *task = NULL;
  printk(KERN_INFO "cancel_rsv called\n");
  if (pid <= 0) {
    return -1;
  }

  struct rsv *ptr = rsv_search(get_rb_root(), pid);
  if (ptr == NULL) {
    return -1;
  }
  printk(KERN_INFO "canceling rsv with C = %.9ld, T = %.9ld\n", ptr->C->tv_nsec,
         ptr->T->tv_nsec);
  rb_erase(&ptr->rb_node, get_rb_root());
  // delete from priority list
  // helper function finds and deletes list node and wrapper struct
  rv = plist_remove(get_plist(), pid);

  // cancel timer
  cancel_timer(ptr->t_data);
  kfree(ptr->t_data);
  kfree(ptr->C);
  kfree(ptr->T);
  kfree(ptr);
  // not adding priority removal yet
  return 0;
}
EXPORT_SYMBOL(sys_set_rsv);
EXPORT_SYMBOL(sys_cancel_rsv);
