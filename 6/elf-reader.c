#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <elf_file>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // 读取ELF文件头
    Elf64_Ehdr ehdr;
    if (read(fd, &ehdr, sizeof(Elf64_Ehdr)) != sizeof(Elf64_Ehdr)) {
        perror("read");
        close(fd);
        return 1;
    }

    // 打印ELF文件头信息
    printf("ELF Header:\n");
    printf("  Magic:   %02x %02x %02x %02x\n",
           ehdr.e_ident[EI_MAG0], ehdr.e_ident[EI_MAG1],
           ehdr.e_ident[EI_MAG2], ehdr.e_ident[EI_MAG3]);
    printf("  Class:                             %d-bit\n", ehdr.e_ident[EI_CLASS] == ELFCLASS64 ? 64 : 32);
    printf("  Data:                              %s\n", ehdr.e_ident[EI_DATA] == ELFDATA2LSB ? "2's complement, little endian" : "2's complement, big endian");
    printf("  Version:                           %d\n", ehdr.e_ident[EI_VERSION]);
    printf("  OS/ABI:                            %d\n", ehdr.e_ident[EI_OSABI]);
    printf("  ABI Version:                       %d\n", ehdr.e_ident[EI_ABIVERSION]);
    printf("  Type:                              %d\n", ehdr.e_type);
    printf("  Machine:                           %d\n", ehdr.e_machine);
    printf("  Version:                           %d\n", ehdr.e_version);
    printf("  Entry point address:               0x%lx\n", ehdr.e_entry);
    printf("  Start of program headers:          %lu (bytes into file)\n", ehdr.e_phoff);
    printf("  Start of section headers:          %lu (bytes into file)\n", ehdr.e_shoff);
    printf("  Flags:                             0x%x\n", ehdr.e_flags);
    printf("  Size of this header:               %lu (bytes)\n", ehdr.e_ehsize);
    printf("  Size of program headers:           %lu (bytes)\n", ehdr.e_phentsize);
    printf("  Number of program headers:         %u\n", ehdr.e_phnum);
    printf("  Size of section headers:           %lu (bytes)\n", ehdr.e_shentsize);
    printf("  Number of section headers:         %u\n", ehdr.e_shnum);
    printf("  Section header string table index: %u\n", ehdr.e_shstrndx);

    close(fd);
    return 0;
}
