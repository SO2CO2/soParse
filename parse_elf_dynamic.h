#ifndef _PARSE_ELF_DYNAMIC_H
#define _PARSE_ELF_DYNAMIC_H
void parse_elf_dynamic(Elf32_Ehdr* ehdr, FILE *elf_fp);
#endif