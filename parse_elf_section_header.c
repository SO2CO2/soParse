#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elf.h"

#define SECTION_TYPE_CNT 20
char* section_type_description[SECTION_TYPE_CNT] = {
    "无效段",/*0*/
    "代码段，数据段",/*1*/
    "符号表",/*2*/
    "字符串表",/*3*/
    "重定位表",/*4*/
    "符号表的hash表",/*5*/
    "动态链接信息",/*6*/
    "提示性信息",/*7*/
    "该段在文件中无内容",/*8*/
    "该段保护重定位信息",/*9*/
    "保留段",/*10*/
    "动态链接符号表",/*11*/
    "NULL",/*12*/
    "NULL",/*13*/
    "Array of constructors",/*14*/
    "Array of destructors",/*15*/
    "Array of pre-constructors",/*16*/
    "Section group",/*17*/
    "Extended section indeces",/*18*/
    "Number of defined types"/*19*/
};
void parse_elf_section_header(Elf32_Ehdr* ehdr, FILE *elf_fp)
{
    Elf32_Shdr *p_shdr = NULL;
    char* section_name_buff = NULL;
    int i = 0;

    if(NULL == ehdr || NULL == elf_fp)
        goto _error;

    printf("\n共有 0x%x 个section header, 从偏移 0x%x 开始\n", ehdr->e_shnum, ehdr->e_shoff);
    printf("[Nr] %-24s%-10s%-10s%-10s%-32s\n", "Name", "Addr", "Offset", "Size", "类型描述");

    fseek(elf_fp, ehdr->e_shoff, SEEK_SET);
    p_shdr = (Elf32_Shdr*)malloc(sizeof(Elf32_Shdr) * ehdr->e_shnum);
    if(NULL == p_shdr)
        goto _error;
    if(fread(p_shdr, 1, sizeof(Elf32_Shdr) * ehdr->e_shnum, elf_fp) != sizeof(Elf32_Shdr) * ehdr->e_shnum)
    {
        printf("read section header error\n");
        goto _error;
    }
    //先取字符串表的偏移和大小，以便取section的名字
    section_name_buff = (char*)malloc(p_shdr[ehdr->e_shstrndx].sh_size);
    if(NULL == section_name_buff)
        goto _error;
    fseek(elf_fp, p_shdr[ehdr->e_shstrndx].sh_offset, SEEK_SET);
    if(fread(section_name_buff, 1, p_shdr[ehdr->e_shstrndx].sh_size, elf_fp) != p_shdr[ehdr->e_shstrndx].sh_size)
    {
        printf("read str table section fail\n");
        goto _error;
    }

    for(i = 0; i < ehdr->e_shnum; i++)
    {
        printf("[%02d]", i);
        if(strlen(section_name_buff + p_shdr[i].sh_name) == 0)
        {
            printf(" %-24s", "NULL");
        }
        else
        {
            printf(" %-24s", section_name_buff + p_shdr[i].sh_name);
        }
        printf("0x%-8x",p_shdr[i].sh_addr);
        printf("0x%-8x",p_shdr[i].sh_offset);
        printf("0x%-8x",p_shdr[i].sh_size);
        if(p_shdr[i].sh_type > SECTION_TYPE_CNT - 1)
        {
            printf("0x%-8x",p_shdr[i].sh_type);
        }
        else
        {
            printf("%-32s" ,section_type_description[p_shdr[i].sh_type]);
        }
        printf("\n");
    }
    _error:
    if(p_shdr) free(p_shdr);
    if(section_name_buff) free(section_name_buff);
    return;
}