

    # what are the names of our compiler
CCC  = CC-flags
CC = gcc
CPP  = cpp

OBJS =  cli-set.o  cli-show.o  cli-save.o  cli-load.o  cli-misc.o  cli-lists.o  \
	cli-lib.o  cli-ui.o  cli-popup.o  cli-fractal.o		\
	cli-mouse.o  cli-proj.o  cli-cortex2d.o  cli-cortex3d.o  agen-iplib.o	\
	cli-ps.o


SRCS =  cli-set.cc cli-show.cc cli-save.cc cli-load.cc cli-misc.cc cli-lists.cc \
	cli-lib.cc cli-ui.cc cli-popup.cc cli-fractal.cc		\
	cli-mouse.cc cli-proj.cc cli-cortex2d.cc cli-cortex3d.cc agen-iplib.cc	\
	cli-ps.cc

# popup.cc


gen-ipl-h-lex.c: gen-ipl-h.l
	lex gen-ipl-h.l
	mv lex.yy.c gen-ipl-h-lex.c
gen-ipl-h.c: gen-ipl-h.y
	yacc gen-ipl-h.y
	mv y.tab.c gen-ipl-h.c
gen-ipl-h: gen-ipl-h.c gen-ipl-h-lex.c
	$(CC) -o gen-ipl-h gen-ipl-h.c -ly -g
agen-iplib.cc: gen-ipl-h ../../iplib/include/image-ops.h
	cpp -B -P -D_LISP_IP_ ../../iplib/include/image-ops.h | ./gen-ipl-h > agen-iplib.cc

cli-jl.a: $(OBJS)
	ar rcuv cli-jl.a $(OBJS)
	ranlib cli-jl.a

defun.list: defun.list.tmp
	../../bin/if-changed cp defun.list.tmp defun.list
defun.list.tmp: $(SRCS)
	exec > defun.list.tmp;\
	for f in $(SRCS); do \
		echo ""; \
		echo "/* $$f */"  ; \
		sed -n -f ../../jlisp/mdefun.sed $$f ; \
	done

defvar.list: defvar.list.tmp
	../../bin/if-changed cp defvar.list.tmp defvar.list
defvar.list.tmp: $(SRCS)
	exec > defvar.list.tmp;\
	for f in $(SRCS); do \
		echo ""; \
		echo "/* $$f */"  ; \
		sed -n -f ../../jlisp/mdefvar.sed $$f ; \
	done

clean:
	-rm -f cli-jl.a $(OBJS) agen-iplib.cc lex.yy.c y.tab.c gen-ipl *.list *.tmp
rcs:
	for x in $(SRCS) gen-ipl-h.l gen-ipl-h.y; do  echo . | ci -l $$x ; done

depends: cli-jl.cc
	@echo making depends...
	@/usr/ceas/lang.old/CC -M $(CPPFLAGS) cli-jl.cc\
	| grep -v /usr/include\
	| grep -v /usr/ceas\
	| grep -v /usr/openwin > depends




	
