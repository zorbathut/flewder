# Makefile for src/mod/flewder.mod/
# $Id: Makefile,v 1.14 2005-12-09 09:29:12 wcc Exp $

srcdir = .


doofus:
	@echo ""
	@echo "Let's try this from the right directory..."
	@echo ""
	@cd ../../../ && make

static: ../flewder.o

modules: ../../../flewder.$(MOD_EXT)

../flewder.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -DMAKING_MODS -c $(srcdir)/flewder.c
	@rm -f ../flewder.o
	mv flewder.o ../

../flewcore.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -DMAKING_MODS -c $(srcdir)/flewcore.cpp
	@rm -f ../flewcore.o
	mv flewcore.o ../

../../../flewder.$(MOD_EXT): ../flewder.o ../flewcore.o
	$(LD) -o ../../../flewder.$(MOD_EXT) ../flewder.o ../flewcore.o -lstdc++ $(XLIBS) $(MODULE_XLIBS)
	$(STRIP) ../../../flewder.$(MOD_EXT)

depend:
	$(CC) $(CFLAGS) -MM $(srcdir)/flewder.c  $(srcdir)/flewcore.cpp -MT ../flewder.o > .depend

clean:
	@rm -f .depend *.o *.$(MOD_EXT) *~

distclean: clean

#safety hash
../flewder.o ../flewcore.o: ./flewder.c ./flewcore.cpp ./flewcore.h ../../../src/mod/module.h ../../../src/main.h \
  ../../../config.h ../../../lush.h ../../../src/lang.h \
  ../../../src/eggdrop.h ../../../src/flags.h ../../../src/cmdt.h \
  ../../../src/tclegg.h ../../../src/tclhash.h ../../../src/chan.h \
  ../../../src/users.h ../../../src/compat/compat.h \
  ../../../src/compat/inet_aton.h ../../../src/main.h \
  ../../../src/compat/snprintf.h ../../../src/compat/memset.h \
  ../../../src/compat/memcpy.h ../../../src/compat/strcasecmp.h \
  ../../../src/compat/strftime.h ../../../src/mod/modvals.h \
  ../../../src/tandem.h
