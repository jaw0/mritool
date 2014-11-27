
# jlisp makefile
# $Id: Makefile,v 1.1 1995/11/23 04:30:39 jaw Exp jaw $

OBJSA = unexec.o alloc.o eval.o gc.o jlisp.o reader.o\
	symtab.o print.o jlib.o pred.o string.o \
	list.o alist.o math.o port.o unistd.o \
	ball.o hooks.o error.o signals.o debug.o \
	weak.o bignum.o ipc2.o struct.o \
	$(EXTRAOBJS)

OBJSL = init_l.o
OBJSS = init_s.o

SRCS =  unexec.c alloc.c eval.c gc.c jlisp.c reader.c\
	symtab.c print.c jlib.c pred.c string.c \
	list.c alist.c math.c port.c unistd.c \
	ball.c hooks.c error.c signals.c debug.c \
	weak.c bignum.c ipc2.c struct.c \
	init.c


CFLAGS = -I. \
	$(OPTIMIZE)			\
	$(DEBUG)			\
	$(STANDALONE)			\
	-DJLISP_INIT=$(INIT_FILE) 	\
	-DVERSION_MAJOR=$(version)

all: jl jlisp.a

jl: $(OBJSA) $(OBJSS) defproto.h
	$(CC)  $(CFLAGS) -o jl $(OBJSA) $(OBJSS) -lm $(EXTRALIBS)

init_l.o:	defun.list defvar.list init.c
	$(CC) $(CFLAGS) -DNOT_STAND_ALONE -c init.c
	mv init.o init_l.o
init_s.o:	defun.list defvar.list init.c
	$(CC) $(CFLAGS) -c init.c
	mv init.o init_s.o

$(OBJSA): jlisp.h jlconf.h
$(OBJSL): jlisp.h jlconf.h
$(OBJSS): jlisp.h jlconf.h

jlisp.a: $(OBJSA) $(OBJSL)
	ar rcuv jlisp.a $(OBJSA) $(OBJSL)
	ranlib jlisp.a

clean:
	-rm -f $(OBJSA) $(OBJSL) $(OBJSS) jl jlisp.a defun.list defvar.list
	-rm -rf defun.list.tmp defvar.list.tmp defproto.h
	-rm -rf a.out core

realclean: clean

defun.list: defun.list.tmp
	../bin/if-changed cp defun.list.tmp defun.list
defun.list.tmp: defun
defun:
	exec > defun.list.tmp;\
	for f in $(SRCS); do \
		echo ""; \
		echo "/* $$f */"  ; \
		sed -n -f mdefun.sed $$f ; \
	done

defvar.list: defvar.list.tmp
	../bin/if-changed cp defvar.list.tmp defvar.list
defvar.list.tmp: defvar
defvar:
	exec > defvar.list.tmp;\
	for f in $(SRCS); do \
		echo ""; \
		echo "/* $$f */"  ; \
		sed -n -f mdefvar.sed $$f ; \
	done
	
defproto.h: defun.list defproto.gen
	$(CPP) -I. -C -P defproto.gen > defproto.h

rcs:
	for f in $(SRCS) *.h Internals Makefile ToDo Describble Ack* License README; do echo .|ci -l $$f; done
	for x in `find Doc -name RCS -prune -o -type f -print`; do echo . | ci -l $$x; done

depends:
	@echo making depends...
	@$(CC) -M $(CFLAGS) $(SRCS) \
	| grep -v /usr/ceas\
	| grep -v /usr/include > depends


Makefile: depends
	@echo making Makefile...
	@cp Makefile Makefile.BAK
	@sed -n '1,/#END_[oO]F_MAKE/p' Makefile\
		| sed "s%^srcdir =.*\$$%srcdir = `pwd`%"  >Make.new
	@cat Make.new depends > Makefile
	@rm -f Make.new

stamp:
	touch stamp


# DO NOT remove following line!
#END_OF_MAKE
unexec.o: unexec.c
unexec.o: ./jlisp.h
unexec.o: ./jlconf.h
unexec.o: ./jl_proto.h
unexec.o: ./jlconf.h
alloc.o: alloc.c
alloc.o: ./jlisp.h
alloc.o: ./jlconf.h
alloc.o: ./jl_proto.h
alloc.o: ./jlconf.h
eval.o: eval.c
eval.o: ./jlisp.h
eval.o: ./jlconf.h
eval.o: ./jl_proto.h
eval.o: ./jlconf.h
gc.o: gc.c
gc.o: ./jlisp.h
gc.o: ./jlconf.h
gc.o: ./jl_proto.h
gc.o: ./jlconf.h
jlisp.o: jlisp.c
jlisp.o: ./jlisp.h
jlisp.o: ./jlconf.h
jlisp.o: ./jl_proto.h
jlisp.o: ./jlconf.h
reader.o: reader.c
reader.o: ./jlisp.h
reader.o: ./jlconf.h
reader.o: ./jl_proto.h
reader.o: ./jlconf.h
symtab.o: symtab.c
symtab.o: ./jlisp.h
symtab.o: ./jlconf.h
symtab.o: ./jl_proto.h
symtab.o: ./jlconf.h
print.o: print.c
print.o: ./jlisp.h
print.o: ./jlconf.h
print.o: ./jl_proto.h
print.o: ./jlconf.h
jlib.o: jlib.c
jlib.o: ./jlisp.h
jlib.o: ./jlconf.h
jlib.o: ./jl_proto.h
jlib.o: ./jlconf.h
pred.o: pred.c
pred.o: ./jlisp.h
pred.o: ./jlconf.h
pred.o: ./jl_proto.h
pred.o: ./jlconf.h
string.o: string.c
string.o: ./jlisp.h
string.o: ./jlconf.h
string.o: ./jl_proto.h
string.o: ./jlconf.h
list.o: list.c
list.o: ./jlisp.h
list.o: ./jlconf.h
list.o: ./jl_proto.h
list.o: ./jlconf.h
alist.o: alist.c
alist.o: ./jlisp.h
alist.o: ./jlconf.h
alist.o: ./jl_proto.h
alist.o: ./jlconf.h
math.o: math.c
math.o: ./jlisp.h
math.o: ./jlconf.h
math.o: ./jl_proto.h
math.o: ./jlconf.h
port.o: port.c
port.o: ./jlisp.h
port.o: ./jlconf.h
port.o: ./jl_proto.h
port.o: ./jlconf.h
unistd.o: unistd.c
unistd.o: ./jlisp.h
unistd.o: ./jlconf.h
unistd.o: ./jl_proto.h
unistd.o: ./jlconf.h
ball.o: ball.c
ball.o: ./jlisp.h
ball.o: ./jlconf.h
ball.o: ./jl_proto.h
ball.o: ./jlconf.h
hooks.o: hooks.c
hooks.o: ./jlisp.h
hooks.o: ./jlconf.h
hooks.o: ./jl_proto.h
hooks.o: ./jlconf.h
error.o: error.c
error.o: ./jlisp.h
error.o: ./jlconf.h
error.o: ./jl_proto.h
error.o: ./jlconf.h
signals.o: signals.c
signals.o: ./jlisp.h
signals.o: ./jlconf.h
signals.o: ./jl_proto.h
signals.o: ./jlconf.h
debug.o: debug.c
debug.o: ./jlisp.h
debug.o: ./jlconf.h
debug.o: ./jl_proto.h
debug.o: ./jlconf.h
weak.o: weak.c
weak.o: ./jlisp.h
weak.o: ./jlconf.h
weak.o: ./jl_proto.h
weak.o: ./jlconf.h
bignum.o: bignum.c
bignum.o: ./jlisp.h
bignum.o: ./jlconf.h
bignum.o: ./jl_proto.h
bignum.o: ./jlconf.h
ipc2.o: ipc2.c
ipc2.o: ./jlisp.h
ipc2.o: ./jlconf.h
ipc2.o: ./jl_proto.h
ipc2.o: ./jlconf.h
struct.o: struct.c
struct.o: ./jlisp.h
struct.o: ./jlconf.h
struct.o: ./jl_proto.h
struct.o: ./jlconf.h
init.o: init.c
init.o: ./jlisp.h
init.o: ./jlconf.h
init.o: ./jl_proto.h
init.o: ./jlconf.h
init.o: ./defun.list
init.o: ./defvar.list
init.o: ./defun.list
init.o: ./defvar.list
