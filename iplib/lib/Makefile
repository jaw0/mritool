

R_OBJS =  fullname.o ras_open_ghdr.o ras_read_bin.o ras_read_rgb.o\
	ras_read_ucr.o ras_write_ucr.o

V_OBJS = vff_write_3d.o ras_read_3d.o malloc_d.o
C_OBJS = spec_image.o fullname.o

M_OBJS = matrix.o vector.o cvector.o bvector.o

I_OBJS = images.o ip.o solid.o bug.o statics.o 	\
	fullnamecc.o bsave.o ptable.o\
	color.o shape.o contour.o fbimage.o imgops.o \
	segment.o color-quant.o	edgemap.o \
	cimages.o fft.o fft2d.o fractal.o stereo.o hough.o \
	pretty.o pipebuf.o
#shimages.o

IMGLIB = libImages.so.1.4
IMGLSA = libImages.sa.1.4

CC = gcc
CCC = g++

CFLAGS = -fpic  -O4
CCFLAGS = -fpic -g -DCPLUSPLUS -DDEBUG -D_STREAM_COMPAT -Wall
CPPFLAGS = -I../include

LDFLAGS = \
	-L$$HOME/lib4	\
	-lMatrix	\
	-lImages	\
	-lGG		\
	-lcomplex

CCR = $(CC) $(CFLAGS) $(CPPFLAGS) -c
CCCR = $(CCC) $(CFLAGS) $(CPPFLAGS) -c

all: shared static
install: all
	cp  lib3D.so.1.1 libMatrix.so.1.1 $(IMGLIB) $(IMGLSA) $(LIBDIR)


shared:  lib3D.so.1.1 libMatrix.so.1.1 $(IMGLIB) $(IMGLSA)
static:  lib3D.a libMatrix.a libImages.a

libjawR.so.1.1: $(R_OBJS)
	ld -assert pure-text -o libjawR.so.1.1 $(R_OBJS)
libjawR.a: $(R_OBJS)
	ar r libjawR.a $(R_OBJS)
	ranlib libjawR.a
libjawCC.so.1.1: $(C_OBJS)
	ld -assert pure-text -o libjawCC.so.1.1 $(C_OBJS)
libjawCC.a: $(C_OBJS)
	ar r libjawCC.a $(C_OBJS)
	ranlib libjawCC.a
lib3D.so.1.1: $(V_OBJS)
	ld -assert pure-text -o lib3D.so.1.1 $(V_OBJS)
lib3D.a: $(V_OBJS)
	ar r lib3D.a $(V_OBJS)
	ranlib lib3D.a
libMatrix.so.1.1: $(M_OBJS)
	ld -assert pure-text -o libMatrix.so.1.1 $(M_OBJS)
libMatrix.a:
	ar r libMatrix.a $(M_OBJS)
	ranlib libMatrix.a
$(IMGLIB): $(I_OBJS)
	ld -assert pure-text -o $(IMGLIB) $(I_OBJS)
$(IMGLSA): statics.o 
	ar rv $(IMGLSA) statics.o
	ranlib $(IMGLSA)
libImages.a: $(I_OBJS)
	ar r libImages.a $(I_OBJS) statics.o
	ranlib libImages.a
pipebuf.o:
	cd PStream; make
	cp PStream/pipebuf.o .
rcs:
	for x in *c; do echo . |ci -l $$x; done
	cd ../include; make rcs
clean:
	-rm -f *.o a.out core *.a *.so.* *.sa.*
	-rm -f PStream/pipebuf.o

vector.o:  ../include/vector.h ../include/function.h
bvector.o: ../include/vector.h ../include/function.h
cvector.o: ../include/vector.h ../include/function.h
matrix.o:  ../include/vector.h ../include/function.h ../include/matrix.h

images.o:	../include/images.h
ip.o:		../include/images.h
solid.o:	../include/images.h
shimages.o:	../include/images.h
bsave.o:	../include/images.h ../include/ptable.h
color.o:	../include/images.h
shape.o:	../include/images.h
fbimage.o:	../include/images.h
imgops.o:	../include/images.h

