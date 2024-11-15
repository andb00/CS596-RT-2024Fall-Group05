#include "rsv.h"

struct rb_root rsv_root = RB_ROOT;
struct rb_root *get_rb_root() {
  return &rsv_root;
}

LIST_HEAD(plist);
struct list_head *get_plist() {
  return &plist;
}

struct task_struct *task_from_pid(pid_t pid) {
  struct pid_namespace *pid_ns = NULL;

  pid_ns = current->nsproxy->pid_ns_for_children;
  // re find the process
  return = find_task_by_pid_ns(pid, pid_ns);
}
static enum hrtimer_restart callback(struct hrtimer *timer) {
  struct timer_data *data = NULL;
  pid_t pid = 0;
  struct task_struct *task = NULL;
  ktime_t comp_time;
  struct rsv *res = NULL;
  struct timespec comp_time_ts;
  int cmp = 0;
  float llC = 0, llT = 0;
  const float SECTONSEC = 1000000000;
  printk("KERN_INFO: callback called\n");
  data = container_of(timer, struct timer_data, timer);
  if (data == NULL) {
    printk(KERN_INFO "Parent struct for timer not found for callback");
    return HRTIMER_NORESTART;
  }
  // when the timer expires
  pid = data->pid;
  task = task_from_pid(pid);
  if (task == NULL) {
    printk(KERN_INFO "Task with pid %d not found\n", pid);
    return HRTIMER_NORESTART;
  }
  hrtimer_forward_now(timer, data->interval);

  // get reservation to check execution and period
  res = rsv_search(get_rb_root(), pid);
  if (res == NULL) {
    printk(KERN_INFO "No reservation found for task with pid %d\n", pid);
    return HRTIMER_NORESTART;
  }
  if (res->C == NULL || res->T == NULL) {
    printk(KERN_INFO "Reservation for task with pid %d's timespecs are null",
           pid);
    return HRTIMER_NORESTART;
  }

  // check ACCUMULATOR
  comp_time = get_updated_task_comp_time(task);
  comp_time_ts = ktime_to_timespec(comp_time);

  cmp = timeCompare(comp_time_ts, res->C);

  if (cmp > 0) {
    fC = res->C->tv_sec * SECTONSEC + res->C->tv_nsec;
    fT = res->T->tv_sec * SECTONSEC + res->T->tv_nsec;
    if (llT == 0) {
      printk(KERN_INFO "Task %d: period is 0");
    } else {
      printk(KERN_INFO "Task %d: budget overrun (util: %d \%)\n", pid,
             llC / llT);
    }
  }

  start_task_time_accu(task);

  if (task->state != RUNNING){
    send_sig(SIGCONT, task, 1);
  }

  // restart the timer by returning
  return HRTIMER_RESTART;
}

struct timer_data *start_timer(pid_t pid, ktime_t time) {
  struct timer_data *timer_data = NULL;
  printk(KERN_INFO "starting timer for %d\n", pid);

  if (task == NULL || task->pid != pid) {
    printk("Null or incorrect task");
    return NULL;
  }

  timer_data = kzalloc(sizeof(struct timer_data), GFP_KERNEL);
  if (timer_data == NULL) {
    printk(KERN_INFO "kalloc failed\n");
    return NULL;
  }

  // block until next period
  hrtimer_init(&(timer_data->timer), CLOCK_MONOTONIC, HRTIMER_MODE_REL_PINNED);
  timer_data->interval = time;
  timer_data->pid = pid;
  timer_data->timer.function = callback;
  // timer_data->timer.data =
  hrtimer_start(&(timer_data->timer), time, HRTIMER_MODE_REL_PINNED);

  return timer_data;
}

void cancel_timer(struct timer_data *data) { 
  hrtimer_cancel(&(data->timer)); 
}
int rsv_insert(struct rb_root *root, struct rsv *new_rsv) {
  // https://www.kernel.org/doc/html/latest/core-api/rbtree.html
  struct rb_node **p = &(root->rb_node);
  struct rb_node *parent = NULL;

  while (*p != NULL) {
    // container of finds the rsv associated with the node
    struct rsv *data = container_of(*p, struct rsv, rb_node);
    int cmp = data->pid - new_rsv->pid;

    parent = *p;

    if (cmp > 0) {
      p = &((*p)->rb_left);
    } else if (cmp < 0) {
      p = &((*p)->rb_right);
    } else {
      // pid already exists
      return -1;
    }
  }
  rb_link_node(&new_rsv->rb_node, parent, p);
  rb_insert_color(&new_rsv->rb_node, root);

  return 0;
}

struct rsv *rsv_search(struct rb_root *root, pid_t pid) {
  struct rb_node *node = root->rb_node;

  while (node != NULL) {
    struct rsv *data = container_of(node, struct rsv, rb_node);
    int cmp = data->pid - pid;

    if (cmp > 0) {
      node = node->rb_left;
    } else if (cmp < 0) {
      node = node->rb_right;
    } else {
      return data;
    }
  }
  return NULL;
}
int timeCompare(struct timespec *a, struct timespec *b) {
  if (a == NULL || b == NULL) {
    printk("Invalid timespecs for timeCompare func, a: %p, b: %p", a, b);
    return -2;
  }
  if (a->tv_sec < b->tv_sec) {
    return -1;
  } else if (a->tv_sec > b->tv_sec) {
    return 1;
  } else {
    if (a->tv_nsec < b->tv_nsec) {
      return -1;
    } else if (a->tv_nsec > b->tv_nsec) {
      return 1;
    } else {
      return 0;
    }
  }
}
// rate monotonic scheduler
struct priority_node *plist_insert(struct list_head *plist, struct timespec *T,
                                   pid_t pid) {
  struct priority_node *new_node = NULL;
  struct list_head *curr = NULL;
  struct priority_node *parent = NULL;
  struct priority_node *data = NULL;
  int low = 1;
  int high = 99;
  int pri = 0;
  int cmp = 0;
  if (T == NULL || plist == NULL) {
    return NULL;
  }
  new_node = kzalloc(sizeof(struct priority_node), GFP_KERNEL);
  if (new_node == NULL) {
    printk("PLIST Insert: Couldn't allocate new_node");
    return NULL;
  }
  new_node->T = T;
  new_node->pid = pid;

  curr = plist->next;
  parent = NULL;
  // look for a node with higher priority
  // reference comparison
  while (curr != plist) {
    data = list_entry(curr, struct priority_node, list);
    if (data == NULL) {
      printk("List Entry Function Failed");
      return NULL;
    }
    cmp = timeCompare(T, data->T);
    if (cmp == -2) {
      printk("Error during time comparison");
      return NULL;
    } else if (cmp == 0) {
      new_node->priority = data->priority;
      list_add(&(new_node->list), curr);
      return new_node;
    } else if (cmp > 0) {
      high = data->priority;
      pri = (low + high) / 2;
      // larger period = lower priority
      // prepend
      list_add(&new_node->list, curr->prev);
      // priority is mid between the two nodes
      // else curr is the head
      if (parent != NULL) {
        low = parent->priority;
      }
      // ideally prioritys would be unique
      // after about 8 insertions to the start, priorities are the same
      // won't error will just schedule tasks with sane priority
      new_node->priority = pri;
      return new_node;
    }
    parent = data;
    curr = curr->next;
  }
  // new node has the highest priority
  if (parent != NULL) {
    low = parent->priority;
  }
  pri = (low + high) / 2;
  new_node->priority = pri;
  // using curr->prev since parent can be null
  list_add(&(new_node->list), curr->prev);

  return new_node;
}
struct priority_node *plist_search(struct list_head *plist, pid_t pid) {
  struct list_head *curr = NULL;
  struct priority_node *data = NULL;
  if (plist == NULL) {
    return NULL;
  }
  curr = plist->next;
  while (curr != plist) {
    data = list_entry(curr, struct priority_node, list);
    if (data == NULL) {
      printk("List Entry Function Failed");
      return NULL;
    }
    if (data->pid == pid) {
      return data;
    }
    curr = curr->next;
  }
  return NULL;
}

int plist_remove(struct list_head *plist, pid_t pid) {
  struct priority_node *pnode = NULL;
  if (plist == NULL) {
    printk(KERN_INFO "Invalid plist");
    return -1;
  }
  pnode = plist_search(plist, pid);
  if (pnode == NULL) {
    printk(KERN_INFO "PLIST Remove: No node found with pid %d", pid_t);
    return -1;
  }
  list_del(&(pnode->list));
  // free timespec from rsv pointer
  kfree(pnode);
  return 0;
}

void start_task_time_accu(struct task_struct *task) {
  if (task == NULL) {
    printk("STARTING TIME ACCUMULATOR: TASK IS NULL");
    return
  }
  // current time
  task->last_toggle_time = ktime_get();
  task->computation_time = ktime_set(0, 0);
  task->state = TASK_RUNNING;
}

ktime_t get_updated_task_comp_time(task_struct *task) {
  ktime_t comp_time, time_since_toggle, now;
  comp_time = task->computation_time;
  if (task->state == TASK_RUNNING) {
    time_since_toggle = task->last_toggle_time;
    now = ktime_get();
    time_since_toggle = ktime_sub(now, time_since_toggle);
    comp_time = ktime_add(comp_time, time_since_toggle);
  }
  // update pointers
  task->computation_time = comp_time;
  task->last_toggle_time = now;
  return comp_time;
}
