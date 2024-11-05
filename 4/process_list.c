#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("打印所有进程名称");

static int __init print_processes_init(void)
{
    struct task_struct *task;
    
    // 打印当前进程的名称
    printk(KERN_INFO "当前进程名称: %s\n", current->comm);

    // 遍历系统中所有进程
    for_each_process(task) {
        printk(KERN_INFO "进程名称: %s\n", task->comm);
    }

    return 0;
}

static void __exit print_processes_exit(void)
{
    printk(KERN_INFO "模块已卸载\n");
}

module_init(print_processes_init);
module_exit(print_processes_exit);

