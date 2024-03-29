
/*
    Copyright (c) 1994 Jeff Weisberg

    see the file "License"
*/

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: unistd.c,v 1.1 1995/11/23 04:30:45 jaw Exp jaw $";
#endif

/* did I miss anything useful? */

#include <jlisp.h>

#ifndef NO_UNIX

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <malloc.h>

#ifdef INFERIOR_UNIX
#	define d_fileno d_ino
#endif

extern int errno, etext, edata;

Obj maktimel(time_t t, int m){
	
	Obj foo = Fcons( MAKINT(t>>16),
		Fcons( MAKINT(t&0xFFFF),
		      Fcons( MAKINT(m), IC_NIL)));
	return foo;
}

time_t time_arg(Obj a){
	time_t t = time(0);

	if( INUMP(a)) t = (time_t) CINT(a);
	if( CONSP(a)){
		if( INUMP( CAR(a)))
			t = (time_t)( CINT(CAR(a))<<16);
		else t = 0;
		if( INUMP( CADR(a)))
			t += (time_t)( CINT(CADR(a)));
	}

	return t;
}


DEFUN("current-time", Ftime, Stime, 0,0,1,0,
      "(current-time) return seconds since 00:00:00 GMT, Jan. 1 1970\n"
      "returns a list (hi lo miscrosecs)",
      ())
{
	struct timeval tv;
	
	gettimeofday(&tv, 0);
	return maktimel( tv.tv_sec, tv.tv_usec );
}

DEFUN("current-time-string", Ftimestr, Stimestr, 0,1,1,0,
      "(current-time-string) return a string representing the current time",
      (Obj a))
{
	time_t t = time_arg(a);

	return makstr( ctime(&t) );
}

DEFUN("localtime", Floctime, Sloctime, 0,1,1,0,
      "(localtime [time]) return a vector of time info\n"
      "#(sec min hr mday mon yr wday yday isdst tzone gmtoff)",
      (Obj a))
{

	time_t t = time_arg(a);
	struct tm *stm;
	Obj v;
	
	stm = localtime( &t );

	v = makvect(11);

	CVECTOR(v)[0] = MAKINT( stm->tm_sec );
	CVECTOR(v)[1] = MAKINT( stm->tm_min );
	CVECTOR(v)[2] = MAKINT( stm->tm_hour );
	CVECTOR(v)[3] = MAKINT( stm->tm_mday );
	CVECTOR(v)[4] = MAKINT( stm->tm_mon );
	CVECTOR(v)[5] = MAKINT( stm->tm_year );
	CVECTOR(v)[6] = MAKINT( stm->tm_wday );
	CVECTOR(v)[7] = MAKINT( stm->tm_yday );
	CVECTOR(v)[8] = MAKINT( stm->tm_isdst );
#ifndef INFERIOR_UNIX	
	CVECTOR(v)[9] = makstr( stm->tm_zone );
	CVECTOR(v)[10] = MAKINT( stm->tm_gmtoff );
#endif
	
	return v;
}

Obj stat_lstat_common(struct stat *sb){
	Obj v;

	v = makvect(9);

	CVECTOR(v)[0] = MAKINT( sb->st_ino   );
	CVECTOR(v)[1] = MAKINT( sb->st_mode  );
	CVECTOR(v)[2] = MAKINT( sb->st_nlink );
	CVECTOR(v)[3] = MAKINT( sb->st_uid   );
	CVECTOR(v)[4] = MAKINT( sb->st_gid   );
	CVECTOR(v)[5] = MAKINT( sb->st_size  );
	CVECTOR(v)[6] = maktimel( sb->st_atime,0 );
	CVECTOR(v)[7] = maktimel( sb->st_mtime,0 );
	CVECTOR(v)[8] = maktimel( sb->st_ctime,0 );

	return v;
}


DEFUN("stat", Fstat, Sstat, 1,1,1,0,
      "(stat filename) return a vector conaining file info\n"
      "#(inode mode nlink uid gid size atime mtime ctime)",
      (Obj a))
{

	struct stat sb;
	Obj v;
	
	if( !STRINGP(a))
		return IC_FALSE;

	if( stat( CCHARS(a), &sb))
		return MAKINT( errno );

	return stat_lstat_common( &sb );
}

DEFUN("lstat", Flstat, Slstat, 1,1,1,0,
      "(lstat filename) return a vector conaining file info\n"
      "#(inode mode nlink uid gid size atime mtime ctime)",
      (Obj a))
{

	struct stat sb;
	Obj v;
	
	if( !STRINGP(a))
		return IC_FALSE;

	if( lstat( CCHARS(a), &sb))
		return MAKINT( errno );

	return stat_lstat_common( &sb );
}


DEFUN("chdir", Fchdir, Schdir, 1,1,1,0,
      "(chdir dirname) change the current directory",
      (Obj a))
{

	if( !STRINGP(a))
		return IC_FALSE;

	if( chdir( CCHARS(a)))
		return MAKINT( errno );

	return IC_TRUE;
}

DEFUN("getlogin", Fgetlogin, Sgetlogin, 0,0,1,0,
      "(getlogin ) return the login name",
      ())
{
	return makstr( getlogin() );
}

DEFUN("gethostname", Fgethostn, Sgethostn, 0,0,1,0,
      "(gethostname) return the hostname of the system",
      ())
{
	char buf[256 ];

	gethostname(buf, 256 );
	return makstr( buf );
}

DEFUN("getwd", Fgetwd, Sgetwd, 0,0,1,0,
      "(getwd) return the current working directory",
      ())
{
	char buf[1024];

	getcwd(buf, 1024);
	return makstr( buf );
}

DEFUN("fork", Ffork, Sfork, 0,0,1,0,
      "(fork) fork the process",
      ())
{
	return MAKINT( fork() );
}

DEFUN("vfork", Fvfork, Svfork, 0,0,1,0,
      "(vfork) vfork the process",
      ())
{
	return MAKINT( vfork() );
}

DEFUN("getuid", Fgetuid, Sgetuid, 0,0,1,0,
      "(getuid) return the real user id",
      ())
{
	return MAKINT( getuid());
}

DEFUN("getgid", Fgetgid, Sgetgid, 0,0,1,0,
      "(getgid) return the real group id",
      ())
{
	return MAKINT( getgid());
}
DEFUN("geteuid", Fgeteuid, Sgeteuid, 0,0,1,0,
      "(geteuid) return the effective user id",
      ())
{
	return MAKINT( geteuid());
}

DEFUN("getegid", Fgetegid, Sgetegid, 0,0,1,0,
      "(getegid) return the effective group id",
      ())
{
	return MAKINT( getegid());
}

DEFUN("setuid", Fsetuid, Ssetuid, 1,1,1,0,
      "(setuid n) set the user id",
      (Obj n))
{
	if(! INUMP(n)) return IC_FALSE;
	return setuid( CINT(n)) ? MAKINT( errno ) : IC_TRUE;
}
	
DEFUN("seteuid", Fseteuid, Sseteuid, 1,1,1,0,
      "(setuid n) set the effective user id",
      (Obj n))
{
	if(! INUMP(n)) return IC_FALSE;
	return seteuid( CINT(n)) ? MAKINT( errno ) : IC_TRUE;
}
	
DEFUN("setgid", Fsetgid, Ssetgid, 1,1,1,0,
      "(setgid n) set the group id",
      (Obj n))
{
	if(! INUMP(n)) return IC_FALSE;
	return setgid( CINT(n)) ? MAKINT( errno ) : IC_TRUE;
}
		
DEFUN("setegid", Fsetegid, Ssetegid, 1,1,1,0,
      "(setegid n) set the effective group id",
      (Obj n))
{
	if(! INUMP(n)) return IC_FALSE;
	return setegid( CINT(n)) ? MAKINT( errno ) : IC_TRUE;
}
		
DEFUN("getpid", Fgetpid, Sgetpid, 0,0,1,0,
      "(getpid) return the process id",
      ())
{
	return MAKINT( getpid());
}

DEFUN("getppid", Fgetppid, Sgetppid, 0,0,1,0,
      "(getppid) return the process' parent id",
      ())
{
	return MAKINT( getppid());
}

DEFUN("getgroups", Fgetgroups, Sgetgroups, 0,0,1,0,
      "(getgroups) return a vector of groups in",
      ())
{
	int i;
	gid_t g[ NGROUPS_MAX ];
	Obj v;

	i = getgroups( NGROUPS_MAX, g);
	if(i==-1) return IC_FALSE;
	v = makvect( i );
	for(i--; i>=0; i--){
		CVECTOR(v)[i] = MAKINT( g[i] );
	}

	return v;
}

#ifndef INFERIOR_UNIX
DEFUN("getrusage", Fgetrusg, Sgetrusg, 0,0,1,0,
      "(getrusage) return a vector filled with usage info\n"
      "#(usertime systime maxrss ixrss idrss isrss minor-faults major-faults\n"
      "  n-swaps in-blocks out-blocks msgs-sent msgs-rcvd n-signals n-vol-cntxt\n"
      "  n-invol-cntx data-size text-size stack-size-approx)"
      "[see also: the getrusage(2) man page]",
      ())
{
	Obj v;
	struct rusage ru;

	if( getrusage(0, &ru) )
		return MAKINT( errno );

	v = makvect(19);
	CVECTOR(v)[0] = maktimel(ru.ru_utime.tv_sec, ru.ru_utime.tv_usec );
	CVECTOR(v)[1] = maktimel(ru.ru_stime.tv_sec, ru.ru_stime.tv_usec );
	CVECTOR(v)[2] = MAKINT(ru.ru_maxrss);
	CVECTOR(v)[3] = MAKINT(ru.ru_ixrss);
	CVECTOR(v)[4] = MAKINT(ru.ru_idrss);
	CVECTOR(v)[5] = MAKINT(ru.ru_isrss);
	CVECTOR(v)[6] = MAKINT(ru.ru_minflt);
	CVECTOR(v)[7] = MAKINT(ru.ru_majflt);
	CVECTOR(v)[8] = MAKINT(ru.ru_nswap);
	CVECTOR(v)[9] = MAKINT(ru.ru_inblock);
	CVECTOR(v)[10] = MAKINT(ru.ru_oublock);
	CVECTOR(v)[11] = MAKINT(ru.ru_msgsnd);
	CVECTOR(v)[12] = MAKINT(ru.ru_msgrcv);
	CVECTOR(v)[13] = MAKINT(ru.ru_nsignals);
	CVECTOR(v)[14] = MAKINT(ru.ru_nvcsw);
	CVECTOR(v)[15] = MAKINT(ru.ru_nivcsw);

	CVECTOR(v)[16] = MAKINT( (int)((char*)sbrk(0) - (char*)&etext) );	/* data size */
	CVECTOR(v)[17] = MAKINT( (int)(&etext) );				/* text size (+ 2 pages) */

#ifdef SYS_sstk
	/* use the undocumented sstk syscall */

	CVECTOR(v)[18] = MAKINT( (int)((char*)stackbase - (char*)syscall(SYS_sstk, 0)));
#else	
	CVECTOR(v)[18] = MAKINT( (int)((char*)stackbase - (char*)&v));		/* ~~~ stack size */
#endif
	
	return v;
}
#endif /* INFERIOR_UNIX */

DEFUN("isatty", Fisatty, Sisatty, 1,1,1,0,
      "(isatty port) Is this port a tty?",
      (Obj p))
{
	int t;
	if(! IOPORTP(p)) return IC_FALSE;

	t = SUBPORT(p);		/* make sure it is a file port */
	if( t ) return IC_FALSE;

	t = fileno( CFILEPTR(p));
	return isatty( t ) ? IC_TRUE : IC_FALSE;
}

DEFUN("alarm", Falarm, Salarm, 1,1,1,0,
      "(alarm time) scedule a SIGALARM time seconds from now",
      (Obj t))
{
	int tm;

	if(! INUMP(t)) tm = 0;
	else tm = CINT(t);

	alarm(tm);
	return IC_UNSPEC;
}

DEFUN("sleep", Fsleep, Ssleep, 1,1,1,0,
      "(sleep secs) sleep for a specified number of seconds",
      (Obj s))
{
	int t;
	
	if(! INUMP(s))
		t = 0;
	else
		t = CINT(s);
	sleep( t );
	return IC_UNSPEC;
}

#ifndef INFERIOR_UNIX
DEFUN("usleep", Fusleep, Susleep, 1,1,1,0,
      "(usleep usecs) sleep for a specified number of microseconds",
      (Obj s))
{
	int t;
	
	if(! INUMP(s))
		t = 0;
	else
		t = CINT(s);
	usleep( t );
	return IC_UNSPEC;
}
#endif

DEFUN("kill", Fkill, Skill, 1,2,1,0,
      "(kill proc [signal]) send a process a signal",
      (Obj p, Obj s))
{
	int sig = 15;
	
	if( BOUNDP(s) && INUMP(s)) sig = CINT(s);
	if(! INUMP(p))
		return jlerror(Skill.name, p, "WTA: intp");

	if( kill( CINT(p), sig) ==-1)
		return MAKINT( errno );
	return IC_TRUE;
}

DEFUN("getdirents", Fgetdirent, Sgetdirent, 1,1,1,0,
      "(getdirents dirname) returns a list of entries in the speciified directory\n"
      "of the form: (name inode)",
      (Obj dn))
{
	DIR *dp;
	Obj head=IC_NIL, tail;
	struct dirent *de;
	
	if(! STRINGP(dn))
		return jlerror(Sgetdirent.name, dn, "WTA: stringp");

	if(! (dp=opendir( CCHARS(dn))))
		return jlerror(Sgetdirent.name, dn, "Could not open directory");

	while( de=readdir(dp) ){
		Obj lt = Fcons(makstr(de->d_name),
			Fcons(MAKINT(de->d_fileno), IC_NIL));
		
		if( NULLP(head))
			head=tail = Fcons(lt, IC_NIL);
		else{
			CDR(tail) = Fcons(lt, IC_NIL);
			tail = CDR(tail);
		}
	}
	closedir(dp);
	return head;
}

DEFUN("mkdir", Fmkdir, Smkdir, 1,2,1,0,
      "(mkdir dirname [mode]) create a directory",
      (Obj d, Obj m))
{
	
	if( NBOUNDP(m)) m = MAKINT(0755);
	if(! STRINGP(d) || ! INUMP(m))
		return jlerror(Smkdir.name, Fcons(d, m), "WTA: expected dirname mode");

	if(mkdir(CCHARS(d), CINT(m))==-1)
		return MAKINT(errno);

	return IC_TRUE;
}

DEFUN("unlink", Funlink, Sunlink, 1,1,1,0,
      "(unlink filename) delete the specified file",
      (Obj a))
{

	if(! STRINGP(a))
		return jlerror(Sunlink.name, a, "WTA: stringp");

	if( unlink(CCHARS(a)))
		return MAKINT( errno );
	return IC_TRUE;
}

DEFUN("rmdir", Frmdir, Srmdir, 1,1,1,0,
      "(rmdir dirname) delete the specified directory",
      (Obj a))
{
	
	if(! STRINGP(a))
		return jlerror(Srmdir.name, a, "WTA: stringp");

	if(rmdir(CCHARS(a)))
		return MAKINT( errno );
	return IC_TRUE;
}

DEFUN("umask", Fumask, Sumask, 1,1,1,0,
      "(umask mode) set the umask",
      (Obj a))
{

	if(! INUMP(a))
		return jlerror(Sumask.name, a, "WTA: intp");

	return MAKINT( umask( CINT(a)));
}

Obj getpw_common(struct passwd*pw){
	Obj v;

	if(! pw)
		return MAKINT(errno);
	
	v = makvect(10);
	CVECTOR(v)[0] = makstr(pw->pw_name);
	CVECTOR(v)[1] = makstr(pw->pw_passwd);
	CVECTOR(v)[2] = MAKINT(pw->pw_uid);
	CVECTOR(v)[3] = MAKINT(pw->pw_gid);
#ifdef PASSWD_IS_BIG
	CVECTOR(v)[4] = MAKINT(pw->pw_change);
	CVECTOR(v)[5] = makstr(pw->pw_class);
	CVECTOR(v)[9] = maktimel(pw->pw_expire, 0);
#else
	CVECTOR(v)[4] = MAKINT(0);
	CVECTOR(v)[5] = makstr("");
	CVECTOR(v)[9] = maktimel(0,0);	     
#endif	     
	CVECTOR(v)[6] = makstr(pw->pw_gecos);
	CVECTOR(v)[7] = makstr(pw->pw_dir);
	CVECTOR(v)[8] = makstr(pw->pw_shell);

	return v;
}

Obj get_group_common(struct group * gr){
	Obj v;

	v = makvect(3);

	CVECTOR(v)[0] = makstr(gr->gr_name);
	CVECTOR(v)[1] = makstr(gr->gr_passwd);
	CVECTOR(v)[2] = MAKINT(gr->gr_gid);

	return v;
}
	
DEFUN("getpwnam", Fgetpwnam, Sgetpwnam, 1,1,1,0,
      "(getpwnam name) get pw file info by name",
      (Obj a))
{
	if(! STRINGP(a))
		return jlerror(Sgetpwnam.name, a, "WTA: stringp");

	return getpw_common( getpwnam( CCHARS(a)) );
}

DEFUN("getpwuid", Fgetpwuid, Sgetpwuid, 1,1,1,0,
      "(getpwuid uid) get pw file info by uid",
      (Obj a))
{
	if(! INUMP(a))
		return jlerror(Sgetpwuid.name, a, "WTA: stringp");

	return getpw_common( getpwuid( CINT(a)) );
}

DEFUN("getgrnam", Fgetgrnam, Sgetgrnam, 1,1,1,0,
      "(getgrnam name) get group info by name",
      (Obj a))
{
	if(! STRINGP(a))
		return jlerror(Sgetgrnam.name, a, "WTA: stringp");

	return get_group_common( getgrnam( CCHARS(a)) );
}

DEFUN("getgrgid", Fgetgrgid, Sgetgrgid, 1,1,1,0,
      "(getgrgid gid) get group info by gid",
      (Obj a))
{
	if(! INUMP(a))
		return jlerror(Sgetgrgid.name, a, "WTA: intp");

	return get_group_common( getgrgid( CINT(a)) );
}

DEFUN("system", Fsystem, Ssystem, 1,1,1,0,
      "(system command)  have the system execute command",
      (Obj a))
{

	if(! STRINGP(a))
		return jlerror(Ssystem.name, a, "WTA: stringp");

	return MAKINT( system( CCHARS(a)));
}

DEFUN("wait", Fwait, Swait, 0,0,1,0,
      "(wait) wait for child to exit, returns (pid . status)\n"
      "[see also: waitpid fork]",
      ())
{
	int status,pid;

	pid = wait(&status);

	return Fcons( MAKINT(pid), MAKINT(status));
}

DEFUN("waitpid", Fwaitpid, Swaitpit, 1,2,1,0,
      "(waitpid pid [options]) wait for specified child to exit, returns (pid . status)\n"
      "[see also: wait fork]",
      (Obj p, Obj opt))
{
	int status, pid;
	
	if(! INUMP(p)) p = MAKINT(-1);
	if(! INUMP(opt)) opt = MAKINT(0);
	
	pid = waitpid(CINT(p), &status, CINT(opt));

	return Fcons( MAKINT(pid), MAKINT(status));
}

DEFUN("rename", Frename, Srename, 2,2,1,0,
      "(rename from-file to-file) rename a file\n"
      "[see also: link symlink unlink]",
      (Obj fr, Obj to))
{
	
	if(! STRINGP(fr))
		return jlerror(Srename.name, fr, "WTA: stringp");
	if(! STRINGP(to))
		return jlerror(Srename.name, to, "WTA: stringp");

	if( rename(CCHARS(fr), CCHARS(to))==-1)
		return MAKINT(errno);
	return IC_TRUE;
}

DEFUN("link", Flink, Slink, 2,2,1,0,
      "(link from-file to-file) hard link a file\n"
      "[see also: symlink rename unlinl]",
      (Obj fr, Obj to))
{
	
	if(! STRINGP(fr))
		return jlerror(Slink.name, fr, "WTA: stringp");
	if(! STRINGP(to))
		return jlerror(Slink.name, to, "WTA: stringp");

	if( link(CCHARS(fr), CCHARS(to))==-1)
		return MAKINT(errno);
	return IC_TRUE;
}


DEFUN("symlink", Fsymlink, Ssymlink, 2,2,1,0,
      "(symlink from-file to-file) symbolic link a file\n"
      "[see also: link rename unlink readlink]",
      (Obj fr, Obj to))
{
	
	if(! STRINGP(fr))
		return jlerror(Slink.name, fr, "WTA: stringp");
	if(! STRINGP(to))
		return jlerror(Slink.name, to, "WTA: stringp");

	if( symlink(CCHARS(fr), CCHARS(to))==-1)
		return MAKINT(errno);
	return IC_TRUE;
}

DEFUN("readlink", Freadlink, Sreadlink, 1,1,1,0,
      "(readlink linkname) return the file referenced by the symlink\n"
      "[see also: symlink]",
      (Obj a))
{
	char buf[1024];
	int l;
	
	if(! STRINGP(a))
		return jlerror(Sreadlink.name, a, "WTA: stringp");

	if( (l=readlink(CCHARS(a), buf, 1024))==-1)
		return MAKINT(errno);

	return makstrn(buf, l);
}

DEFUN("chmod", Fchmod, Schmod, 2,2,1,0,
      "(chmod filename mode) change file attributes",
      (Obj f, Obj m))
{

	if(! STRINGP(f))
		return jlerror(Schmod.name, f, "WTA: stringp");

	if(! INUMP(m))
		return jlerror(Schmod.name, m, "WTA: intp");

	if( chmod(CCHARS(f), CINT(m))==-1)
		return MAKINT(errno);

	return IC_TRUE;
}

DEFUN("_quit", F_quit, S_quit, 0,1,1,0,
      "(_quit [val]) quit using _exit",
      (Obj v))
{
	if( INUMP(v))
		_exit( CINT(v));
	else
		_exit(0);

	return IC_UNSPEC;
}

DEFUN("chown", Fchown, Schown, 3,3,1,0,
      "(chown filename uid gid) chown file owner uid and gid",
      (Obj f, Obj u, Obj g))
{
	int uid=-1, gid=-1;
	
	if(! STRINGP(f))
		return jlerror(Schown.name, f, "WTA: stringp");
	if( INUMP(u))
		uid = CINT(u);
	if( INUMP(g))
		gid = CINT(g);

	return chown( CCHARS(f), uid, gid) ? MAKINT( errno ) : IC_TRUE;
}

DEFUN("exec", Fexec, Sexec, 1,1,1,1,
      "(exec file arg1 arg2...) exec a program",
      (Obj argl))
{
	int len = CINT( Flength(argl));
	char** argv = (char**)malloc((len+1) * sizeof(char*));
	Obj arg;
	int i;

	for(i=0; i<len; i++){
		arg = CAR(argl);
		argl = CDR(argl);
		if(! STRINGP(arg)){
			free( argv );
			return jlerror(Sexec.name, arg, "WTA: stringp");
		}

		argv[i] = CCHARS(arg);
	}
	argv[i] = 0;

	execvp(argv[0], argv);
	/* ought not return */

	free( argv );
	return MAKINT( errno );
}

#endif /* NO_UNIX */

      
