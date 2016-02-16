#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elf.h"


char* program_segment_type[9] =
{
    "NULL",
    "LOAD",
    "DYNAMIC",
    "INTERP",
    "NOTE",
    "SHLIB",
    "PHDR",
    "TLS",
    "NUM"
};
void parse_elf_programe_header(Elf32_Ehdr* ehdr, FILE *elf_fp)
{
    Elf32_Phdr *p_phdr = NULL;
    int i = 0;
    char phdr_flag[4] = {0};

    if(NULL == ehdr || NULL == elf_fp)
        goto _error;

    printf("\n共有 0x%x program header, 从偏移 0x%x 开始\n", ehdr->e_phnum, ehdr->e_phoff);
    printf("[Nr] %-12s%-10s%-10s%-10s%-10s%-10s%-10s%-10s\n",
        "Type", "offset", "VirtAddr", "PhysAddr", "FileSize", "MemSize", "Flg", "Align");

    fseek(elf_fp, ehdr->e_phoff, SEEK_SET);
    p_phdr = (Elf32_Phdr*)malloc(sizeof(Elf32_Phdr) * ehdr->e_phnum);
    if(NULL == p_phdr)
        goto _error;
    if(fread(p_phdr, 1, sizeof(Elf32_Phdr) * ehdr->e_phnum, elf_fp) != sizeof(Elf32_Phdr) * ehdr->e_phnum)
    {
        printf("read program header error\n");
        goto _error;
    }

    for(i = 0; i < ehdr->e_phnum; i++)
    {
        printf("[%02d]", i);
        if(p_phdr[i].p_type < 9)
        {
            printf(" %-12s", program_segment_type[p_phdr[i].p_type]);
        }
        else
        {
            printf(" 0x%-8x  ", p_phdr[i].p_type);
        }
        printf("0x%-8x",p_phdr[i].p_offset);
        printf("0x%-8x",p_phdr[i].p_vaddr);
        printf("0x%-8x",p_phdr[i].p_paddr);
        printf("0x%-8x",p_phdr[i].p_filesz);
        printf("0x%-8x",p_phdr[i].p_memsz);
        memset(phdr_flag, 0, 4);
        if((p_phdr[i].p_flags & PF_R) == PF_R)
        {
            phdr_flag[strlen(phdr_flag)] = 'R';
        }
        if((p_phdr[i].p_flags & PF_W) == PF_W)
        {
            phdr_flag[strlen(phdr_flag)] = 'W';
        }
        if((p_phdr[i].p_flags & PF_X) == PF_X)
        {
            phdr_flag[strlen(phdr_flag)] = 'X';
        }
        printf("%-10s",phdr_flag);
        printf("0x%-8x",p_phdr[i].p_flags);
        printf("0x%-8x",p_phdr[i].p_align);
        printf("\n");
    }
    _error:
    if(p_phdr) free(p_phdr);
    return;
}