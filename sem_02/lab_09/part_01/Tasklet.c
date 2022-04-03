#include <stdio.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Batbileg Nomuundalai");

static int irq = 1;
static int dev_id, irq_cnt = 0;

char tasklet_data[] = "tasklet_data";
void tasklet_handler(unsigned long data);
DECLARE_TASKLET(tasklet, tasklet_handler, (unsigned long) &tasklet_data);
static struct timespec64 cur_time;

void tasklet_handler(unsigned long data) {
    printk(KERN_INFO "[TASKLET] state: %ld, count: %d, data: %s\n", tasklet.state, tasklet.count, tasklet.data);
}

static irqreturn_t test_interrupt(int irq, void *dev_id) {
    if (irq == 1) {
        irq_cnt++;
        printk(KERN_INFO "[interrupt] irq_count = %d\n", irq_cnt);
        tasklet_schedule(&tasklet);
        return IRQ_HANDLED;
    }

    return IRQ_NONE;
}

static int __init test_tasklet_init(void) {
    if (request_irq(irq, test_interrupt, IRQF_SHARED,
        "test_interrupt", &dev_id)) {
        return -1;
    }
    printk(KERN_INFO "[MODULE] Module is loaded\n");
    return 0;
}

static void __exit test_tasklet_exit(void) {
    tasklet_kill(&tasklet);
    synchronize_irq(irq);
    free_irq(irq, &dev_id);
    printk(KERN_INFO "[MODULE] Result irq_count = %d\n", irq_cnt);
    printk(KERN_INFO "[MODULE] Module is unloaded\n");
}

module_init(test_tasklet_init);
module_exit(test_tasklet_exit);