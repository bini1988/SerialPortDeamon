include ../Makefile.inc

CFLAGS 	= $(PRJCFLAGS) -I..
OBJLIBS = ../libproto.a
OBJS	= proto.o

all : $(OBJLIBS)

../libproto.a : $(OBJS)
	$(ECHO) $(AR) $(ARFLAGS) rv ../libproto.a $?
	$(AR) $(ARFLAGS) rv ../libproto.a $?
	$(ECHO) $(RANLIB) ../libproto.a
	$(RANLIB) ../libproto.a

clean :
	$(ECHO) cleaning up in proto directory
	-$(RM) -f $(OBJS)
	
force_look :
	true
