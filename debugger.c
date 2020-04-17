#include "debugger.h"

void menu(void)
{
    puts("\x1B[01;95m");
    puts("░█████╗░░█████╗░░█████╗░██╗░░░░░  ██████╗░███████╗██████╗░██╗░░░██╗░██████╗░░██████╗░███████╗██████╗░");
    puts("██╔══██╗██╔══██╗██╔══██╗██║░░░░░  ██╔══██╗██╔════╝██╔══██╗██║░░░██║██╔════╝░██╔════╝░██╔════╝██╔══██╗");
    puts("██║░░╚═╝██║░░██║██║░░██║██║░░░░░  ██║░░██║█████╗░░██████╦╝██║░░░██║██║░░██╗░██║░░██╗░█████╗░░██████╔╝");
    puts("██║░░██╗██║░░██║██║░░██║██║░░░░░  ██║░░██║██╔══╝░░██╔══██╗██║░░░██║██║░░╚██╗██║░░╚██╗██╔══╝░░██╔══██╗");
    puts("╚█████╔╝╚█████╔╝╚█████╔╝███████╗  ██████╔╝███████╗██████╦╝╚██████╔╝╚██████╔╝╚██████╔╝███████╗██║░░██║");
    puts("░╚════╝░░╚════╝░░╚════╝░╚══════╝  ╚═════╝░╚══════╝╚═════╝░░╚═════╝░░╚═════╝░░╚═════╝░╚══════╝╚═╝░░╚═╝");
    putc(0xa, stdout);
    puts("██████╗░██╗░░░██╗  ██████╗░██╗███╗░░██╗░█████╗░██████╗░██╗░░░██╗");
    puts("██╔══██╗╚██╗░██╔╝  ██╔══██╗██║████╗░██║██╔══██╗██╔══██╗╚██╗░██╔╝");
    puts("██████╦╝░╚████╔╝░  ██████╦╝██║██╔██╗██║███████║██████╔╝░╚████╔╝░");
    puts("██╔══██╗░░╚██╔╝░░  ██╔══██╗██║██║╚████║██╔══██║██╔══██╗░░╚██╔╝░░");
    puts("██████╦╝░░░██║░░░  ██████╦╝██║██║░╚███║██║░░██║██║░░██║░░░██║░░░");
    puts("╚═════╝░░░░╚═╝░░░  ╚═════╝░╚═╝╚═╝░░╚══╝╚═╝░░╚═╝╚═╝░░╚═╝░░░╚═╝░░░");
    putc(0xa, stdout);
    puts("███╗░░██╗███████╗░██╗░░░░░░░██╗██████╗░██╗███████╗");
    puts("████╗░██║██╔════╝░██║░░██╗░░██║██╔══██╗██║██╔════╝");
    puts("██╔██╗██║█████╗░░░╚██╗████╗██╔╝██████╦╝██║█████╗░░");
    puts("██║╚████║██╔══╝░░░░████╔═████║░██╔══██╗██║██╔══╝░░");
    puts("██║░╚███║███████╗░░╚██╔╝░╚██╔╝░██████╦╝██║███████╗");
    puts("╚═╝░░╚══╝╚══════╝░░░╚═╝░░░╚═╝░░╚═════╝░╚═╝╚══════╝\x1B[0m");
    putc(0xa, stdout);

    printf("Hello \x1B[01;95m%s\x1B[0m !!!\nIf it is your first time, type \x1B[01;93mman\x1B[0m\n", username_from_uid(geteuid()));
}

bool disable_aslr(void)
{
    unsigned long pers_value = PER_LINUX | ADDR_NO_RANDOMIZE;

    if (personality(pers_value) < 0)
    {
        if (personality(pers_value) < 0)
            return false;
    }
    return true;
}

char **extract_cmdline_args(int argc, char **argv)
{
    char **args = calloc(argc, sizeof(char *));

    if (!args)
    {
        perror("calloc error: ");
        return NULL;
    }

    args[0] = strdup(argv[1]);

    if (!(*args))
    {
        free(args);
        perror("strdup error: ");
        return NULL;
    }

    for (int i = 2; i < argc; ++i)
    {
        args[i-1] = strdup(argv[i]);
        
        if (!(args[i-1]))
        {
            free(args);
            perror("strdup error: ");
            return NULL;
        }
    }

    args[argc-1] = NULL;
    return args;
}

char *username_from_uid(uid_t uid)
{
    struct passwd *pwd = getpwuid(uid);

    if (!pwd)
    {
        perror("getpwuid error: ");
        exit(EXIT_FAILURE);
    }

    return pwd->pw_name;
}

void *map_file(const char *path, long *length)
{
    FILE *f = fopen(path, "rb");
    int fd = 0;
    void *content;

    if (!f)
    {
        perror("fopen error: ");
        exit(EXIT_FAILURE);
    }

    fd = fileno(f);

    if (fseek(f, 0, SEEK_END) == -1)
    {
        fclose_wrapper(f);
        perror("fseek error: ");
        exit(EXIT_FAILURE);
    }

    *length = ftell(f);

    if (*length == -1)
    {
        fclose_wrapper(f);
        perror("ftell wrapper: ");
        exit(EXIT_FAILURE);
    }

    if (*length == 0)
    {
        fclose_wrapper(f);
        fprintf(stderr, "%s is empty or is a special file...\n", path);
        exit(EXIT_FAILURE);
    }

    rewind(f);

    content = mmap(NULL, *length, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

    if (content == MAP_FAILED)
    {
        fclose_wrapper(f);
        perror("mmap error: ");
        exit(EXIT_FAILURE);
    }

    fclose_wrapper(f);
    return content;
}

void fclose_wrapper(FILE *file)
{
    if (fclose(file) == -1)
    {
        perror("fclose error: ");
        exit(EXIT_FAILURE);
    }
}

void munmap_wrapper(void *content, long size)
{
    if (munmap(content, size) == -1)
    {
        perror("munmap error: ");
        exit(EXIT_FAILURE);
    }
}

void free_wrapper(void *heap_block)
{
    if (heap_block != NULL)
        free(heap_block);
}

short check_type(Elf64_Ehdr *elf_headers)
{
    switch (elf_headers->e_type)
    {
    case ET_EXEC:
    {
        puts("[\x1B[96mLOG\x1B[0m] Executable/static linked format...");
        return 1;
    }
    case ET_DYN:
    {
        puts("[\x1B[96mLOG\x1B[0m] Shared lib/Dynamic linked format...");
        return 2;
    }
    default:
    {
        puts("[\x1B[96mLOG\x1B[0m] Other format...");
        return 3;
    }
    }
}

bool hasSections(Elf64_Ehdr *elf_headers)
{
    if (elf_headers->e_shnum == 0 || elf_headers->e_shstrndx == 0 || elf_headers->e_shoff == 0)
        return false;
    return true;
}

struct breakpoint_t *extract_symbols(Elf64_Ehdr *elf_headers, char *content, long *sym_size)
{
    if (!hasSections(elf_headers))
    {
        puts("[\x1B[01;93mWARNING\x1B[0m] It doesn't have any sections...");
        return NULL;
    }

    char *strtab;
    Elf64_Sym *sym_tab;
    Elf64_Shdr *section_headers = (Elf64_Shdr *)((unsigned char *)elf_headers + elf_headers->e_shoff);
    struct breakpoint_t *symbols = NULL;

    for (int i = 1; i < elf_headers->e_shnum; ++i)
    {
        if (section_headers[i].sh_type == SHT_SYMTAB)
        {
            printf("[+] Found a symbol table at 0x%lx\n[+] End at 0x%lx\n[+] Size: 0x%lx\n", section_headers[i].sh_offset, section_headers[i].sh_offset + section_headers[i].sh_size, section_headers[i].sh_size);

            strtab = &content[section_headers[section_headers[i].sh_link].sh_offset];
            sym_tab = (Elf64_Sym *)(&content[section_headers[i].sh_offset]); // começo da secção
            *sym_size = (long)(section_headers[i].sh_size / sizeof(Elf64_Sym));
            symbols = calloc(*sym_size, sizeof(struct breakpoint_t));

            if (!symbols)
            {
                perror("calloc error: ");
                exit(EXIT_FAILURE);
            }

            for (long j = 0; j < (long)(section_headers[i].sh_size / sizeof(Elf64_Sym)); ++j, ++sym_tab)
            {
                symbols[j].symbol_name = strdup(&strtab[sym_tab->st_name]);
                symbols[j].breakpoint = 0;
                symbols[j].hit = 0;
                symbols[j].addr = sym_tab->st_value;
            }

            puts("[\x1B[96mLOG\x1B[0m] Symbol table parsed...");
            return symbols;
        }
    }

    puts("[\x1B[01;93mWARNING\x1B[0m] It doesn't have symbols, maybe it is stripped...");
    return NULL;
}

long find_symbol_addr(struct breakpoint_t *file_symbols, long symtab_size, const char *symbol)
{
    for (long i = 0; i < symtab_size; ++i)
        if (strcmp(file_symbols[i].symbol_name, symbol) == 0)
            return file_symbols[i].addr;
    return -1;
}

void display_simbols(long symtab_size, struct breakpoint_t *file_symbols)
{
    if (symtab_size == 0)
    {
        puts("\x1B[01;93mNo symbol table...\x1B[0m");
        return;
    }
    for (long i = 0; i < symtab_size; ++i)
        printf("Symbol (\x1B[96m%ld\x1B[0m) => \x1B[01;91m%s\x1B[0m at \x1B[32m0x%lx\x1B[0m\n", i, file_symbols[i].symbol_name, file_symbols[i].addr);
}

long get_base(struct breakpoint_t *file_symbols, pid_t pid)
{
    char path[PROCS_LENGTH] = {'\0'}, buffer[200] = {'\0'}, tmp_base[15] = {'\0'};
    unsigned long base = 0;
    int i = 0;
    size_t read = 0;

    sprintf(path, "/proc/%d/maps", pid);

    FILE *handler = fopen(path, "r");

    if (!handler)
    {
        free_wrapper(file_symbols);
        perror("fopen error: ");
        exit(EXIT_FAILURE);
    }

    read = fread(buffer, sizeof(char), 200, handler);

    if (read == (size_t)-1)
    {
        free_wrapper(file_symbols);
        fclose_wrapper(handler);
        perror("fread error: ");
        exit(EXIT_FAILURE);
    }

    for (char *tmp = buffer; *tmp != '\0' && *tmp != '-'; tmp++)
        tmp_base[i++] = *tmp;

    base = strtol(tmp_base, NULL, 16);
    fclose_wrapper(handler);
    return base;
}

void check_aslr(struct breakpoint_t *file_symbols)
{
    int persona = personality(CURRENT_PERSONA);

    if (persona == -1)
    {
        free_wrapper(file_symbols);
        perror("personality error: ");
        exit(EXIT_FAILURE);
    }

    if ((persona & ADDR_NO_RANDOMIZE) != 0)
    {
        puts("aslr is \x1B[31mdisabled\x1B[0m");
        return;
    }

    puts("aslr is \x1B[32menabled\x1B[0m");
}

void sep_tokens(char *tokens, char **args)
{
    int i = 0;

    while (tokens != NULL)
    {
        args[i] = tokens;
        i++;
        tokens = strtok(NULL, "\n");
    }
}

void patch_regs(pid_t pid, struct user_regs_struct *old_registers, struct breakpoint_t *file_symbols)
{
    struct user_regs_struct tmp_regs;

    if (ptrace(PTRACE_GETREGS, pid, NULL, &tmp_regs) == -1)
    {
        free_wrapper(file_symbols);
        perror("ptrace GETREGS error: ");
        exit(EXIT_FAILURE);
    }

    tmp_regs = *old_registers;

    if (ptrace(PTRACE_SETREGS, pid, NULL, &tmp_regs) == -1)
    {
        free_wrapper(file_symbols);
        perror("ptrace SETREGS error: ");
        exit(EXIT_FAILURE);
    }
}

long set_breakpoint(pid_t pid, long addr, struct breakpoint_t *file_symbols)
{
    long ptrace_res = ptrace(PTRACE_PEEKTEXT, pid, (void *)addr, NULL);

    if (ptrace_res == -1)
    {
        free_wrapper(file_symbols);
        perror("Ptrace PEEKTEXT error: ");
        exit(EXIT_FAILURE);
    }

    unsigned long long trap = (ptrace_res & ~0xff) | 0xcc;

    if (ptrace(PTRACE_POKETEXT, pid, (void *)addr, trap) == -1)
    {
        free_wrapper(file_symbols);
        perror("Ptrace POKETEXT error: ");
        exit(EXIT_FAILURE);
    }

    return ptrace_res;
}

void store_breakpoint(struct breakpoint_t *breakpoint_list, long breakpoint, long addr)
{
    for (short i = 0; i < MAX_BREAKPOINTS; ++i)
    {
        if (i == (MAX_BREAKPOINTS - 1) && breakpoint_list[i].addr != 0 && breakpoint_list[i].breakpoint != 0)
        {
            puts("[\x1B[01;93mWarning\x1B[0m] The breakpoint list is full...");
            break;
        }

        if (breakpoint_list[i].addr == 0 && breakpoint_list[i].breakpoint == 0)
        {
            breakpoint_list[i].addr = addr;
            breakpoint_list[i].breakpoint = breakpoint;
            break;
        }
    }
}

bool resume_execution(pid_t pid, struct user_regs_struct *regs, struct breakpoint_t *breakpoins_list, struct breakpoint_t *file_symbols)
{
    struct breakpoint_t tmp;
    short which = 0;
    memset(&tmp, 0, sizeof(struct breakpoint_t));

    for (short i = 0; i < MAX_BREAKPOINTS; ++i)
    {
        if ((regs->rip - 1) == (unsigned long long)(breakpoins_list[i].addr))
        {
            tmp = breakpoins_list[i];
            breakpoins_list[i].hit++;
            which = i;
            break;
        }
    }

    if ((tmp.addr == 0 && tmp.breakpoint == 0) || tmp.hit > 0)
        return false;

    printf("[\x1B[96mBREAKPOINT\x1B[0m] Breakpoint \x1B[01;91m(%d)\x1B[0m hit at \x1B[01;90m0x%lx\x1B[0m\n", which, tmp.addr);

    if (ptrace(PTRACE_POKETEXT, pid, (void *)tmp.addr, tmp.breakpoint) == -1)
    {
        free_wrapper(file_symbols);
        perror("ptrace POKETEXT error: ");
        exit(EXIT_FAILURE);
    }

    regs->rip = tmp.addr;
    patch_regs(pid, regs, file_symbols);

    if (ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL) == -1)
    {
        free_wrapper(file_symbols);
        perror("ptrace CONT error: ");
        exit(EXIT_FAILURE);
    }

    return true;
}

void display_breakpoints(struct breakpoint_t *breakpoint_list)
{
    if (breakpoint_list[0].addr == 0 && breakpoint_list[0].breakpoint == 0)
    {
        puts("\x1B[01;93mThe breakpoint list is empty...\x1B[0m");
        return;
    }

    for (short i = 0; i < MAX_BREAKPOINTS; ++i)
    {
        if (breakpoint_list[i].addr == 0 && breakpoint_list[i].breakpoint == 0)
            break;
        printf("Breakpoint (\x1B[96m%d\x1B[0m) at \x1B[32m0x%lx\x1B[0m\n", i, breakpoint_list[i].addr);
    }
}

void copy_registers(unsigned long long *regs_cpy, struct user_regs_struct *original_regs)
{
    unsigned long long *ptr = &original_regs->r15; // first field of the struct
    for (int i = 0; i < USER_REGS_STRUCT_NO; ++i)
        regs_cpy[i] = *ptr++;
}

void modify_regs(unsigned long long *regs_cpy, struct user_regs_struct *new_regs)
{
    unsigned long long *ptr = &new_regs->r15;
    for (int i = 0; i < USER_REGS_STRUCT_NO; ++i)
        *ptr++ = regs_cpy[i];
}

void format_print(struct user_regs_struct *new_regs, struct user_regs_struct *saved, const char **registers)
{
    puts("\n\x1B[01;93mRegisters:\x1B[0m");
    printf("RAX: 0x%llx\nRBX: 0x%llx\nRCX: 0x%llx\nRDX: 0x%llx\nRSP: 0x%llx\nRBP: 0x%llx\nRSI: 0x%llx\nRDI: 0x%llx\nR8:  0x%llx\nR9:  0x%llx\nR10: 0x%llx\nR11: 0x%llx\nR12: 0x%llx\nR13: 0x%llx\nR14: 0x%llx\nR15: 0x%llx\n",
           new_regs->rax, new_regs->rbx, new_regs->rcx, new_regs->rdx, new_regs->rsp,
           new_regs->rbp, new_regs->rsi, new_regs->rdi, new_regs->r8,
           new_regs->r9, new_regs->r10, new_regs->r11, new_regs->r12, new_regs->r13,
           new_regs->r14, new_regs->r15);

    unsigned long long *ptr = &saved->r15;
    unsigned long long *ptr2 = &new_regs->r15;

    puts("\n\x1B[01;93mLast changes:\x1B[0m");

    for (int i = 0; i < USER_REGS_STRUCT_NO; ++i)
    {
        if (*ptr != *ptr2)
            printf("[\x1B[01;91m%s\x1B[0m] \x1B[31m0x%llx\x1B[0m => \x1B[32m0x%llx\x1B[0m\n", registers[i], *ptr, *ptr2);
        ptr++;
        ptr2++;
    }

    putc(0xa, stdout);
}