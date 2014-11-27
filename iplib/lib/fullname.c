#include <stdio.h>
#define MAXNAMELEN 120
/************************************************
		fullname.c
	takes a filename as an argument,
	and returns the fullname of it
	expanding ~/ to users home dir
	~name/ to names home dir
	and various ~x/ and ~x:1/ to
	the appropriate images directories
	= and + may also be used for ~
	to override the shells (incorrect) globbing

Date: June 1 1991
By:   Jeff Weisberg
*************************************************/


#include <pwd.h>


char *fullname(file)
	char *file;
{

	static char str[MAXNAMELEN];
	char jnk[80],*sp;
	int i,n;

if ( file[0] == NULL ) return (NULL);
if ( (file[0] !='~')&&(file[0] !='+')&&(file[0] !='=') ) return(file);

if ( (file[1]=='/')||(file[1]=='\0') ) {   
	 /* get users home dir. */
	if((sp = (char *)getenv("HOME")) != NULL) {
		strncpy(str, sp, MAXNAMELEN-1);	
		strcat(str,&file[1]);
		strcat(str,"\0");
		return(str);
	}
	return(NULL);
}


if ( (file[2]=='/')||(file[2]=='\0')||(file[2]==':') ){ 
	    /* it is a special macro */
	i=2;
	strcpy(str,"/home/images");

	if (file[2]==':'){
		i=4;
		if (isdigit(file[3]))
			strncat(str,&file[3],1);
	}
		
	switch (file[1]){
	  case 's':
		strcat(str,"/standard");
		break;
	  case 'p':
		strcat(str,"/pavlovic");
		break;
	  case 'w':
		strcat(str,"3/weisberg");
		break;
	  case 't':
		strcat(str,"/tekalp");
		break;
	  case 'a':
		strcat(str,"3/alam");
		break;
	  case 'y':
		strcat(str,"3/yao");
		break;
	  case 'c':
		strcat(str,"/cchen");
		break;
	  case 'e':
		strcat(str,"/cheung");
		break;
	  case 'h':
		strcat(str,"/chang");
		break;
	  case 'r':
		strcat(str,"/riek");
		break;
	  case 'u':
		strcat(str,"/");
		strcat(str,getlogin());
		break;
	  case 'S':
		strcpy(str,"/usr/ceas/common/raster/SunRas");
		break;
 	  case 'G':
		strcpy(str,"/usr/ceas/common/raster/GIF");
		break;
	  case 'R':
		strcpy(str,"/usr/ceas/common/raster");
		break;
	  case 'k':
		strcpy(str,"/home/knuth/weisberg/src/tool/masks");
		break;
	  case 'i':
	  case '1':
		strcpy(str,"/home/orion/weisberg/images");
		break;
	  case '3':
		strcpy(str,"/home/images3/weisberg/ee452");
		break;
	  case '4':
		strcpy(str,"/home/images3/weisberg/ee447");
		break;
	 }

	if((i==4)&&(!isdigit(file[3]))){
		switch(file[3]){
		  case 'g':	/* =2:g  */
			strcat(str,"/gray.test.ras");
			break;
		  case 'c':
			strcat(str,"/color.test.ras");
			break;
		  case 'm':
			strcat(str,"/misc");
			break;
		  case 'h':
			strcat(str,"/mri/hippo_test_1");
			break;
		  case 'r':
			strcat(str,"/mri");
			break;
		  case 'I':
			strcat(str,"/ee447");
			break;
		  case 'M':
			strcat(str,"/ee452");
			break;
		  case 'd':
			strcat(str,"/mri/debug");
			break;
		}
	}
	strcat(str,&file[i]);
	strcat(str,"\0");
	return(str);


}

/* else the next part of the string is a username, look for their home dir in the passwd file */

	/* get the user name */
	i=1;
	while ( (file[i] != '/')&&(file[i] !='\0') ){
		jnk[i-1] = file[i];
		i++;
		if (i>16) return(NULL);
	}
	jnk[i-1]= '\0';
	strcpy(str,getpwnam(jnk)->pw_dir);
	strcat(str,&file[i]);
	return(str);


}/* end of fnc */
