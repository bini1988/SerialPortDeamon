include ../../Makefile.inc

EXE		= cfg_test
OBJS	= cfg_test.o 
OBJLIBS = libcfg.a
LIBS	= -L. -lcfg

all: $(EXE)

$(EXE) : cfg_test.o $(OBJLIBS)
	$(ECHO) $(LD) -o $(EXE) $(OBJS) $(LIBS)
	$(LD) -o $(EXE) $(OBJS) $(LIBS)
	
libcom.a : force_look
	$(ECHO) looking into cfg directory : $(MAKE) $(MFLAGS)
	cd ..; $(MAKE) $(MFLAGS)

clean :
	$(ECHO) cleaning up in net directory
	-$(RM) -f $(OBJS)
	
force_look :
	true
