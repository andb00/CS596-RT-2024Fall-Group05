#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <asm/cacheflush.h>
#include <asm/pgtable.h>

#define __NR_syscalls 397
#define __NR_calc 397
// defines a system call handling function
// maps a pt_regs (CPU Register State struct) to a long
typedef asmlinkage long (*sys_call_ptr_t)(const struct pt_regs *);

static sys_call_ptr_t *table;

// edit calc sys call
typedef asmlinkage long (*calc_func)(int p1, int p2, char op, int* res);

calc_func original;

int mod_calc(int param1, int param2, char operation, int* result){
	if (result == NULL)
		return -1;
	if (param2 == 0)
		return -1;
	*result = param1 % param2;
	printk(KERN_INFO "OVERRIDED SYS_CALC EXECUTED SUCCESFULLY: %d", *result);
	return 0;
}

static sys_call_ptr_t* sc_table(void){
	sys_call_ptr_t* table = kallsyms_lookup_name("sys_call_table");
	return table;
}
static int __init mod_calc_init(void){
	table = sc_table(); 
	original = (calc_func)table[__NR_calc];


	table[__NR_calc] = (sys_call_ptr_t)mod_calc;
	
	return 0;
}

static void __exit mod_calc_exit(void){
	table[__NR_calc] = (sys_call_ptr_t)original;
	// no memory was allocated dynamically
}

module_init(mod_calc_init);
module_exit(mod_calc_exit);

MODULE_LICENSE("GPL");
