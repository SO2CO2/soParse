#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elf.h"

char* symbol_type[] = {
    "NOTYPE",
    "OBJECT",
    "FUNC",
    "SECTION",
    "FILE"
};

void parse_elf_symbol_table(Elf32_Ehdr* ehdr, FILE *elf_fp)
{
    Elf32_Shdr *p_shdr = NULL;
    Elf32_Sym *p_symtab = NULL;
    int i = 0, symbol_cnt = 0;
    uint32_t str_table_offset = 0, str_table_size = 0;
    char* str_table_buff = NULL;

    if(NULL == ehdr || NULL == elf_fp)
        goto _error;

    fseek(elf_fp, ehdr->e_shoff, SEEK_SET);

    p_shdr = (Elf32_Shdr*)malloc(sizeof(Elf32_Shdr) * ehdr->e_shnum);

    if(NULL == p_shdr)
        goto _error;

    if(fread(p_shdr, 1, sizeof(Elf32_Shdr) * ehdr->e_shnum, elf_fp) != sizeof(Elf32_Shdr) * ehdr->e_shnum)
    {
        printf("read section header error\n");
        goto _error;
    }

    for(i = 0; i < ehdr->e_shnum; i++)
    {
        //还有一个SHT_SYM符号表不知道什么情况下是符号表
        if(p_shdr[i].sh_type == SHT_DYNSYM )
        {
            break;
        }
        else if(p_shdr[i].sh_type == SHT_STRTAB)
        {
            str_table_offset = p_shdr[i].sh_offset;
            str_table_size = p_shdr[i].sh_size;
        }
    }
    if(i == ehdr->e_shnum)
    {
        printf("\n没有符号表\n");
        goto _error;
    }
    fseek(elf_fp, p_shdr[i].sh_offset, SEEK_SET);
    p_symtab = (Elf32_Sym*)malloc(p_shdr[i].sh_size);
    if(NULL == p_symtab)
        goto _error;
    if(fread(p_symtab, 1, p_shdr[i].sh_size, elf_fp) != p_shdr[i].sh_size)
    {
        printf("read symbol table fail\n");
        goto _error;
    }
    symbol_cnt = (p_shdr[i].sh_size) / (sizeof(Elf32_Sym));
    printf("\n共有%d个symbol\n", symbol_cnt);

    //先找字符串表，来找符号的名字
    for(i = 0; i < ehdr->e_shnum; i++)
    {
        if(p_shdr[i].sh_type == SHT_STRTAB)
        {
            str_table_offset = p_shdr[i].sh_offset;
            str_table_size = p_shdr[i].sh_size;
            break;
        }
    }
    str_table_buff = (char*)malloc(str_table_size);
    if(NULL == str_table_buff)
        goto _error;
    fseek(elf_fp, str_table_offset, SEEK_SET);
    if(fread(str_table_buff, 1, str_table_size, elf_fp) != str_table_size)
    {
        printf("read string table fail\n");
        goto _error;
    }
    printf("[Nr] %-64s%-10s%-10s%-10s\n", "Name", "Value", "Size", "Type");
    for(i = 0; i < symbol_cnt; i++)
    {
        printf("[%02d]", i);
        printf(" %-64s", str_table_buff + p_symtab[i].st_name);
        printf("0x%-8x", p_symtab[i].st_value);
        printf("0x%-8x", p_symtab[i].st_size);
        printf("%-10s", symbol_type[p_symtab[i].st_info & 0xF]);
        printf("\n");
    }

    _error:
    if(p_shdr) free(p_shdr);
    if(p_symtab) free(p_symtab);
    if(str_table_buff) free(str_table_buff);
}