#include "rsv.h"
#include <linux/ktime.h>

#include <linux/pid.h>
#include <linux/hrtimer.h>

#include <linux/pid_namespace.h>


SYSCALL_DEFINE0(wait_until_next_period) {
  struct rsv* data = NULL; 
  pid_t pid = current->pid;
  struct task_struct *task = NULL;
  struct pid_namespace *ns = NULL;
  printk(KERN_INFO "wait called\n");
  // search for pid 
  data = rsv_search(get_rb_root(), pid);

  if (data == NULL){
    // reservation doesn't exist
    return -1;
  }
  
  // getting task
  ns = current->nsproxy->pid_ns_for_children;

  task = find_task_by_pid_ns(pid, ns);
  
  if (task == NULL){
    printk(KERN_INFO "Task with pid %d not found\n", pid);
    return -1;
  }
  send_sig(SIGSTOP, task, 1);
  return 0;
}
EXPORT_SYMBOL(sys_wait_until_next_period);
