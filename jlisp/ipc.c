
/*
    Copyright (c) 1994 Jeff Weisberg

    see the file "License"
*/

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: ipc.c,v 1.1 1995/11/23 04:30:47 jaw Exp jaw $";
#endif

/* first go at some interprocess communication support */

#include <jlisp.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <syscall.h>
#include <malloc.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>

/* must be inet or unix domain socket */
#define JL_AF_UNIX MAKINT(0)
#define JL_AF_INET MAKINT(1)

/* currently only streams are supported */
#define JL_SCK_STREAM	MAKINT(0)
#define JL_SCK_DGRAM	MAKINT(1)

extern int errno;

DEFUN("socketpair", Fsocketpair, Ssocketpair, 0,0,1,0,
      "(socketpair) returns a pair of ports",
      ())
{
	int fd[2];
	FILE *fp[2];
	
	if( socketpair(AF_UNIX, SOCK_STREAM, 0, fd)==-1)
		return MAKINT(errno);

	fp[0] = fdopen(fd[0], "r+");
	setbuf(fp[0], 0);
	fp[1] = fdopen(fd[1], "r+");
	setbuf(fp[1], 0);

	return Fcons( makport(fp[0], READABLE|WRITABLE),
		makport(fp[1], READABLE|WRITABLE));
}

/*
examples:

(open:socket AF_INET SOCK_STREAM "ee.rochester.edu" "nntp")
(open:socket AF_INET SOCK_STREAM INADDR_ANY         "smtp")
(open:socket AF_INET SOCK_DGRAM  "ee.rochester.edu" "nfs" )
(open:socket AF_UNIX SOCK_STREAM "/tmp/socket")
*/

DEFUN("open:socket", Fopen_socket, Sopen_socket, 3,5,1,0,
      "(open:socket domain type address port) create a port for IPC\n"
      " domain is AF_UNIX or AF_INET, type is SOCK_STREAM or SOCK_DGRAM\n"
      " address is hostname (af_inet) or pathname (af_unix)\n"
      " port is the port (string or number) (af_inet), (ignored for af_unix)\n"
      " returns a read/write port (or an error number on failure)", 
      (Obj dom, Obj typ, Obj adr, Obj prt, Obj flg))
{
	int sock, fd, flag, foo;
	FILE *fp;
	struct sockaddr_un saun;
	struct sockaddr_in sain;
	
	switch (dom){

	  case JL_AF_UNIX:
		if( typ!= JL_SCK_STREAM )
			return jlerror(Sopen_socket.name, typ, "invalid type for unix domain socket");
		if(! STRINGP(adr))
			return jlerror(Sopen_socket.name, adr, "WTA: stringp");
		
		strcpy(saun.sun_path, CCHARS(adr));
		saun.sun_family = AF_UNIX;
		sock = socket( AF_UNIX, SOCK_STREAM, 0);
		if( sock==-1)
			return MAKINT(errno);

		foo = 1;
		setsockopt(sock, SOL_SOCKET, SO_DEBUG,     &foo, sizeof(foo));

		if( bind(sock, &saun, strlen(saun.sun_path) + sizeof(saun.sun_family)) ==-1){
			/* assume alreay created -- connect to it */
			if(connect(sock,  &saun, strlen(saun.sun_path) + sizeof(saun.sun_family)) ==-1)
				return MAKINT(errno);
		}else{
			listen(sock, 1);
			fd = accept(sock, 0,0);
			if( fd==-1) return MAKINT( errno );
			sock = fd;
		}
		break;
		
	  case JL_AF_INET:
		if( typ!= JL_SCK_STREAM )
			return jlerror(Sopen_socket.name, typ, "invalid type for inet domain socket");

		sain.sin_family = AF_INET;
		sock = socket( AF_INET, SOCK_STREAM, 0);
		
		if( STRINGP(adr)){
			/* client -- hostname */
			struct hostent *host;
			
			host = gethostbyname( CCHARS(adr));
			if(! host )
				return MAKINT(errno);
			bcopy(host->h_addr, &sain.sin_addr, host->h_length);
			flag = 0;
		}else if( VECTORP(adr)){
			/* client -- ip adr vector of octets */
			int host=0, i, l=CLENGTH(adr);
			for(i=0;i<4; i++){
				host <<= 8;
				if(i<l){
					if(! INUMP(CVECTOR(adr)[i]))
						return jlerror(Sopen_socket.name, adr, "WTA: invalid address");
					host |= CINT( CVECTOR(adr)[i] ) & 0xFF;
				}
			}
			sain.sin_addr.s_addr = htonl( host );

		}else{
			/* server -- inaddr_any */
			sain.sin_addr.s_addr = INADDR_ANY;
			flag = 1;
		}

		if( STRINGP(prt)){
			/* get port no. */
			struct servent *sp;

			sp = getservbyname( CCHARS(prt), 0);
			if(! sp)
				return MAKINT(errno);
			sain.sin_port = sp->s_port;
		}else if( INUMP(prt)){
			sain.sin_port = htons( CINT(prt));
		}else
			return jlerror(Sopen_socket.name, prt, "invalid port");

		
		if( flag ){
			/* server */
			int baz = 1;
			setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &baz, sizeof(baz));
			setsockopt(sock, SOL_SOCKET, SO_DEBUG,     &baz, sizeof(baz));
			
			if(bind(sock, &sain, sizeof(sain))==-1)
				return MAKINT(errno);
			listen(sock, 1);
			foo = sizeof(sain);
			fd = accept(sock, &sain, &foo);
			if(fd==-1) return MAKINT(errno);
			sock = fd;

		}else{
			/* client */
			int baz = 1;
			setsockopt(sock, SOL_SOCKET, SO_DEBUG,     &baz, sizeof(baz));
			if(connect( sock, &sain, sizeof(sain))==-1)
				return MAKINT(errno);

		}
		

		break;
		
	  default:
		return jlerror(Sopen_socket.name, dom, "unknown address family");
	}
	
	fp = fdopen(sock, "r+");
	setbuf(fp, 0);

	return makport( fp, READABLE|WRITABLE );
}

DEFUN("getpeername", Fgetpeername, Sgetpeername, 0,1,1,0,
      "(getpeername [port])...",
      (Obj p))
{
	int fd = 0;
	struct sockaddr_in name;
	int len = sizeof(name);
	Obj v = makvect(5);
	
	if( IOPORTP(p) && (CAR(p) >>14) < 2){
		fd = fileno( CFILEPTR(p));
	}

	getpeername( fd, &name, &len );



	
	return IC_UNDEF;
}



		
