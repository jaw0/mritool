// @(#)pipebuf.C	1.15 
/*
 * pipebuf.c
 *   by Bill Dimm   June, 1991
 */

/* #define options:
 *   If your computer has a waitpid() function, you should define WAITPID
 *   If it does not have waitpid(), but does have wait4(), you should define WAIT4
 *   If your computer has neither and doesn't have a suitable substitute, you should
 *   define KILL0, but be warned that the stat variable will not get set. 
 */

/* COMMENT: On some machines (eg Sun), when read() or write() gets
 * the wrong number of characters, errno becomes non-zero.  Since
 * we are in O_NDELAY mode, this does not necessarily indicate an
 * error for us.  So, I reset errno after read()/write() 
 */

#include "pstream.h"
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#if defined(_IBMR2)
#  include <sys/mode.h>
#  include "osfcn.h"
#else
#  include <osfcn.h>
#endif
#include <sys/types.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <memory.h>
#include <signal.h>
  // apparently this is not defined in any of the header files 
#ifndef NOUSLEEP
extern "C"
	{
	int usleep(unsigned int us);
	}
#endif

#undef NULL
#define NULL 0
  // DEFAULTBUFSIZE must be at least 2
#define DEFAULTBUFSIZE	1024
  // Minimum amount of putback allowed with a large buffer
#define MINPUTBACKSPACE	4

  /* CHKCHILD must be defined so that it invokes some system function which returns the
   * pid of the child if it terminated, and returns 0 if it has not terminated.  The
   * function must not block (ie. it must return immediately) and it SHOULD store 
   * the status information (as is done by wait()) in _statloc which can be cast
   * to int*.  On machines which don't have a suitable function, KILL0 may
   * be defined - it will check to see if child is dead, but won't fetch the status -
   * this is a last resort.
   */

#ifdef _IBMR2
#  ifndef WAITPID
#    define WAITPID
#  endif
#endif

#if defined(WAITPID)
#  define CHKCHILD(_pid,_statloc) waitpid((_pid),(int *)(_statloc),WNOHANG)
#endif

#if defined(WAIT4)
#  include <sys/time.h>
#  include <sys/resource.h>
#  ifdef WAIT4_INTPTR
#    define CHKCHILD(_pid,_statloc) wait4((_pid),(int *)(_statloc),WNOHANG,NULL)
#  else
#    define CHKCHILD(_pid,_statloc) wait4((_pid),(union wait *)(_statloc),WNOHANG,NULL)
#  endif
#endif

  // Warning: this is really not a good way to do it... stat will not get set
  // Only use this as a last resort
#if defined(KILL0)
#  define CHKCHILD(_pid,_statloc) ( kill((_pid),0) ? (_pid) : 0 )
#endif

void pipebuf::init()
	{
	output_isopen = 0;
	input_isopen = 0;
	child_isdead = -1;
	child_stat = -1;
	child_pid = -1;
	fdin = -1;
	fdout = -1;
	mode = 0;
	usleeptime = 0;
		/* Although pipebuf can act unbuffered, it always has a buffer (although it may be very small)
		 * which it uses.  So, we set make the unbuffered flag 0 to ensure that we always have a buffer
		 * allocated (even if we are behaving in an unbuffered manner) */
	unbuffered(0);  
	}


void pipebuf::updatechildstate()
	{
	int stat;
	int storeerrno;

	storeerrno = errno; /* avoid screwing up errno using CHKCHILD */
		// We don't touch child_stat or child_isdead unless child's state has changed
	if (child_isdead)
		return;  // Child was found to be dead by a previous call
	if (CHKCHILD(child_pid,&stat) == child_pid)
		{  // child is dead
		child_isdead = 1;
		  // 'stat' will be 0 if child exited without problems.  If non-zero, may be exit value, signal which killed it, etc.
		child_stat = stat; // NOTE: Must fiddle with bits to get the exit value of child - see wait() for info on 'stat'
		}
	errno = storeerrno;
	}
	

  /* omode is used to tell it which file descriptor it should check.  If omode is 0, it
   * uses 'mode'.  However, you must be careful to avoid ambiguities - using an
   * omode of input|ouput will cause it to fail, returning -1 (this is why 
   * defaulting is not allowed for a pstream - it is typically opened in input|output
   * mode).  */
int pipebuf::bytesinpipe(int omode)
	{
	int fd;
	struct stat statbuf;

	if (omode == 0)
		omode = mode;  // default

	if ((omode & ios::in) && (omode & ios::out))
		return -1;  // must specify a single file descriptor

	if (omode & ios::in)
		if (!input_isopen)
			return -1;
		else
			fd = fdin;
	else if (omode & ios::out)
		if (!output_isopen)
			return -1;
		else
			fd = fdout;
	else
		return -1;  // User didn't give input or output!

	if (fstat(fd,&statbuf))
		return -1;  // stat failed
	return statbuf.st_size;
	}


  /* This allows you to close a file descriptor without shutting things down
   * completely - useful if the child expects to see EOF on its input
   * before it produces output */
pipebuf *pipebuf::close(int cmode)
	{
	if (cmode & ios::in)
		if (fdin != -1)
			{
			::close(fdin);
			fdin = -1;
			input_isopen = 0;
			mode &= ~ios::in;
			setg(0,0,0);  // clear the get buffer - lose the contents
			}
	if (cmode & ios::out)
		{
		overflow(EOF); // try to write out chars remaining in put buffer
		if (fdout != -1)
			::close(fdout);
		fdout = -1;
		output_isopen = 0;
		mode &= ~ios::out;
		setp(0,0); // clear the put buffer
		}
	return this;
	}


  /* close() leaves child_pid, child_stat, and child_isdead in tact.
   * This is done so that the user may close the file descriptors, but
   * still check on the status of the process later (ie. determine whether
   * or not closing its input causes it to die) */
pipebuf *pipebuf::close()
	{
	close(ios::in|ios::out);
	mode = 0;  // To make sure 'noshell' bit gets removed too 
	return this;
	}


  /*
   * only omode flags which are not ignored are: ios::in, ios::out, pipebuf::noshell
   *
   * Note: I guess I really ought to check to see if dup fails, but I don't...
   */
pipebuf *pipebuf::open(const char *cmd, int omode)
	{
	int fd_readset[2];  // our program will read from one end of pipe & cmd writes to other end 
	int fd_writeset[2];  // our program will write to one end of pipe & cmd reads from other end

	if (input_isopen || output_isopen)
		return NULL;

		// set fd's which are not yet defined to -1
	fd_readset[0] = -1;
	fd_readset[1] = -1;
	fd_writeset[0] = -1;
	fd_writeset[1] = -1;

		// Open the pipe(s)
	if (omode & ios::in)
		{
		if (pipe(fd_readset))
			return NULL;  // failed to open pipe 
		  // set read end of pipe to nonblocking, and set 'close on exec' flag so child won't hang the pipe
		if (fcntl(fd_readset[0],F_SETFL,O_NDELAY) || fcntl(fd_readset[0],F_SETFD,1))
			{ // fcntl failed
			::close(fd_readset[0]);
			::close(fd_readset[1]);
			return NULL;
			}
		fdin = fd_readset[0];
		}
	if (omode & ios::out)
		{
		if (pipe(fd_writeset))
			return NULL;  // failed to open pipe 
		  // set write end of pipe to nonblocking, and set 'close on exec' flag so child won't hang the pipe
		if (fcntl(fd_writeset[1],F_SETFL,O_NDELAY) || fcntl(fd_writeset[1],F_SETFD,1))  
			{ // fcntl failed
			::close(fd_writeset[0]);
			::close(fd_writeset[1]);
			if (fd_readset[0] != -1)
				{
				::close(fd_readset[0]);
				::close(fd_readset[1]);
				}
			return NULL;
			}
		fdout = fd_writeset[1];
		}

	  /* Here comes the fork. */

	child_pid = fork();
	if (child_pid == -1)
		{   // fork failed 
		  // close fd's which child would read/write from
		if (fd_readset[1] != -1)
			::close(fd_readset[1]);
		if (fd_writeset[0] != -1)
			::close(fd_writeset[0]);
		close();  // this will handle the the fd's which parent would read/write from
		return NULL;
		}

	if (child_pid == 0)
		{  // this is the child 

		  // create stdin for child
		if (fd_writeset[0] != -1)
			{
			::close(0);
			dup(fd_writeset[0]);  // Use read end of pipe which parent is writing to as stdin
			::close(fd_writeset[0]);
			}

		  // create stdout for child
		if (fd_readset[1] != -1)
			{
			int dupresult;
			::close(1);
			dupresult = dup(fd_readset[1]);
			if (dupresult == 0)
				{  // Bizarre user has written a program with no stdin!
				dup(fd_readset[1]);
				::close(0); // If the user wants no stdin, we'll let him have it...
				}
			::close(fd_readset[1]);
			}

		  /* Now, we ought to close all of the unneeded file descriptors.  It is crucial that the child not
		   * keep the parent's part of the pipe open.  We took care of that by setting the 'close on exec'
		   * flag for the appropriate pipe fd's.  But, what do we do about the other files that may be open?
		   * On the IBMs it is easy to close them, but what do we do for the Suns?  For the sake of 
		   * consistency, I will leave them open on all machines - user can set FD_CLOEXEC if this
	  	   * really bothers him.  */
/*  This could be used to close all fd's >2 on IBMs
#ifdef _IBMR2
		fcntl(3,F_CLOSEM,3);  // WARNING: IBM's docs are wrong - FileDescriptor, not Argument, is the limiting (inclusive) fd
#endif
*/

		signal(SIGPIPE,SIG_DFL);  // Do this so child dies when trying to write to dead parent
		if (omode & pipebuf::noshell)
			execlp(cmd,cmd,NULL);
		else
#ifdef _IBMR2
			execlp("bsh","bsh","-c",cmd,NULL);
#else
			execlp("sh","sh","-c",cmd,NULL);
#endif

		exit(127);  // we only get here if exec fails
		}

	  // this is the parent 
	
	if (omode & ios::out)
		signal(SIGPIPE,SIG_IGN);  // if we don't do this, our process is killed when hit end of pipe

	  // close fd's which child reads/writes from (parent doesn't need them) 
	if (fd_readset[1] != -1)
		::close(fd_readset[1]);
	if (fd_writeset[0] != -1)
		::close(fd_writeset[0]);

	if (omode & ios::in)
		input_isopen = 1;
	if (omode & ios::out)
		output_isopen = 1;
	child_stat = -1;  // A meaningful value will be set when child exits
	child_isdead = 0;  // not dead yet
	setp(0,0);  // reset buffers 
	setg(0,0,0);
	mode = omode;
	return this;
	}


  // Note: default to read buffer of size which is half (about) of total buffer size
streambuf *pipebuf::setbuf(char *b, int len)
	{
	int rdlen;

	rdlen = len / 2;
	if (rdlen < 1)
		rdlen = 1;
	return setbuf(b,len,rdlen);
	}


streambuf *pipebuf::setbuf(char *b, int len, int rdlen)
	{
	if ((input_isopen || output_isopen) && ( (gptr() && gptr() < egptr()) || (pptr() && pptr() > pbase()) ) )
		return NULL; // buffer not empty - must empty buffer first if calling when pipe is open

	if (b == NULL || len < sizeof(smallbuf))
		{ // This is the "unbuffered" mode - still need 1 byte for put buffer, and 1 byte for get buffer
		b = smallbuf;  
		len = 2;
		rdlen = 1;
		}

	  // Save at least one byte for get buffer, and one byte for put buffer
	  // since we don't necessarily know what mode the user will open in
	if (rdlen > (len - 1))
		sizeofgetbuf = len - 1; // Save a byte for put buffer
	if (rdlen <= 0)
		sizeofgetbuf = 1;  // Need at least one byte
	else
		sizeofgetbuf = rdlen;


	setb(b,b+len,0); // The 0 tells it not to 'delete' the buffer
	setp(0,0);  // initialize put buffer to be empty 
	setg(0,0,0); // init get buf to be empty 
	return this;
	}


int pipebuf::doallocate()
	{
	char *buffer;

	buffer = new char[DEFAULTBUFSIZE];
	if (buffer == NULL)
		return EOF;
	sizeofgetbuf = DEFAULTBUFSIZE / 2;
	if (sizeofgetbuf < 1)
		sizeofgetbuf = 1;
	setb(buffer,buffer+DEFAULTBUFSIZE,1); // WARNING: They use 'delete' rather than 'delete[]' to free this - potential disaster
	return 0;
	}


int pipebuf::pbackfail(int c)
	{
	return EOF; // can't put a char back into pipe
	}


streampos pipebuf::seekpos(streampos sp, int i)
	{
	return EOF; // no seeks with pipes 
	}


#ifdef OLDCC
streampos pipebuf::seekoff(streamoff so, seek_dir sd, int omode)
#else
  // cfront 2.0 won't take the line below
streampos pipebuf::seekoff(streamoff so, ios::seek_dir sd, int omode)
#endif
	{
	return EOF;  // no seeks with pipes 
	}


int pipebuf::sync()
	{
	int result = 0;

	  // Can't sync if not open, or if child is dead
	if ( !(input_isopen || output_isopen) )
		return EOF;
	if (childisdead()) 
		return EOF; // this is neccessary because close() calls sync() before setting isopen

	if (pptr() && pptr() > pbase())
		if (overflow(EOF) != EOF) // flush the output
			result = 0;
		else
			result = EOF; 
	if (gptr() && egptr() > gptr())
		result = EOF;  // can't put characters back into the pipe 
	return result;  
	}

  /* Note: characters which are sitting in the get buffer are not disturbed by overflow()
   * since we can't do a putback with a pipe. */
int pipebuf::overflow(int ch)
	{
		// initial check for errors 
	if (!output_isopen)
		return EOF;
	if (allocate() == EOF)
		return EOF;
	if (!(mode & ios::out))  
		return EOF;

	if (!pptr())
		setp(base()+sizeofgetbuf,ebuf()-1);  /* create put buffer if there wasn't one already - see comment 
		                                      * below for reason that I am using ebuf()-1, rather than ebuf() */

	int chars_in_putbuf;
	chars_in_putbuf = pptr() - pbase();

	  /* We intentionally left an extra space at the end of the put buffer so that we
	   * can jam this extra ch onto the end, thus allowing us to put this character out
	   * right away instead of hanging onto it until the next time overflow() is called.  Thus
	   * we accomplish unbuffered() output without considering it as a special case (the buffer
	   * size is simply set to 1) - I am following the example on p 3-26 of the Library Manual */
	if (ch != EOF)
		{
		*pptr() = (char)ch;
		++chars_in_putbuf;
		}

		// flush out whatever characters are currently in the put buffer 
	register int amt_written;
	register char *p;

	p = pbase();
	int storeerrno;
	while (chars_in_putbuf > 0)
		{
		if (childisdead())
			return EOF;  // Nothing left to write to

		  // We don't want the user to see all of the (system dependent) errnos generated by writing to empty pipe
		storeerrno = errno;
		amt_written = write(fdout,p,chars_in_putbuf);
		errno = storeerrno;

		if (amt_written != chars_in_putbuf)
			{
			int storeerrno;

			storeerrno = errno; // errno will get set to 4 on the Suns when usleep is halted
#ifndef NOUSLEEP
			if (usleeptime > 0)
				usleep(usleeptime);  // wait a while before trying to put the rest out
#endif
			errno = storeerrno;
			}
		if (amt_written < 0)
			amt_written = 0;  // don't want to add -1 to our counters
		p += amt_written;
		chars_in_putbuf -= amt_written;
		}

		// reset buffers 
	setp(base()+sizeofgetbuf,ebuf()-1);  // we leave one char beyond end of put buffer so can spill over by one 
	return 0;  // it says "return some value other than EOF" 
	}


  /* Note: When underflow is called to read characters, and there are characters in
   * the put buffer, we output them before reading.  This is done for two
   * reasons:
   *   1. Because we can (we can't do a similar thing when we have chars in the get buffer - pipes don't allow putback).
   *   2. The process which we are expecting to get input from may not send it's input until we have sent it
   *      all that we need to, so we must output all of the bytes which the user thinks have been output before
   *      doing a read.  
   */
int pipebuf::underflow()
	{
		// initial check for errors 
	if (!input_isopen)
		return EOF;
	if (allocate() == EOF)
		return EOF;
	if (!(mode & ios::in))  
		return EOF;
	if (gptr() && gptr() < egptr())
		return *gptr();  // sbuf.prot(3C++) says we should do this, but nobody else does...
	if (pptr() && pptr() > pbase())  // output buffer has characters - must get rid of them 
		if (overflow(EOF) == EOF)  
			return EOF; // Some error occured while trying to output chars

	register char *start_of_chars;
	register int amt_read;
	int chars_to_read, putbackspace;

	  /* eback() is the lowest spot where a char CAN be stored,
	   * gptr() is the lowest spot where a char IS stored (this is the
	   * oldest char),
	   * and egptr() points just past the highest stored char (this is
	   * the newest char).
	   * Ideally, we would like to have the reserve area filled, all
	   * except the lowest byte.  That way, we guarantee one character
	   * of putback.  However, our read() may give fewer chars than
	   * we ask for, so we just set egptr one past the last char we got.
	   * Also, if our buffer has size 1, we don't have room to reserve
	   * a byte for a putback */

	putbackspace = MINPUTBACKSPACE;
	if (sizeofgetbuf == 1)
		putbackspace = 0;
	else if (2*MINPUTBACKSPACE > sizeofgetbuf)
		putbackspace = 1;

	chars_to_read = sizeofgetbuf - putbackspace;
	start_of_chars = base() + putbackspace;

	int secondtry = 0; // true when it is trying to read after the child has died, and it has failed once already
	int storeerrno;

	do
		{
			// We don't want the user to see the (system dependent) errnos resulting from reading from empty pipe
		storeerrno = errno;
		amt_read = read(fdin,start_of_chars,chars_to_read);
		errno = storeerrno;

		if (amt_read <= 0)
			{
			if (childisdead())
				{ // the child is dead
				if (secondtry)
					return EOF; // give up - nothing is left seeping out of the pipe of the dead child
				else
					secondtry = 1;  // we try to read from the pipe one more time even though the child is dead
				}
			storeerrno = errno;  // errno will get set to 4 when usleep terminates on a Sun
#ifndef NOUSLEEP
			if (usleeptime > 0)
				usleep(usleeptime);  // give child a chance to generate some output (or time for it to get through pipe)
#endif
			errno = storeerrno;
			}
		} while (amt_read <= 0);  // keep trying till we get something (or jump out if child dies)

	  // Tell the get buffer where the characters are
	setg(base(),start_of_chars,start_of_chars + amt_read);
	return(zapeof(*start_of_chars));
	}


pipebuf::~pipebuf()
	{
	close();
	}

