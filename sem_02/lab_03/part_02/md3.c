#include <linux/init.h>
#include <linux/module.h>
#include "md.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nomuundalai");
MODULE_DESCRIPTION("lab_03");

static int __init md_init(void)
{
    printk("LAB3.3 : module md3 start!\n");
    printk("LAB3.3 : data string exported from md1 : %s\n", md1_data);
    printk("LAB3.3 : string returned md1_proc() is : %s\n", md1_proc());
    return -1;
}

static void __exit md_exit(void)
{
    printk("LAB3.3 : module md3 unloaded!\n");
}

module_init(md_init);
module_exit(md_exit);