all:
	gcc -o elf elf.c parse_elf_header.c parse_elf_section_header.c parse_elf_symbol_table.c parse_elf_programe_header.c parse_elf_dynamic.c
