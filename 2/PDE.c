#include <linux/module.h>      // 引入模块相关的头文件
#include <linux/kernel.h>      // 引入内核相关的头文件
#include <linux/init.h>        // 引入初始化和退出相关的头文件
#include <linux/sched.h>       // 引入进程调度相关的头文件
#include <linux/mm.h>          // 引入内存管理相关的头文件
#include <linux/mm_types.h>    // 引入内存类型相关的头文件
#include <asm/io.h>            // 引入输入输出相关的汇编头文件
#include <linux/uaccess.h>     // 引入用户空间访问相关的头文件

#define PAGE_DIR_MASK 0xFFFFF000UL  // 定义页目录地址掩码，用于获取页目录项的地址
#define PDE_PRESENT    0x1           // 页目录项存在标志
#define PDE_RW        0x2             // 可读写标志
#define PDE_USER      0x4             // 用户可访问标志
#define PDE_PWT       0x8             // 页写-through标志
#define PDE_PCD       0x10            // 页缓存禁用标志
#define PDE_ACCESSED  0x20            // 已访问标志
#define PDE_DIRTY     0x40            // 脏页标志
#define PDE_4MB       0x80            // 4MB 页面标志
#define PDE_GLOBAL    0x100           // 全局页标志

// 函数：读取CR3寄存器值
static unsigned long read_cr3(void)
{
    unsigned long cr3; // 存储CR3寄存器的值
    asm volatile("mov %%cr3, %0" : "=r"(cr3) : : "memory"); // 使用汇编指令读取CR3寄存器
    return cr3; // 返回CR3的值
}

// 函数：打印页目录项的标志
static void print_pde_flags(unsigned long pde)
{
    pr_info("标志位: "); // 打印提示信息
    // 检查并打印页目录项的各个标志位
    if (pde & PDE_PRESENT)   pr_info("Present ");
    if (pde & PDE_RW)        pr_info("R/W ");
    if (pde & PDE_USER)      pr_info("User ");
    if (pde & PDE_PWT)       pr_info("PWT ");
    if (pde & PDE_PCD)       pr_info("PCD ");
    if (pde & PDE_ACCESSED)  pr_info("Accessed ");
    if (pde & PDE_DIRTY)     pr_info("Dirty ");
    if (pde & PDE_4MB)       pr_info("4MB ");
    if (pde & PDE_GLOBAL)    pr_info("Global ");
    pr_info("\n"); // 换行
}

// 初始化函数：打印当前进程的页表目录信息
static int __init print_page_directory_init(void)
{
    unsigned long cr3; // 存储CR3寄存器的值
    pgd_t *pgd; // 指向页目录的指针
    int i; // 循环索引
    struct task_struct *task = current; // 获取当前进程的task_struct
    struct mm_struct *mm; // 指向内存描述符的指针
    unsigned long entry; // 存储页目录项的值

    mm = task->mm; // 获取当前进程的内存管理信息
    if (!mm) { // 如果没有内存管理信息
        mm = task->active_mm; // 获取活动的内存管理信息
        if (!mm) { // 如果仍然没有
            pr_err("无法访问内存描述符\n"); // 打印错误信息
            return -EINVAL; // 返回无效参数错误
        }
    }

    cr3 = read_cr3(); // 读取CR3寄存器的值
    pgd = mm->pgd; // 获取页目录基地址

    // 打印当前进程的页表目录信息
    pr_info("当前进程页表目录信息:\n");
    pr_info("CR3寄存器值: 0x%lx\n", cr3);
    pr_info("页目录基地址: %px\n", pgd);
    pr_info("当前进程: %s (PID: %d)\n", task->comm, task->pid);

    // 遍历页目录项
    for (i = 0; i < PTRS_PER_PGD; i++) {
        entry = native_pgd_val(pgd[i]); // 获取页目录项的值
        if (entry & PDE_PRESENT) { // 如果页目录项存在
            pr_info("\n页目录项 %d:\n", i); // 打印页目录项的索引
            pr_info("地址: %px\n", (void *)(entry & PAGE_DIR_MASK)); // 打印页目录项的地址
            print_pde_flags(entry); // 打印页目录项的标志
        }
    }

    return 0; // 返回成功
}

// 卸载函数：打印模块卸载信息
static void __exit print_page_directory_exit(void)
{
    pr_info("页表目录信息打印模块已卸载\n"); // 打印卸载信息
}

// 定义模块的初始化和卸载函数
module_init(print_page_directory_init);
module_exit(print_page_directory_exit);

// 定义模块的许可证、作者、描述和版本
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("打印当前进程页表目录信息 - OpenEuler版本");
MODULE_VERSION("1.0");
