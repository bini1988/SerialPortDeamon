include ../Makefile.inc

CFLAGS 	= $(PRJCFLAGS) -I..
OBJLIBS = ../libcfg.a test/libcfg.a
OBJS	= cfg.o ini.o

all : $(OBJLIBS)

../libcfg.a : $(OBJS)
	$(ECHO) $(AR) $(ARFLAGS) rv ../libcfg.a $?
	$(AR) $(ARFLAGS) rv ../libcfg.a $?
	$(ECHO) $(RANLIB) ../libcfg.a
	$(RANLIB) ../libcfg.a
	
test/libcfg.a : $(OBJS)
	$(ECHO) $(AR) $(ARFLAGS) rv test/libcfg.a $?
	$(AR) $(ARFLAGS) rv test/libcfg.a $?
	$(ECHO) $(RANLIB) test/libcfg.a
	$(RANLIB) test/libcfg.a
	
clean :
	$(ECHO) cleaning up in cfg directory
	-$(RM) -f $(OBJS)
	
force_look :
	true
