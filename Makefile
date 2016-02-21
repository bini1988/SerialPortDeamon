include Makefile.inc

DIRS	= com dm net cfg proto
EXE		= comsrv
CFG		= etc/comsrv.conf
INIT 	= etc/init.d/comsrv
OBJS	= comsrv.o
OBJLIBS = libcom.a libdm.a libnet.a libcfg.a libproto.a
LIBS	= -L. -lcom -ldm -lnet -lcfg -lproto


all: $(EXE)

$(EXE) : comsrv.o $(OBJLIBS)
	$(ECHO) $(LD) -o $(EXE) $(OBJS) $(LIBS)
	$(LD) -o $(EXE) $(OBJS) $(LIBS)

libcom.a : force_look
	$(ECHO) looking into com directory : $(MAKE) $(MFLAGS)
	cd com; $(MAKE) $(MFLAGS)

libdm.a : force_look
	$(ECHO) looking into dm directory : $(MAKE) $(MFLAGS)
	cd dm; $(MAKE) $(MFLAGS)

libnet.a : force_look
	$(ECHO) looking into net directory : $(MAKE) $(MFLAGS)
	cd net; $(MAKE) $(MFLAGS)
	
libcfg.a : force_look
	$(ECHO) looking into cfg directory : $(MAKE) $(MFLAGS)
	cd cfg; $(MAKE) $(MFLAGS)
	
libproto.a : force_look
	$(ECHO) looking into proto directory : $(MAKE) $(MFLAGS)
	cd proto; $(MAKE) $(MFLAGS)
	
clean :
	$(ECHO) cleaning up in root directory
	-$(RM) -f $(EXE) $(OBJS) $(OBJLIBS)
	-for d in $(DIRS); do (cd $$d; $(MAKE) clean ); done

force_look :
	true

install:
	install ./$(EXE) /bin
	install $(CFG) /etc
	install ./$(INIT) /etc/init.d
	
unistall:
	rm -f /bin/$(EXE)
	rm -f /etc/$(CFG)
	rm -f /etc/init.d/$(EXE)
