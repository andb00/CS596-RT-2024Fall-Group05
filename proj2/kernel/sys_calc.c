#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/module.h>


SYSCALL_DEFINE4(calc, int, param1, int, param2, char, operation, int*, result) {
	printk(KERN_INFO "SYS_CALC called");
	// input validation
	if (result == NULL)
		return -1;
	int res = 0;
	switch (operation){
		case '+':
			res = param1 * param2;
			break;
		case '-':
			res = param1 - param2;
			break;
		case '*':
			res = param1 * param2;
			break;
		case '/':
			if (param2 == 0)
				return -1;
			res = param1 / param2;
			break;
		default:
			return -1;
	}
	*result = res;
	printk(KERN_INFO "SYS_CALC EXECUTED SUCCESFULLY: %d", *result);
	return 0;
}

EXPORT_SYMBOL(sys_calc);
