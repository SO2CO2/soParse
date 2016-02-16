#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elf.h"
#include "parse_elf_header.h"
#include "parse_elf_section_header.h"
#include "parse_elf_symbol_table.h"
#include "parse_elf_programe_header.h"
#include "parse_elf_dynamic.h"
int main(int argc, char** argv)
{
    if(argc < 2)
	{
		printf("Usage:elf xx.so\n");
	}
	FILE *fp = fopen(argv[1], "rb");
	if(NULL == fp)
	{
		printf("open %s failed\n", argv[1]);
		goto _error;
	}
    Elf32_Ehdr ehdr;
    if(fread(&ehdr, 1, sizeof(Elf32_Ehdr), fp) != sizeof(Elf32_Ehdr))
    {
        printf("read header content fail\n");
        goto _error;
    }
    parse_elf_header(&ehdr);
    parse_elf_section_header(&ehdr, fp);
    parse_elf_symbol_table(&ehdr, fp);
    parse_elf_programe_header(&ehdr, fp);
    parse_elf_dynamic(&ehdr, fp);
	_error:
    if(fp) fclose(fp);
    return 0;
}
