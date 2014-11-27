// @(#)pstream.h	1.15 
/*
 * pstream.h
 *    by Bill Dimm   June, 1991
 */

#include <stream.h>

#ifndef zapeof
#  define zapeof(_x) ((_x)&0377)
#endif

#undef NULL
#define NULL 0

/* Open modes (omode) which are currently supported: input, output, noshell
 * These can all be bitwise or'ed together.  If you don't use noshell it executes the process
 * in a shell (thus allowing environment variables, etc. to be used).  However,
 * if you don't use 'noshell', the child_pid will be that of the shell rather than that
 * of the process you are executing, so you can't use childpid() to get the id
 * of the child in order to kill it */

class pipebuf : public streambuf
	{
		void     	init();
	protected:
		int      	sizeofgetbuf;
		int      	input_isopen;  // 1 when pipe is open for input, 0 otherwise 
		int      	output_isopen; // 1 when pipe is open for output, 0 otherwise
		int      	child_pid; // if (mode|noshell) this is the pid of child, otherwise it is the pid of shell 
		int      	child_stat; // stat value of child, if still running (may not be current - must do updatechildstate())
		int      	child_isdead; // non-zero if child is dead (may not be current - must do updatechildstate())
		int      	fdin; // input file descriptor 
		int      	fdout; // output file descriptor 
		int      	mode; // gives mode of pipe - see pipe_mode below 
		unsigned int
		         	usleeptime;  // time to sleep between failed pipe accesses in microsec 
		char     	smallbuf[2];  // must be at least 2 bytes

		void     	updatechildstate();

		  // overrides virtual function in streambuf
		int      	doallocate(); 

	public:
		         	pipebuf() : streambuf() { init(); }
		         	pipebuf(const char *cmd, int omode) : streambuf() { init(); open(cmd,omode); }
		         	~pipebuf(); // virtual

		enum     	popen_mode { noshell = 010000 }; // 'noshell' tells it not to invoke sh to run the process

		pipebuf 	*open(const char *cmd, int omode);
		pipebuf  	*close(int cmode); // cmode is either 'input' or 'output'
		pipebuf  	*close();
		void     	setsleeptime(unsigned int micro_sec = 0) { usleeptime = micro_sec; }  
		int      	childstat() { updatechildstate(); return child_stat; } // updates child's state & returns stat
		int      	childisdead() { updatechildstate(); return child_isdead; } // updates child's state & tells if is dead
		int      	childpid() { return child_pid; }
		int      	bytesinpipe(int omode);
		streambuf	*setbuf(char *b, int len, int rdlen);

		  // these override virtual funcs in streambuf 
#ifdef OLDCC
		streampos	seekoff(streamoff, seek_dir, int omode);  // no seeking in pipe 
#else
		 // cfront 2.0 won't take the line below
		streampos	seekoff(streamoff, ios::seek_dir, int omode);  // no seeking in pipe 
#endif
		int      	pbackfail(int c);
		int      	overflow(int ch=EOF);
		int      	underflow();
		streampos	seekpos(streampos, int); // no seeking in pipe 
		streambuf	*setbuf(char *b, int len);
		int      	sync();
	};


static const int noshell = (pipebuf::noshell);

#define SETPSTRSTATE(_x) if (_x) clear(); else setstate(ios::failbit)

class ipstream : public istream
	{
	private:
		pipebuf  	pb;
	public:
		         	ipstream() : istream(&pb), pb() { }
		         	ipstream(const char *cmd, int omode=input) : istream(&pb), pb(cmd,omode|input) { }
		         	// use default destructor

		void     	close(int cmode) { SETPSTRSTATE(pb.close(cmode) != NULL); }
		void     	close() { SETPSTRSTATE(pb.close() != NULL); }
		void     	open(const char *cmd, int omode=input) { SETPSTRSTATE(pb.open(cmd,omode|input) != NULL); }
		pipebuf  	*rdbuf() { return &pb; }
		void     	setbuf(char *bufptr, int len, int rdlen) { SETPSTRSTATE(pb.setbuf(bufptr,len,rdlen) != NULL); } 
		void     	setbuf(char *bufptr, int len) { SETPSTRSTATE(pb.setbuf(bufptr,len) != NULL); } 
		void     	setsleeptime(unsigned int micro_sec = 0) { pb.setsleeptime(micro_sec); }
		int      	childisdead() { return pb.childisdead(); }
		int      	childstat() { return pb.childstat(); }
		int      	childpid() { return pb.childpid(); }
		int      	bytesinpipe(int omode = 0) { return pb.bytesinpipe(omode); }  // 0 tells it to use 'pb.mode'
	};


class opstream : public ostream
	{
	private:
		pipebuf  	pb;
	public:
		         	opstream() : ostream(&pb), pb() { }
		         	opstream(const char *cmd, int omode=output) : ostream(&pb), pb(cmd,omode|output) { }
		         	// use the default destructor

		void     	close(int cmode) { SETPSTRSTATE(pb.close(cmode) != NULL); }
		void     	close() { SETPSTRSTATE(pb.close() != NULL); }
		void     	open(const char *cmd, int omode=output) { SETPSTRSTATE(pb.open(cmd,omode|output) != NULL); }
		pipebuf  	*rdbuf() { return &pb; }
		void     	setbuf(char *bufptr, int len, int rdlen) { SETPSTRSTATE(pb.setbuf(bufptr,len,rdlen) != NULL); } 
		void     	setbuf(char *bufptr, int len) { SETPSTRSTATE(pb.setbuf(bufptr,len) != NULL); } 
		void     	setsleeptime(unsigned int micro_sec = 0) { pb.setsleeptime(micro_sec); }
		int      	childisdead() { return pb.childisdead(); }
		int      	childstat() { return pb.childstat(); }
		int      	childpid() { return pb.childpid(); }
		int      	bytesinpipe(int omode = 0) { return pb.bytesinpipe(omode); }  // 0 tells it to use 'pb.mode'
	};


class pstream : public iostream
	{
	private:
		pipebuf  	pb;
	public:
		         	pstream() : iostream(&pb), pb() { }
		         	pstream(const char *cmd, int omode) : iostream(&pb), pb(cmd,omode) { }
		         	// Use the default destructor

		void     	close(int cmode) { SETPSTRSTATE(pb.close(cmode) != NULL); }
		void     	close() { SETPSTRSTATE(pb.close() != NULL); }
		void     	open(const char *cmd, int omode) { SETPSTRSTATE(pb.open(cmd,omode) != NULL); }
		pipebuf  	*rdbuf() { return &pb; }
		void     	setbuf(char *bufptr, int len, int rdlen) { SETPSTRSTATE(pb.setbuf(bufptr,len,rdlen) != NULL); } 
		void     	setbuf(char *bufptr, int len) { SETPSTRSTATE(pb.setbuf(bufptr,len) != NULL); } 
		void     	setsleeptime(unsigned int micro_sec = 0) { pb.setsleeptime(micro_sec); }
		int      	childisdead() { return pb.childisdead(); }
		int      	childstat() { return pb.childstat(); }
		int      	childpid() { return pb.childpid(); }
		int      	bytesinpipe(int omode) { return pb.bytesinpipe(omode); } // No default since it would typically be ambiguous
	};

#undef SETPSTRSTATE

