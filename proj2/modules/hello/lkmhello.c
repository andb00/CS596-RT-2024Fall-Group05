#include <linux/module.h>
#include <linux/kernel.h>
static int hello(void){
	printk(KERN_NOTICE "Hello World! Group05 in kernel space\n");
	return 0;
}

module_init(hello);
