include ../Makefile.inc

CFLAGS 	= $(PRJCFLAGS) -I..
OBJLIBS = ../libdm.a
OBJS	= dm.o

all : $(OBJLIBS)

../libdm.a : $(OBJS)
	$(ECHO) $(AR) $(ARFLAGS) rv ../libdm.a $?
	$(AR) $(ARFLAGS) rv ../libdm.a $?
	$(ECHO) $(RANLIB) ../libdm.a
	$(RANLIB) ../libdm.a

clean :
	$(ECHO) cleaning up in dm directory
	-$(RM) -f $(OBJS)
	
force_look :
	true
