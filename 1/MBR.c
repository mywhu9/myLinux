#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MBR_SIZE 512
#define PARTITION_TABLE_OFFSET 446
#define NUM_PARTITIONS 4
#define PARTITION_ENTRY_SIZE 16

#pragma pack(push, 1)  // 以1字节对齐结构体
typedef struct {
    uint8_t  status;        // 启动状态
    uint8_t  start_head;    // 起始磁头
    uint8_t  start_sector;  // 起始扇区
    uint8_t  start_cylinder; // 起始气缸
    uint8_t  type;          // 分区类型
    uint8_t  end_head;      // 结束磁头
    uint8_t  end_sector;    // 结束扇区
    uint8_t  end_cylinder;  // 结束气缸
    uint32_t start_lba;     // 分区起始扇区（逻辑块地址）
    uint32_t size;          // 分区大小（以扇区为单位）
} PartitionEntry;

typedef struct {
    uint8_t boot_code[446]; // 引导代码
    PartitionEntry partitions[NUM_PARTITIONS]; // 分区表
    uint16_t signature;      // MBR签名（0x55AA）
} MBR;
#pragma pack(pop)

void print_partition_info(const PartitionEntry *entry, int index) {
    printf("分区 %d:\n", index + 1);
    printf("  启动状态: %u\n", entry->status);
    printf("  起始地址: %u\n", entry->start_lba);
    printf("  大小: %u 扇区\n", entry->size);
    printf("  类型: %u\n", entry->type);
    printf("  结束气缸: %u\n", entry->end_cylinder);
    printf("  结束扇区: %u\n", entry->end_sector);
    printf("  结束磁头: %u\n", entry->end_head);
}

int main() {
    FILE *disk = fopen("/dev/vda", "rb"); // 修改为实际磁盘路径
    if (!disk) {
        perror("无法打开磁盘");
        return 1;
    }

    MBR mbr;
    fread(&mbr, sizeof(MBR), 1, disk);
    fclose(disk);

    // 验证MBR签名
    if (mbr.signature != 0xAA55) {
        printf("无效的MBR签名\n");
        return 1;
    }

    // 打印分区信息
    for (int i = 0; i < NUM_PARTITIONS; i++) {
        print_partition_info(&mbr.partitions[i], i);
    }

    return 0;
}
