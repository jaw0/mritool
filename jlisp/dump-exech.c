
/*
    dump out the exec header
*/    


#include <stdio.h>
#include <sys/exec.h>


main(int argc, char**argv){
	FILE *fp;
	struct exec ex;
	char *foo;
	
	if( argc!=2)
		exit(-1);

	fp = fopen(argv[1], "r");
	
	fread(&ex, sizeof( struct exec ), 1, fp);

	if( ex.a_dynamic)
		puts("Dynamically linked");
	else
		puts("Statically linked");
	printf("TVer:\t%d\n", ex.a_toolversion);
	
	switch( ex.a_machtype){
	  case M_OLDSUN2:
		foo = "Sun 2"; break;
	  case M_68010:
		foo = "Sun 2/3"; break;
	  case M_68020:
		foo = "Sun 3"; break;
	  case M_SPARC:
		foo = "Sun 4"; break;
	  default:
		foo = 0;
	};
	printf("Mach:\t%s\n", foo);

	
	switch( ex.a_magic){
	  case OMAGIC:
		foo = "Old impure format (OMAGIC)"; break;
	  case NMAGIC:
		foo = "Read-only text (NMAGIC)"; break;
	  case ZMAGIC:
		foo = "Demand load format (ZMAGIC)"; break;
	  default:
		foo = "Unknown";
	}

	printf("Exec:\t%s\n", foo);

	printf("Text:\t%x\n", ex.a_text);
	printf("Data:\t%x\n", ex.a_data);
	printf("Bss:\t%x\n", ex.a_bss);
	printf("Syms:\t%x\n", ex.a_syms);
	printf("Entry:\t%x\n", ex.a_entry);
}
