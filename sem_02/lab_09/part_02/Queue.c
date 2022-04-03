#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Batbileg Nomuundalai");

static int irq = 1;
static int dev_id, irq_cnt = 0;

struct workqueue_struct *workqueue_test;
void workqueue_handler(struct work_struct *);

DECLARE_WORK(workname, workqueue_handler);

void workqueue_handler(struct work_struct *work) {
    printk(KERN_INFO "[WORKQUEUE] data: %d\n", work->data);
}

static irqreturn_t test_interrupt(int irq, void *dev_id) {
    if (irq == 1) {
        irq_cnt++;
        printk(KERN_INFO "[interrupt] irq_count = %d\n", irq_cnt);
        queue_work(workqueue_test, &workname);
        return IRQ_HANDLED;
    }

    return IRQ_NONE;
}

static int __init test_workqueue_init(void) {
    if (request_irq(irq, test_interrupt, IRQF_SHARED, "WQ_test_interrupt", &dev_id))
        return -1;
    
    workqueue_test = create_workqueue("workqueue_test");
    if (workqueue_test)
        printk (KERN_INFO "[MODULE] Workqueue created\n");
    
    printk(KERN_INFO "[MODULE] Module is loaded\n");
    return 0;
}
    
static void __exit test_workqueue_exit(void) {
    flush_workqueue(workqueue_test);
    destroy_workqueue(workqueue_test);
    synchronize_irq(irq);
    free_irq (irq, &dev_id);
    printk(KERN_INFO "[MODULE] Result irq_count = %d\n", irq_cnt);
    printk(KERN_INFO "[MODULE] Module is unloaded\n");
}

module_init(test_workqueue_init);
module_exit(test_workqueue_exit);

