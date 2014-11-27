

main(int argc, char**argv){
	int i=0;
	int *pi = &i;

	if(argc>1)
		i = atoi(argv[1]);

	pi = (int*)((int)pi + i);
	*pi = i;

	exit(0);
}
