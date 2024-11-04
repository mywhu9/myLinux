#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <asm/io.h>
#include <linux/uaccess.h>

#define PAGE_DIR_MASK 0xFFFFF000UL
#define PDE_PRESENT    0x1
#define PDE_RW        0x2
#define PDE_USER      0x4
#define PDE_PWT       0x8
#define PDE_PCD      0x10
#define PDE_ACCESSED 0x20
#define PDE_DIRTY    0x40
#define PDE_4MB      0x80
#define PDE_GLOBAL  0x100

static unsigned long read_cr3(void)
{
    unsigned long cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3) : : "memory");
    return cr3;
}

static void print_pde_flags(unsigned long pde)
{
    pr_info("标志位: ");
    if (pde & PDE_PRESENT)   pr_info("Present ");
    if (pde & PDE_RW)        pr_info("R/W ");
    if (pde & PDE_USER)      pr_info("User ");
    if (pde & PDE_PWT)       pr_info("PWT ");
    if (pde & PDE_PCD)       pr_info("PCD ");
    if (pde & PDE_ACCESSED)  pr_info("Accessed ");
    if (pde & PDE_DIRTY)     pr_info("Dirty ");
    if (pde & PDE_4MB)       pr_info("4MB ");
    if (pde & PDE_GLOBAL)    pr_info("Global ");
    pr_info("\n");
}

static int __init print_page_directory_init(void)
{
    unsigned long cr3;
    pgd_t *pgd;
    int i;
    struct task_struct *task = current;
    struct mm_struct *mm;
    unsigned long entry;

    mm = task->mm;
    if (!mm) {
        mm = task->active_mm;
        if (!mm) {
            pr_err("无法访问内存描述符\n");
            return -EINVAL;
        }
    }

    cr3 = read_cr3();
    pgd = mm->pgd;

    pr_info("当前进程页表目录信息:\n");
    pr_info("CR3寄存器值: 0x%lx\n", cr3);
    pr_info("页目录基地址: %px\n", pgd);
    pr_info("当前进程: %s (PID: %d)\n", task->comm, task->pid);

    for (i = 0; i < PTRS_PER_PGD; i++) {
        entry = native_pgd_val(pgd[i]);
        if (entry & PDE_PRESENT) {
            pr_info("\n页目录项 %d:\n", i);
            pr_info("地址: %px\n", (void *)(entry & PAGE_DIR_MASK));
            print_pde_flags(entry);
        }
    }

    return 0;
}

static void __exit print_page_directory_exit(void)
{
    pr_info("页表目录信息打印模块已卸载\n");
}

module_init(print_page_directory_init);
module_exit(print_page_directory_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("打印当前进程页表目录信息 - OpenEuler版本");
MODULE_VERSION("1.0");
