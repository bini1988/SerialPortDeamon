include ../../Makefile.inc

EXE		= net_test
OBJS	= net_test.o 
OBJLIBS = libnet.a
LIBS	= -L. -lnet

all: $(EXE)

$(EXE) : net_test.o $(OBJLIBS)
	$(ECHO) $(LD) -o $(EXE) $(OBJS) $(LIBS)
	$(LD) -o $(EXE) $(OBJS) $(LIBS)
	
libcom.a : force_look
	$(ECHO) looking into net directory : $(MAKE) $(MFLAGS)
	cd ..; $(MAKE) $(MFLAGS)

clean :
	$(ECHO) cleaning up in net directory
	-$(RM) -f $(OBJS)
	
force_look :
	true
