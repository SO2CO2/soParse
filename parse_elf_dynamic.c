#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elf.h"


char* dynamic_type[31] = {
     "DT_NULL",     /* Marks end of dynamic section */
     "DT_NEEDED",   /* Name of needed library */
     "DT_PLTRELSZ", /* Size in bytes of PLT relocs */
     "DT_PLTGOT",   /* Processor defined value */
     "DT_HASH",     /* Address of symbol hash table */
     "DT_STRTAB",   /* Address of string table */
     "DT_SYMTAB",   /* Address of symbol table */
     "DT_RELA",     /* Address of Rela relocs */
     "DT_RELASZ",   /* Total size of Rela relocs */
     "DT_RELAENT",  /* Size of one Rela reloc */
     "DT_STRSZ",    /* Size of string table */
     "DT_SYMENT",   /* Size of one symbol table entry */
     "DT_INIT",     /* Address of init function */
     "DT_FINI",     /* Address of termination function */
     "DT_SONAME",   /* Name of shared object */
     "DT_RPATH",    /* Library search path (deprecated) */
     "DT_SYMBOLIC", /* Start symbol search here */
     "DT_REL",      /* Address of Rel relocs */
     "DT_RELSZ",    /* Total size of Rel relocs */
     "DT_RELENT",   /* Size of one Rel reloc */
     "DT_PLTREL",   /* Type of reloc in PLT */
     "DT_DEBUG",    /* For debugging; unspecified */
     "DT_TEXTREL",  /* Reloc might modify .text */
     "DT_JMPREL",   /* Address of PLT relocs */
     "DT_BIND_NOW", /* Process relocations of object */
     "DT_INIT_ARRAY",   /* Array with addresses of init fct */
     "DT_FINI_ARRAY",   /* Array with addresses of fini fct */
     "DT_INIT_ARRAYSZ", /* Size in bytes of DT_INIT_ARRAY */
     "DT_FINI_ARRAYSZ", /* Size in bytes of DT_FINI_ARRAY */
     "DT_RUNPATH",  /* Library search path */
     "DT_FLAGS",    /* Flags for the object being loaded */
};

uint32_t elf_hash(unsigned char *name)
{
    uint32_t   h = 0, g;
    while (*name)
    {
        h = (h << 4) + *name++;
        g = h & 0xf0000000;
        if (g)
            h ^= g >> 24;
        h &= ~g;
    }
    return h;
}

void parse_elf_dynamic(Elf32_Ehdr* ehdr, FILE *elf_fp)
{
    Elf32_Shdr *p_shdr = NULL;
    Elf32_Dyn *p_dyn = NULL;
    Elf32_Phdr *p_phdr = NULL;
    int i = 0;
    uint32_t dynOff = 0, dynSize = 0;
    uint32_t dyn_symtab_off = 0, dyn_strtab_off = 0, dyn_hash_off = 0;
    uint32_t dyn_strsz = 0;

    uint32_t nbucket = 0, nchain = 0;
    uint32_t *bucket = NULL;
    uint32_t *chain = NULL;
    uint32_t str_hash = 0;


    char* dyn_strtab_buff = NULL;
    int str_cnt = 0;

    Elf32_Sym sym_var;
    uint32_t symbol_index = 0;

    if(NULL == ehdr || NULL == elf_fp)
        goto _error;

    /*
    可以通过section和programe来找,但是在运行的时候，需要通过
    programe来找,下面通过section来找的代码注释掉了.
    */


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
        if(PT_DYNAMIC ==  p_phdr[i].p_type)
        {
            //这里静态是说p_offset,运行时需要用p_vaddr
            dynOff = p_phdr[i].p_offset;
            //运行时用p_p_memsz
            dynSize = p_phdr[i].p_filesz;

        }
    }
    /*
    //先解析section的信息,获取.dynamic的偏移和大小
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
        if(SHT_DYNAMIC == p_shdr[i].sh_type)
        {
            dynOff = p_shdr[i].sh_offset;
            dynSize = p_shdr[i].sh_size;
            break;
        }
    }
    */
    if(0 == dynOff || 0 == dynSize)
    {
        printf("get .dynamic section fail\n");
        goto _error;
    }

    printf("\n.dynamic 偏移 0x%x, 大小 0x%x\n", dynOff, dynSize);
    printf("[Nr] %-20s%-12s%-12s\n", "Type", "d_val", "d_ptr");

    fseek(elf_fp, dynOff, SEEK_SET);
    p_dyn = (Elf32_Dyn*)malloc(dynSize);
    if(NULL == p_dyn)
        goto _error;

    if(fread(p_dyn, 1, dynSize, elf_fp) != dynSize)
    {
        printf("read .dynamic section error\n");
        goto _error;
    }
    for(i = 0; i < dynSize /(sizeof(Elf32_Dyn)); i++)
    {
        printf("[%02d]", i);
        if(p_dyn[i].d_tag < 31)
        {
            printf(" %-20s", dynamic_type[p_dyn[i].d_tag]);
        }
        else
        {
            printf(" 0x%-8x          ", p_dyn[i].d_tag);
        }
        printf("0x%-8x  ", p_dyn[i].d_un.d_val);
        printf("0x%-8x  ", p_dyn[i].d_un.d_ptr);

        switch(p_dyn[i].d_tag)
        {
            case DT_HASH:
            dyn_hash_off = p_dyn[i].d_un.d_ptr;
            break;
            case DT_STRTAB:
            dyn_strtab_off = p_dyn[i].d_un.d_ptr;
            break;
            case DT_SYMTAB:
            dyn_symtab_off = p_dyn[i].d_un.d_ptr;
            break;
            case DT_STRSZ:
            dyn_strsz = p_dyn[i].d_un.d_val;
            break;
            default:
            break;
        }

        printf("\n");
    }

    /*
    http://www.sco.com/developers/gabi/latest/ch5.dynamic.html#hash
    A hash table of Elf32_Word objects supports symbol table access.
    The same table layout is used for both the 32-bit and 64-bit file class.
    Labels appear below to help explain the hash table organization,
    but they are not part of the specification.

    nbucket
    nchain
    bucket[0]
    . . .
    bucket[nbucket-1]
    chain[0]
    . . .
    chain[nchain-1]
    The bucket array contains nbucket entries, and the chain array contains nchain entries;
    indexes start at 0. Both bucket and chain hold symbol table indexes.
    Chain table entries parallel the symbol table. The number of symbol table entries should equal nchain;
    so symbol table indexes also select chain table entries.
    A hashing function (shown below) accepts a symbol name and returns a value that may be used to compute a bucket index.
    Consequently, if the hashing function returns the value x for some name, bucket[x%nbucket] gives an index, y,
    into both the symbol table and the chain table. If the symbol table entry is not the one desired,
    chain[y] gives the next symbol table entry with the same hash value.
    One can follow the chain links until either the selected symbol table entry
    holds the desired name or the chain entry contains the value STN_UNDEF.

    unsigned long elf_hash(const unsigned char *name)
    {
        unsigned long   h = 0, g;
        while (*name)
        {
            h = (h << 4) + *name++;
            if (g = h & 0xf0000000)
                h ^= g >> 24;
            h &= ~g;
        }
        return h;
    }
    */

    printf("  Dynamic Hash Table:\n");
    fseek(elf_fp, dyn_hash_off, SEEK_SET);
    //先把nbucket, nchain读出来.
    if(fread(&nbucket, 1, 4, elf_fp) != 4)
    {
        printf("read hash table nbucket fail\n");
        goto _error;
    }
    if(fread(&nchain, 1, 4, elf_fp) != 4)
    {
        printf("read hash table nchain fail\n");
        goto _error;
    }

    printf("nbucket 0x%x, nchain 0x%x\n", nbucket, nchain);

    bucket = (uint32_t*)malloc(nbucket * sizeof(uint32_t));
    chain = (uint32_t*)malloc(nchain * sizeof(uint32_t));
    if(NULL == bucket || NULL == chain)
        goto _error;

    if(fread(bucket, 1, nbucket * sizeof(uint32_t), elf_fp) != nbucket * sizeof(uint32_t))
    {
        printf("read hash table bucket array fail\n");
        goto _error;
    }
    if(fread(chain, 1, nchain * sizeof(uint32_t), elf_fp) != nchain * sizeof(uint32_t))
    {
        printf("read hash table chain array fail\n");
        goto _error;
    }

    //通过hashtable把strtable里的字符串算出在symtable的index，然后打印出地址,大小
    //先把strtable读出来
    dyn_strtab_buff = (char*)malloc(dyn_strsz);
    if(NULL == dyn_strtab_buff)
        goto _error;
    fseek(elf_fp, dyn_strtab_off, SEEK_SET);
    if(fread(dyn_strtab_buff, 1, dyn_strsz, elf_fp) != dyn_strsz)
    {
        printf("read dynstrtab fail\n");
        goto _error;
    }
    i = 0;
    while(i < dyn_strsz)
    {
        //先通过bucket[x]来获取字符串在symbol中的index,如果不对，那就用chain[bucket[x]]
        str_hash = elf_hash((unsigned char*)(dyn_strtab_buff + i));
        symbol_index = bucket[str_hash % nbucket];
        fseek(elf_fp, dyn_symtab_off + symbol_index * sizeof(Elf32_Sym), SEEK_SET);
        if(fread(&sym_var, 1, sizeof(Elf32_Sym), elf_fp) != sizeof(Elf32_Sym))
        {
            printf("read symbol at index %d fail\n", symbol_index);
            goto _error;
        }
        //bucket[x]不是要找的index,然后用chain[bucket[x]]
        if(strcmp(dyn_strtab_buff + i, dyn_strtab_buff + sym_var.st_name) != 0)
        {
            //循环在chain[]里找，直到找到或者chain[x] = 0;
            while(1)
            {
                symbol_index = chain[symbol_index];
                if(symbol_index == 0)
                    break;
                fseek(elf_fp, dyn_symtab_off + symbol_index * sizeof(Elf32_Sym), SEEK_SET);
                if(fread(&sym_var, 1, sizeof(Elf32_Sym), elf_fp) != sizeof(Elf32_Sym))
                {
                    printf("read symbol at index %d fail, index from chain array\n", symbol_index);
                    goto _error;
                }
                if(strcmp(dyn_strtab_buff + i, dyn_strtab_buff + sym_var.st_name) == 0)
                {
                    printf("Off: 0x%08x, size: 0x%08x  %s\n", sym_var.st_value, sym_var.st_size, dyn_strtab_buff + i);
                    break;
                }
            }
        }
        else
        {
            printf("Off: 0x%08x, size: 0x%08x  %s\n", sym_var.st_value, sym_var.st_size, dyn_strtab_buff + i);
        }
        i += strlen(dyn_strtab_buff + i) + 1;
        str_cnt++;
    }
    printf("str_cnt 0x%x\n", str_cnt);


    _error:
    if(p_shdr) free(p_shdr);
    if(p_dyn) free(p_dyn);
    if(p_phdr) free(p_phdr);
    if(bucket) free(bucket);
    if(chain) free(chain);
    if(dyn_strtab_buff) free(dyn_strtab_buff);
    return;
}