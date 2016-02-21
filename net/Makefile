include ../Makefile.inc

CFLAGS 	= $(PRJCFLAGS) -I..
OBJLIBS = ../libnet.a test/libnet.a
OBJS	= net.o

all : $(OBJLIBS)

../libnet.a : $(OBJS)
	$(ECHO) $(AR) $(ARFLAGS) rv ../libnet.a $?
	$(AR) $(ARFLAGS) rv ../libnet.a $?
	$(ECHO) $(RANLIB) ../libnet.a
	$(RANLIB) ../libnet.a
	
test/libnet.a : $(OBJS)
	$(ECHO) $(AR) $(ARFLAGS) rv test/libnet.a $?
	$(AR) $(ARFLAGS) rv test/libnet.a $?
	$(ECHO) $(RANLIB) test/libnet.a
	$(RANLIB) test/libnet.a

clean :
	$(ECHO) cleaning up in net directory
	-$(RM) -f $(OBJS)
	
force_look :
	true
