#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elf.h"
void parse_elf_header(Elf32_Ehdr* ehdr)
{
    if(NULL == ehdr) return;
    printf("ELF头:\n");
    printf("  magic:  ");
    int i = 0;
    for(i = 0; i < EI_NIDENT; i++)
    {
        printf("%x ", ehdr->e_ident[i]);
    }
    printf("\n");
    if(ehdr->e_ident[4] == 1)
        printf("  位数:  32位\n");
    else if(ehdr->e_ident[4] == 2)
        printf("  位数:  64位\n");
    if(ehdr->e_ident[5] == 1)
        printf("  字节序:  little-endian\n");
    else if(ehdr->e_ident[5] == 2)
        printf("  字节序:  big-endian\n");

    printf("  类型:  ");
    switch(ehdr->e_type)
    {
        case 1:
            printf(".o文件\n");
            break;
        case 2:
            printf("可执行文件\n");
            break;
        case 3:
            printf("共享库文件.so\n");
            break;
        default:
            break;
    }
    printf("  CPU架构:  0x%x\n", ehdr->e_machine);
    printf("  program头偏移:  0x%x\n", ehdr->e_phoff);
    printf("  section头偏移:  0x%x\n", ehdr->e_shoff);
    printf("  elf头的大小:  0x%x\n", ehdr->e_ehsize);
    printf("  program头的大小:  0x%x\n", ehdr->e_phentsize);
    printf("  program头的数量:  0x%x\n", ehdr->e_phnum);
    printf("  section头的大小:  0x%x\n", ehdr->e_shentsize);
    printf("  section头的数量:  0x%x\n", ehdr->e_shnum);
    printf("  字符串表是段表中的第几个:  0x%x\n", ehdr->e_shstrndx);

}