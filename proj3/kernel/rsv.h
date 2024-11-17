
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

/*
I started designing before fully understanding the problem
The RBTree was because I thought it would be about as easy 
  to implement as a linked list, with quicker inserts and searches
Ended up being useless since I needed priorities in sorted order for O(n) insertion
By the time I realized this it was easier to leave it than to delete
  */
extern struct rb_root rsv_root;
// reservation 
struct rsv {
  struct rb_node rb_node;
  pid_t pid;
  // execution time
  struct timespec* C;
  // period/deadline
  struct timespec* T;

  struct timer_data* t_data;
};
// priority linked list 
struct priority_node{
  struct list_head list;
  // period, used to calculate priority
  struct timespec* T;
  int priority;
  pid_t pid;
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
// linked list for priority( to maintain sorted order, 
//    and allow search on multiple criteria)
//
struct task_struct *task_from_pid(pid_t pid);

struct timer_data *start_timer(struct task_struct* task, ktime_t time);

void cancel_timer(struct timer_data* data);

int timeCompare(struct timespec* a, struct timespec* b);

int rsv_insert(struct rb_root* root, struct rsv* rsv);

struct rsv *rsv_search(struct rb_root* root, pid_t pid);

struct priority_node *plist_insert(struct list_head* plist, struct timespec* T, pid_t pid);

struct priority_node* plist_search(struct list_head* plist, pid_t pid);

int plist_remove(struct list_head* plist, pid_t pid);

void start_task_time_accu(struct task_struct* task);

ktime_t get_updated_task_comp_time(struct task_struct* task);

#endif
