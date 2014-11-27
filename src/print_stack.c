/*
 *--------------------------------------------------------------------------
 *
 * Name           print_stack.c
 *
 *
 * Original:      1993-10-02 Lesley Stoa, CAP Programator AB.
 *		  Email: lesley@sypro.cap.se
 *
 * Description:   Print a stack backtrace in adb format from where the
 *                subrutine is called.
 *
 *
 *-------------------------------------------------------------------------
 */

#include	<stdio.h>
#include	<a.out.h>
#include	<stab.h>
#include	<ctype.h>
#include	<strings.h>
#include	<sys/types.h>
#include	<sys/file.h>
#include	<link.h>

static char     sccsid[] = "@(#)print_stack.c	1.2";

extern char    *index();
extern unsigned long _DYNAMIC[];

struct xsym {
	char           *dat;
	int             lnum;
	int             lnum2;
	int             map;
	struct xsym    *nxt;
};

struct stack_type {
	unsigned long  *regs;
	struct stack_type *next;
	char            funcname[256];	/* name of function */
	char            filename[256];	/* name of source file */
	int             found;	/* 0 if function not found */
	int             filesymbol;	/* symbol # for start of source file */
	int             funcsymbol;	/* symbol # for start of function */
	int             line;	/* line number in source file */
};

struct stack_type *stack_start, *stack_end, *stack_tmp, *stack_tmp2;
char          **argv;
char           *pname;
struct exec     header, lheader;

struct nlist   *symb_list;
int             no_of_symb;

int 
find_ent(adr)
	unsigned int    adr;
{
	int             i, n;

	if (adr > symb_list[no_of_symb - 1].n_value) {
		return no_of_symb - 1;
	}
	if (adr <= symb_list[0].n_value) {
		return 0;
	}
	n = no_of_symb / 2 + 1;
	i = n - 1;
	do {
		if (adr >= symb_list[i].n_value && adr < symb_list[i + 1].n_value) {
			return i;
		}
		if (n > 1) {
			n = n / 2;
		}
		if (adr > symb_list[i].n_value) {
			i += n;
		} else {
			i -= n;
		}
	} while (n > 1 || i >= 0);
	return i >= 0 ? i : 0;
}

asm(".seg \"text\"");
asm(".proc 4");
asm("_getsp: retl");
asm("mov %sp,%o0");
asm(".seg \"data\"");

int 
compar(a, b)
	struct nlist   *a, *b;
{
	unsigned long   i, j;
	i = a->n_value;
	j = b->n_value;
	if (i < j) {
		return -1;
	} else if (i > j) {
		return 1;
	} else {
		return 0;
	}
}

char           *
trb_exefind(pname)
	char           *pname;
{
	FILE           *fp;
	int             ll;
	static char     name[256];
	char           *path;
	char           *ptr, trial[256];
	/*---------------------------------*/
	/* get name of program */
	strcpy(name, pname);

	/* 1: if name contains a slash, assume it is the direct path */
	ptr = strchr(name, '/');
	if (ptr != NULL) {
		fp = fopen(name, "r");
		if (fp != NULL) {
			fclose(fp);
			return name;
		}
	}
	/* 2: look in the user's path */
	ptr = getenv("PATH");
	ll = strlen(ptr);
	path = malloc(ll + 1);
	strcpy(path, ptr);
	/* separate path string into its components */
	ptr = strtok(path, ":");
	while (ptr != NULL) {
		strcpy(trial, ptr);
		strcat(trial, "/");
		strcat(trial, name);
		fp = fopen(trial, "r");
		if (fp != NULL) {
			fclose(fp);
			strcpy(name, trial);
			free(path);
			return name;
		}
		ptr = strtok(NULL, ":");
	}

	/* 3: try the local directory in case it is not in the path */
	strcpy(trial, "./");
	strcat(trial, name);
	fp = fopen(trial, "r");
	if (fp != NULL) {
		fclose(fp);
		strcpy(name, trial);
		return name;
	}
}				/* end of trb_exefind */

void
trb_getline()
{
	FILE           *fp, *fp2;
	struct nlist    symbol;	/* symbol table entry */
	unsigned long   address, match, lastaddress;
	int             type, line, funcsymbol, filesymbol;
	int             i, j, ll, status;
	long            fileaddr, funcaddr, magic;
	int             iff;
	char           *ptr, ttext[256];
	char            funcname[256], filename[256];
	/*----------------------*/
	/* initialize */
	address = 0;
	funcsymbol = 0;
	filesymbol = 0;
	lastaddress = 0xFFFF;

	/* use first file pointer to read symbol table */
	fp = fopen(pname, "r");
	if (fp == NULL) {
		printf("can't open %s\n", pname);
		exit(1);
	}
	/* use second file pointer to read the string table */
	fp2 = fopen(pname, "r");

	/* read all symbols, look at source lines, file & function names */
	fseek(fp, N_SYMOFF(header), 0);
	for (i = 0; i < header.a_syms / sizeof(symbol); i++) {
		status = fread(&symbol, sizeof(symbol), 1, fp);
		if (status == 0)
			printf("could not read symbol\n");
		type = symbol.n_type;
		/* what type of symbol is it */
		if (type == N_SLINE) {	/* src line: 0,,0,linenumber,address */
			line = symbol.n_desc;
			address = symbol.n_value;
		} else if (type == N_FUN) {	/* procedure:
						 * name,,0,linenumber,address */
			funcaddr = symbol.n_un.n_strx;
			address = symbol.n_value;
			funcsymbol = i;
			fseek(fp2, N_STROFF(header), 0);
			fseek(fp2, funcaddr, 1);
			ptr = fgets(ttext, sizeof(ttext), fp2);
			ptr = strtok(ttext, ":");
			ll = strlen(ptr);
			if (ll > 250) {
				strncpy(funcname, ptr, 250);
				funcname[250] = NULL;
			} else
				strcpy(funcname, ptr);
		} else if (type == N_SO) {	/* source file name:
						 * name,,0,0,address */
			fileaddr = symbol.n_un.n_strx;
			address = symbol.n_value;
			filesymbol = i;
			fseek(fp2, N_STROFF(header), 0);
			fseek(fp2, fileaddr, 1);
			ptr = fgets(filename, sizeof(filename), fp2);
		}
		/* does this address match one of ours? */
		stack_tmp = stack_start;
		while (stack_tmp->next) {
			if (!stack_tmp->found) {
				match = stack_tmp->regs[7];
				if (address == match) {
					stack_tmp->found = 1;
					stack_tmp->filesymbol = filesymbol;	/* store symbol# of the
										 * file */
					stack_tmp->funcsymbol = funcsymbol;	/* symbol# of the
										 * function */
					strcpy(stack_tmp->funcname, funcname);
					strcpy(stack_tmp->filename, filename);
					stack_tmp->line = line;
				} else if (address > match && lastaddress < match && lastaddress > (match - 64)) {
					stack_tmp->found = 1;
					stack_tmp->filesymbol = filesymbol;	/* store symbol# of the
										 * file */
					stack_tmp->funcsymbol = funcsymbol;	/* symbol# of the
										 * function */
					strcpy(stack_tmp->funcname, funcname);
					strcpy(stack_tmp->filename, filename);
					stack_tmp->line = line - 1;
				}
			}
			stack_tmp = stack_tmp->next;
		}
		lastaddress = address;
	}			/* end of loop over all nsym */
	fclose(fp2);
	fclose(fp);

}				/* end of trb_getline */

print_stack()
{
	FILE           *lp;
	FILE           *ut;
	FILE           *fdcore, *fdsyms;
	int             fd;

	struct link_map *map_ent;
	long            symoff, stroff;
	int             symsize, i, j;
	unsigned int    adr;
	char            name[256];

	long            fpos;
	int             len;
	unsigned long   getsp(), sp;

	asm("ta 0x83");
	sp = getsp();
	stack_start = stack_end = stack_tmp2 = NULL;
	while (sp) {
		stack_tmp = (struct stack_type *) malloc(sizeof(struct stack_type));
		stack_tmp->regs = (unsigned long *) sp + 8;
		sp = stack_tmp->regs[6];
		stack_tmp->next = NULL;
		stack_tmp->found = 0;
		if (stack_start) {
			stack_end->next = stack_tmp;
			stack_tmp2 = stack_end;
			stack_end = stack_tmp;
		} else {
			stack_start = stack_end = stack_tmp;
		}
	}
	argv = (char **) (stack_tmp2->regs[6] + 100);
	pname = trb_exefind(argv[0]);
	printf("Programname = %s\n", pname);
	if (!symb_list) {
		printf("Reading symbols... ");
		fflush(stdout);
		fdcore = fopen(pname, "r");
		if (fdcore == NULL) {
			fprintf(stderr, "Can't find %s.\n", pname);
			exit(1);
		}
		fdsyms = fopen(pname, "r");
		if (fdsyms == NULL) {
			fprintf(stderr, "Can't find %s.\n", pname);
			exit(1);
		}
		fread(&header, sizeof(struct exec), 1, fdcore);
		symoff = (long) N_SYMOFF(header);
		stroff = (long) N_STROFF(header);
		if (header.a_syms == 0) {
			fprintf(stderr, "Sorry, no symbol table exist. File is striped!\n");
		}
		symsize = header.a_syms / sizeof(struct nlist);
		fseek(fdcore, symoff, 0);
		symb_list = (struct nlist *) malloc(header.a_syms);
		no_of_symb = 0;
		j = 0;
		for (i = 0; i < symsize; i++) {
			fread(&symb_list[no_of_symb], sizeof(struct nlist), 1, fdcore);
			if (symb_list[no_of_symb].n_type == 4 || symb_list[no_of_symb].n_type == 5) {
				fseek(fdsyms, stroff + symb_list[no_of_symb].n_un.n_strx, 0);
				fgets(name, 255, fdsyms);
				len = strlen(name);
				if (len >= 2 && !strcmp(&name[len - 2], ".o")) {
					continue;
				}
				symb_list[no_of_symb].n_un.n_name = (char *) malloc(len + 1);
				if (*name == '_') {
					strcpy(symb_list[no_of_symb].n_un.n_name, &name[1]);
				} else {
					strcpy(symb_list[no_of_symb].n_un.n_name, name);
				}
				no_of_symb++;
				j++;
			}
		}
		printf("%d ", j);
		fflush(stdout);
		symb_list = (struct nlist *) realloc(symb_list, no_of_symb * sizeof(struct nlist));
		fclose(fdcore);
		fclose(fdsyms);

		if (_DYNAMIC != NULL) {
			map_ent = (struct link_map *) _DYNAMIC[9];
		} else {
			map_ent = NULL;
		}
		while (map_ent) {
			fdcore = fopen(map_ent->lm_name, "r");
			if (fdcore == NULL) {
				fprintf(stderr, "Can't find %s.\n", map_ent->lm_name);
				exit(1);
			}
			fdsyms = fopen(map_ent->lm_name, "r");
			if (fdsyms == NULL) {
				fprintf(stderr, "Can't find %s.\n", map_ent->lm_name);
				exit(1);
			}
			fread(&lheader, sizeof(struct exec), 1, fdcore);
			symoff = (long) N_SYMOFF(lheader);
			stroff = (long) N_STROFF(lheader);
			if (lheader.a_syms == 0) {
				fprintf(stderr, "Sorry, no symbol table exist. File is striped!\n");
			}
			symsize = lheader.a_syms / sizeof(struct nlist);
			fseek(fdcore, symoff, 0);
			symb_list = (struct nlist *) realloc(symb_list, no_of_symb * sizeof(struct nlist) + lheader.a_syms);
			j = 0;
			for (i = 0; i < symsize; i++) {
				fread(&symb_list[no_of_symb], sizeof(struct nlist), 1, fdcore);
				if (symb_list[no_of_symb].n_type == 4 || symb_list[no_of_symb].n_type == 5) {
					fseek(fdsyms, stroff + symb_list[no_of_symb].n_un.n_strx, 0);
					fgets(name, 255, fdsyms);
					len = strlen(name);
					if (len >= 2 && !strcmp(&name[len - 2], ".o")) {
						continue;
					}
					symb_list[no_of_symb].n_un.n_name = (char *) malloc(len + 1);
					if (*name == '_') {
						strcpy(symb_list[no_of_symb].n_un.n_name, &name[1]);
					} else {
						strcpy(symb_list[no_of_symb].n_un.n_name, name);
					}
					symb_list[no_of_symb].n_value += (unsigned int) map_ent->lm_addr;
					no_of_symb++;
					j++;
				}
			}
			printf("+ %d ", j);
			fflush(stdout);
			symb_list = (struct nlist *) realloc(symb_list, no_of_symb * sizeof(struct nlist));
			fclose(fdcore);
			fclose(fdsyms);
			map_ent = map_ent->lm_next;
		}

		printf("= %d symbols read.\n", no_of_symb);
		qsort(symb_list, no_of_symb, sizeof(struct nlist), compar);
	}
	trb_getline();
	stack_tmp = stack_start;
	puts("Backtrace:");
	while (stack_tmp->next) {
		i = find_ent(stack_tmp->regs[7]);
		stack_tmp2 = stack_tmp;
		stack_tmp = stack_tmp->next;
		printf("%s(0x%lx,0x%lx,0x%lx,0x%lx,0x%lx,0x%lx) + %x\n",
		       symb_list[i].n_un.n_name,
		       stack_tmp->regs[0],
		       stack_tmp->regs[1],
		       stack_tmp->regs[2],
		       stack_tmp->regs[3],
		       stack_tmp->regs[4],
		       stack_tmp->regs[5],
		       stack_tmp2->regs[7] - symb_list[i].n_value);
		if (stack_tmp2->found) {
			printf("      file %s line %d function %s()\n",
			       stack_tmp2->filename,
			       stack_tmp2->line,
			       stack_tmp2->funcname
				);
		}
	}
	return;

}
