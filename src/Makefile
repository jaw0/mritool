
# $Id: Makefile,v 1.1 1995/11/23 04:30:12 jaw Exp jaw $

    # our object files
OBJS = 							\
	cl_patient.o main.o patient_db.o		\
	window.o lists.o popups.o	 		\
	misc.o project.o mouse.o draw.o 		\
	dg2sun.o  file.o display.o 			\
	segment_m.o test.o adjust_wl.o errors.o		\
	stubs.o mode.o roi.o raster.o			\
	snake.supp.o snake.newer.o			\
	object.o measure.o fullname.mri2.o		\
	nowin.o analyze.o				\
	interp.o user.o signal.o			\
	mrilisp.o rdb_client.o

OBJLS = cli-jl.a ../jlisp.a

TARGET = mri-jl
BAKEXT = .bak

CCC = CC-flags

$(TARGET): $(OBJS) $(OBJLS)
	$(LINK.cc) $(OBJS) $(OBJLS) -o $(TARGET) $(LDFLAGS)
	@mv $(NUMBER) $(NUMBER)$(BAKEXT)
	@echo `expr \`cat $(NUMBER)$(BAKEXT)\` + 1` > $(NUMBER)
	@rm $(NUMBER)$(BAKEXT)

../jlisp.a: _jlispa
	cd ..; $(MAKE) jlisp.a

cli-jl.a: _cli-jla
	cd cli-jl; $(MAKE) cli-jl.a CCFLAGS='$(CCFLAGS)'
	../bin/if-changed cp cli-jl/cli-jl.a cli-jl.a

cli-jl/defun.list: _cli-defun.list
	cd cli-jl; $(MAKE) defun.list

cli-jl/defvar.list: _cli-defvar.list
	cd cli-jl; $(MAKE) defvar.list

clean: nobak
	-rm -f $(OBJS) $(TARGET) cli-jl.a segmtoras nowin-yacc.c nowin-yacc.y nowin-lex.c core a.out #*#
	cd cli-jl; $(MAKE) clean
rcs:
	for x in *cc Makefile; do  echo . | ci -l $$x ; done
	cd cli-jl; $(MAKE) rcs
nobak:
	-rm -f *% *~

depends:
	@echo making depends...
	@/usr/ceas/lang.old/CC -M $(CPPFLAGS) *.cc\
	| grep -v /usr/include\
	| grep -v /usr/ceas\
	| grep -v openwin\
	| grep -v null >depends
	cd jlisp; $(MAKE) depends
	cd cli-jl; $(MAKE) depends
Makefile: depends
	@echo making Makefile...
	@cp Makefile Makefile$(BAKEXT)
	@sed -n '1,/#END_[oO]F_MAKE/p' Makefile\
		| sed "s%^srcdir =.*\$$%srcdir = `pwd`%"  >Make.new
	@cat Make.new depends > Makefile
	@rm -f Make.new

_cli-jla:
_jlispa:
_cli-defun.list:
_cli-defvar.list:

# DO NOT remove following line!
#END_OF_MAKE
mrilisp.o: cli-jl/defvar.list
mrilisp.o: cli-jl/defun.list
