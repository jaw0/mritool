# by Bill Dimm  June 1991   @(#)makefile	1.11
#
# You should change CFLAGS so that it contains one of the 
# following (if appropriate):
#  -DWAITPID  if your computer had waitpid()  (automatic on RS6000)
#  -DWAIT4    if your computer does not have waitpid(), but does have wait4()
#  -DKILL0    if your computer does not have waitpid(), wait4() or something
#                comparable.  Note that this should only be used as a last
#                resort - status info for the child will not get set if this
#                is used.
# If your computer doesn't have a usleep() function, you should add
# the flag -DNOUSLEEP
#
# Some older compilers, like cfront 2.0, don't allow function declarations
# with arguments like "ios::seek_dir sd", but want instead "seek_dir sd".
# If your compiler has this problem add to CFLAGS:  -DOLDCC
# Note that you will need to include the -DOLDCC flag when you
# compile the test programs and your own programs which use pstream.
#
# Some compilers expect the 2nd argument of wait4() to be of type (int *)
# rather than (union wait *).  If you have this problem, 
# add to CFLAGS: -DWAIT4_INTPTR

CC = g++
CFLAGS=-O -DWAIT4 -fpic

pipebuf.o: pstream.h pipebuf.C
	$(CC) $(CFLAGS) -c -o pipebuf.o pipebuf.C
	
test:
	cd Test ; make all

