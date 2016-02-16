#ifndef _PARSE_ELF_SECTION_HEADER_H
#define _PARSE_ELF_SECTION_HEADER_H
void parse_elf_section_header(Elf32_Ehdr* ehdr, FILE *elf_fp);
#endif