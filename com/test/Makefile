include ../../Makefile.inc

EXE		= com_test
OBJS	= com_test.o 
OBJLIBS = libcom.a
LIBS	= -L. -lcom

all: $(EXE)

$(EXE) : com_test.o $(OBJLIBS)
	$(ECHO) $(LD) -o $(EXE) $(OBJS) $(LIBS)
	$(LD) -o $(EXE) $(OBJS) $(LIBS)
	
libcom.a : force_look
	$(ECHO) looking into com directory : $(MAKE) $(MFLAGS)
	cd ..; $(MAKE) $(MFLAGS)

clean :
	$(ECHO) cleaning up in com directory
	-$(RM) -f $(OBJS)
	
force_look :
	true
