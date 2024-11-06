



#include "rsv.h"

// red black tree for logn look up and insertion
// slower than an hashtable but easier to implement
//

void set_priority(struct task_struct *p, int priority){
  struct sched_param param;
  param.sched_priority = priority;

  sched_setscheduler(p, SCHED_FIFO, &param);
  printk(KERN_INFO "set %d 's priority to %d\n", p->pid, priority);
}
SYSCALL_DEFINE3(set_rsv, pid_t, pid, struct timespec *, C, struct timespec *, T){
  struct rsv *ptr = NULL;
  struct task_struct *task = NULL;
  struct pid_namespace *ns = NULL;
  ktime_t time;
  // C is execution time 
  // T is period/deadline
  printk(KERN_INFO "set_rsv called\n");

  if (C == NULL || T == NULL){
    return -1;
  }
  if (pid < 0){
    return -1;
  }
  if (pid == 0){
    // current is TCB of the calling process
    pid = current->pid;
  }
  /*
  struct hrtimer *timer;
  timer = kalloc(sizeof(struct hrtimer), GFP_KERNEL);
  hrtimer_init(timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
  */
  //timer->function = 
  
  ptr = kzalloc(sizeof(struct rsv), GFP_KERNEL);
  if (ptr == NULL){
    printk(KERN_INFO "kalloc failed\n");
    return -1;
  }
  ptr->pid = pid;
  ptr->C = C;
  ptr->T = T;

  int r = rsv_insert(get_rb_root(), ptr); 
  
  if (r != 0){
    kfree(ptr);
    printk(KERN_INFO "insert failed, pid already exists\n");
  }
  // getting task
  ns = current->nsproxy->pid_ns_for_children;

  task = find_task_by_pid_ns(pid, ns);
  
  if (task == NULL){
    printk(KERN_INFO "Task with pid %d not found\n", pid);
    return -1;
  }
  printk("inserting pid &d into priority list\n", pid);
  struct priority_node *pri_node = plist_insert(get_plist(), T, pid);
  if (pri_node == NULL){
    printk(KERN_INFO "Couldn't insert into priority list\n");
    return -1;
  }
  set_priority(task, pri_node->priority);
  time = timespec_to_ktime(*T);
  if(start_timer(pid, task, time) != 0){
    return -1;
  }
  //send_sig(SIGSTOP, task, 1);
  return 0;

}

SYSCALL_DEFINE1(cancel_rsv, pid_t, pid){
  printk(KERN_INFO "cancel_rsv called\n");
  if (pid <= 0){
    return -1;
  }

  struct rsv *ptr = rsv_search(get_rb_root(), pid);
  if (ptr == NULL){
    return -1;
  }
  printk(KERN_INFO "canceling rsv with C = %.9ld, T = %.9ld\n", ptr->C->tv_nsec, ptr->T->tv_nsec);
  rb_erase(&ptr->rb_node, get_rb_root());
  kfree(ptr);
  // not adding priority removal yet
  return 0;
}
EXPORT_SYMBOL(sys_set_rsv);
EXPORT_SYMBOL(sys_cancel_rsv);
