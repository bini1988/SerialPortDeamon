include ../Makefile.inc

CFLAGS 	= $(PRJCFLAGS) -I..
OBJLIBS = ../libcom.a test/libcom.a
OBJS	= com.o

all : $(OBJLIBS)

../libcom.a : $(OBJS)
	$(ECHO) $(AR) $(ARFLAGS) rv ../libcom.a $?
	$(AR) $(ARFLAGS) rv ../libcom.a $?
	$(ECHO) $(RANLIB) ../libcom.a
	$(RANLIB) ../libcom.a

test/libcom.a : $(OBJS)
	$(ECHO) $(AR) $(ARFLAGS) rv test/libcom.a $?
	$(AR) $(ARFLAGS) rv test/libcom.a $?
	$(ECHO) $(RANLIB) test/libcom.a
	$(RANLIB) test/libcom.a

clean :
	$(ECHO) cleaning up in com directory
	-$(RM) -f $(OBJS)
	
force_look :
	true
