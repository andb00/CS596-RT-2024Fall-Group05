
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/rbtree.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/slab.h>

#ifndef _RSV_H_
#define _RSV_H_ 

extern struct list_head plist;

extern struct rb_root rsv_root;
// reservation 
struct rsv {
  struct rb_node rb_node;
  pid_t pid;
  // execution time
  struct timespec* C;
  // period/deadline
  struct timespec* T;
};
// priority linked list 
struct priority_node{
  struct list_head list;
  // period, used to calculate priority
  struct timespec* T;
  int priority;
};

// timer data (so callback will work)
struct timer_data {
  struct hrtimer timer;
  ktime_t interval;
  pid_t pid;
};
// rb tree for semi efficient lookup and insertion 
// singletons
struct rb_root *get_rb_root(void);


struct list_head *get_plist(void);
// linked list for priority
// rb tree would be more efficient but difficult to implement   
//

static enum hrtimer_restart callback(struct hrtimer *timer);


int start_timer(pid_t pid, struct task_struct* task, ktime_t time);

int timeCompare(struct timespec* a, struct timespec* b);

int rsv_insert(struct rb_root* root, struct rsv* rsv);

struct rsv *rsv_search(struct rb_root* root, pid_t pid);

struct priority_node *plist_insert(struct list_head* plist, struct timespec* T, pid_t pid);

#endif
