#ifndef _PARSE_ELF_SYMBOL_TABLE_H
#define _PARSE_ELF_SYMBOL_TABLE_H
void parse_elf_symbol_table(Elf32_Ehdr* ehdr, FILE *elf_fp);
#endif