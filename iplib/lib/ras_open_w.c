FILE *
ras_open_w(name)
	char *name;
{
	FILE *fp;

if ( name[0] == '-' ) return(stdout);
if( (fp=fopen(name,"w"))==NULL){
	fprintf(stderr,"Could not open %s for output\r\n",name);
	exit(-1);
}
return(fp);
