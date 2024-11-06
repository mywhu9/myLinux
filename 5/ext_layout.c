#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fs.h>

// EXT2/3/4 超级块魔数
#define EXT2_SUPER_MAGIC  0xEF53

// 定义超级块结构
struct ext2_super_block {
    uint32_t s_inodes_count;      /* Inodes数量 */
    uint32_t s_blocks_count;      /* 块数量 */
    uint32_t s_r_blocks_count;    /* 保留块数量 */
    uint32_t s_free_blocks_count; /* 空闲块数量 */
    uint32_t s_free_inodes_count; /* 空闲inode数量 */
    uint32_t s_first_data_block;  /* 第一个数据块 */
    uint32_t s_log_block_size;    /* 块大小 = 1024 << s_log_block_size */
    uint32_t s_log_frag_size;     /* 片大小 */
    uint32_t s_blocks_per_group;  /* 每组块数 */
    uint32_t s_frags_per_group;   /* 每组片数 */
    uint32_t s_inodes_per_group;  /* 每组inode数 */
    uint32_t s_mtime;            /* 最后一次挂载时间 */
    uint32_t s_wtime;            /* 最后一次写入时间 */
    uint16_t s_mnt_count;        /* 挂载次数 */
    uint16_t s_max_mnt_count;    /* 最大挂载次数 */
    uint16_t s_magic;            /* 魔数 */
    uint16_t s_state;            /* 文件系统状态 */
    uint16_t s_errors;           /* 错误处理方式 */
    uint16_t s_minor_rev_level;  /* 次版本级别 */
    uint32_t s_lastcheck;        /* 最后一次检查时间 */
    uint32_t s_checkinterval;    /* 最大检查间隔 */
    uint32_t s_creator_os;       /* 创建该文件系统的操作系统 */
    uint32_t s_rev_level;        /* 版本级别 */
    uint16_t s_def_resuid;       /* 保留块的默认uid */
    uint16_t s_def_resgid;       /* 保留块的默认gid */
    uint32_t s_first_ino;        /* 第一个非保留inode */
    uint16_t s_inode_size;       /* inode结构大小 */
    uint16_t s_block_group_nr;   /* 块组号 */
    uint32_t s_feature_compat;   /* 兼容特性 */
    uint32_t s_feature_incompat; /* 不兼容特性 */
    uint32_t s_feature_ro_compat;/* 只读兼容特性 */
    uint8_t  s_uuid[16];         /* 128位uuid */
    char     s_volume_name[16];  /* 卷名 */
    char     s_last_mounted[64]; /* 最后一个挂载点路径 */
    uint32_t s_algorithm_usage_bitmap; /* 压缩算法 */
    uint8_t  s_prealloc_blocks;  /* 预分配块数 */
    uint8_t  s_prealloc_dir_blocks; /* 目录预分配 */
    uint16_t s_padding1;         /* 对齐 */
    uint32_t s_reserved[204];    /* 填充到1024字节 */
};

// 计算块大小的宏
#define EXT2_BLOCK_SIZE(s) (1024 << (s)->s_log_block_size)

// 读取超级块信息的函数
void print_superblock_info(int fd) {
    struct ext2_super_block sb;
    
    // 定位到超级块的位置（第1024字节处）
    lseek(fd, 1024, SEEK_SET);
    
    // 读取超级块内容
    if (read(fd, &sb, sizeof(sb)) != sizeof(sb)) {
        perror("读取超级块失败");
        exit(1);
    }

    // 检查文件系统魔数
    if (sb.s_magic != EXT2_SUPER_MAGIC) {
        printf("错误：不是有效的EXT文件系统（魔数不匹配）\n");
        printf("找到的魔数: 0x%x, 期望的魔数: 0x%x\n", sb.s_magic, EXT2_SUPER_MAGIC);
        exit(1);
    }

    // 获取实际的块大小
    int block_size = EXT2_BLOCK_SIZE(&sb);

    // 打印文件系统基本信息
    printf("\n=== EXT文件系统布局信息 ===\n\n");
    
    // 打印超级块信息
    printf("1. 超级块布局:\n");
    printf("   起始块号: %d\n", sb.s_first_data_block);
    printf("   占用块数: 1\n");
    printf("   块大小: %d bytes\n", block_size);
    printf("   总块数: %u\n", sb.s_blocks_count);
    printf("   每组块数: %u\n", sb.s_blocks_per_group);
    
    // 计算块组数量
    unsigned long blocks_per_group = sb.s_blocks_per_group;
    unsigned long total_blocks = sb.s_blocks_count;
    unsigned long group_count = (total_blocks + blocks_per_group - 1) / blocks_per_group;
    
    printf("\n2. 块组描述符表布局:\n");
    printf("   起始块号: %d\n", sb.s_first_data_block + 1);
    printf("   组数量: %lu\n", group_count);
    printf("   每组块数: %lu\n", blocks_per_group);
    
    // 打印前几个块组的布局和最后一个块组的布局
    printf("\n3. 块组布局:\n");
    for (unsigned long i = 0; i < group_count; i++) {
        if (i < 3 || i == group_count - 1) {  // 只打印前3个和最后一个块组
            printf("\n   块组 %lu:\n", i);
            
            // 计算该块组的起始块号
            unsigned long group_first_block = i * blocks_per_group;
            
            // 块位图（始终占用一个块）
            printf("   - 块位图:\n");
            printf("     起始块号: %lu\n", group_first_block + 3);
            printf("     占用块数: 1\n");
            
            // inode位图（始终占用一个块）
            printf("   - inode位图:\n");
            printf("     起始块号: %lu\n", group_first_block + 4);
            printf("     占用块数: 1\n");
            
            // inode表
            printf("   - inode表:\n");
            printf("     起始块号: %lu\n", group_first_block + 5);
            printf("     占用块数: %d\n", 
                   (int)(sb.s_inodes_per_group * sb.s_inode_size / block_size));

            // 数据块
            unsigned long data_blocks_start = group_first_block + 5 + 
                (sb.s_inodes_per_group * sb.s_inode_size / block_size);
            printf("   - 数据块:\n");
            printf("     起始块号: %lu\n", data_blocks_start);
            
            // 计算该组中的数据块数量
            unsigned long data_blocks;
            if (i == group_count - 1) {
                data_blocks = total_blocks - group_first_block - 
                    (data_blocks_start - group_first_block);
            } else {
                data_blocks = blocks_per_group - 
                    (data_blocks_start - group_first_block);
            }
            printf("     占用块数: %lu\n", data_blocks);
        } else if (i == 3) {
            printf("\n   ... （中间的块组布局类似） ...\n");
        }
    }

    // 打印额外的文件系统信息
    printf("\n4. 其他信息:\n");
    printf("   Inode大小: %u bytes\n", sb.s_inode_size);
    printf("   每组Inode数: %u\n", sb.s_inodes_per_group);
    printf("   总Inode数: %u\n", sb.s_inodes_count);
    printf("   空闲块数: %u\n", sb.s_free_blocks_count);
    printf("   空闲Inode数: %u\n", sb.s_free_inodes_count);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("用法: %s <设备路径>\n", argv[0]);
        printf("示例: %s /dev/sda1\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("打开设备失败");
        return 1;
    }

    print_superblock_info(fd);
    close(fd);
    return 0;
}
