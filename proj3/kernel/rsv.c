#include "rsv.h"

struct rb_root rsv_root = RB_ROOT;
struct rb_root *get_rb_root(){
  return &rsv_root;
}

LIST_HEAD(plist);
struct list_head *get_plist(){
  return &plist;
}

static enum hrtimer_restart callback(struct hrtimer *timer){
  struct timer_data *data = NULL;
  struct pid_namespace *pid_ns = NULL;
  pid_t pid = 0;
  struct task_struct *task = NULL;
  printk("KERN_INFO: callback called\n");
  data = container_of(timer, struct timer_data, timer);
  if (data == NULL){
    printk(KERN_INFO "Parent struct for timer not found for callback");
    return HRTIMER_NORESTART;
  }
  // when the timer expires
  pid = data->pid;
  pid_ns = current->nsproxy->pid_ns_for_children;
  // re find the process
  task = find_task_by_pid_ns(pid, pid_ns);
  if (task == NULL){
    printk(KERN_INFO "Task with pid %d not found\n", pid);
    return HRTIMER_NORESTART;
  }
  hrtimer_forward_now(timer, data->interval);
  
  send_sig(SIGCONT, task, 1);
  // restart the timer by returning
  return HRTIMER_RESTART;
}

int start_timer(pid_t pid, struct task_struct* task, ktime_t time){
  struct rsv* data = NULL;
  struct pid_namespace *ns = NULL;
  struct timer_data *timer_data = NULL;
  printk(KERN_INFO "starting timer for %d\n",pid);

  if(task == NULL || task->pid != pid){
    printk("Null or incorrect task");
    return -1;
  }

  timer_data = kzalloc(sizeof(struct timer_data), GFP_KERNEL);
  if (timer_data == NULL){
    printk(KERN_INFO "kalloc failed\n");
    return -1;
  }
  
  // block until next period 
  hrtimer_init(&(timer_data->timer), CLOCK_MONOTONIC, HRTIMER_MODE_REL_PINNED);
  timer_data->interval = time;
  timer_data->pid = pid;
  timer_data->timer.function = callback;
  //timer_data->timer.data = 
  hrtimer_start(&(timer_data->timer), time, HRTIMER_MODE_REL_PINNED);

  return 0;
}

int rsv_insert(struct rb_root *root, struct rsv *new_rsv){
// https://www.kernel.org/doc/html/latest/core-api/rbtree.html
  struct rb_node **p = &(root->rb_node);
  struct rb_node *parent = NULL;

  while (*p != NULL) {
    // container of finds the rsv associated with the node
    struct rsv *data= container_of(*p, struct rsv, rb_node);
    int cmp = data->pid - new_rsv->pid;

    parent = *p;

    if (cmp > 0) {
      p = &((*p)->rb_left);
    } else if(cmp < 0){
      p = &((*p)->rb_right);
    }
    else {
      // pid already exists
      return -1;
    }
    
  }
  rb_link_node(&new_rsv->rb_node, parent, p);
  rb_insert_color(&new_rsv->rb_node, root);
  
  return 0;
}

struct rsv *rsv_search(struct rb_root *root, pid_t pid){
  struct rb_node *node= root->rb_node;

  while (node != NULL){
    struct rsv *data = container_of(node, struct rsv, rb_node);
    int cmp = data->pid - pid;

    if (cmp > 0){
      node = node->rb_left;
    }else if(cmp < 0){
      node = node->rb_right;
    }else{
      return data;
    }
  }
  return NULL;
}
int timeCompare(struct timespec* a, struct timespec* b){
  if (a->tv_sec < b->tv_sec){
    return -1;
  } 
  else if (a->tv_sec > b->tv_sec){
    return 1;
  }
  else{
    if (a->tv_nsec < b->tv_nsec){
      return -1;
    }
    else if (a->tv_nsec > b->tv_nsec){
      return 1;
    }
    else{
      return 0;
    }
  }
}
// rate monotonic scheduler
struct priority_node *plist_insert(struct list_head *plist, struct timespec *T, pid_t pid){
  struct priority_node *new_node = NULL;
  struct list_head * curr = NULL;
  struct priority_node * parent = NULL;
  int low = 1;
  int high = 99;
  int pri = 0;
  if (T == NULL){
    return NULL;
  } 
  new_node = kzalloc(sizeof(struct priority_node), GFP_KERNEL);
  if (new_node == NULL){
    printk("PLIST Insert: Couldn't allocate new_node");
    return NULL;
  }
  new_node -> T = T;
  
  curr = plist -> next;
  parent = NULL;
  // look for a node with higher priority
  // reference comparison
  while (curr != plist){
    struct priority_node *data = list_entry(curr, struct priority_node, list);
    int cmp = timeCompare(T, data -> T);
    if (cmp == 0){
      new_node -> priority = data -> priority;
      list_add(&(new_node -> list), curr);
      return new_node;
    }
    else if (cmp > 0){
      high = data -> priority;
      pri = (low + high) / 2;
      // larger period = lower priority
      // prepend
      list_add(&new_node -> list,curr->prev);
      // priority is mid between the two nodes
        // else curr is the head
      if (parent != NULL){
        low = parent -> priority;
      }
      // ideally prioritys would be unique
      // after about 8 insertions to the start, priorities are the same
      // won't error will just schedule tasks with sane priority 
      new_node->priority = pri;
      return new_node;
    }
    parent = data;
    curr = curr -> next;
  }
  // new node has the highest priority 
  if (parent != NULL){
    low = parent -> priority;
  }
  pri = (low+high) / 2;
  new_node -> priority = pri;
  // using curr->prev since parent can be null
  list_add(&(new_node -> list), curr->prev);
  
  return new_node;
}

 
