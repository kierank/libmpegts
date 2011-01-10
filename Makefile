# Makefile

include config.mak

all: default

SRCS = crc/crc.c atsc/atsc.c cablelabs/cablelabs.c dvb/dvb.c hdmv/hdmv.c smpte/smpte.c libmpegts.c

SRCSO =

CONFIG := $(shell cat config.h)

OBJS = $(SRCS:%.c=%.o)
OBJCLI = $(SRCCLI:%.c=%.o)
OBJSO = $(SRCSO:%.c=%.o)
DEP  = depend libmpegts.a

.PHONY: all default clean distclean install uninstall dox test testclean

default: $(DEP)

libmpegts.a: .depend $(OBJS)
	$(AR) rc libmpegts.a $(OBJS)
	$(RANLIB) libmpegts.a

$(SONAME): .depend $(OBJS) $(OBJSO)
	$(CC) -shared -o $@ $(OBJS) $(OBJSO) $(SOFLAGS) $(LDFLAGS)

.depend: config.mak
	@rm -f .depend
	@$(foreach SRC, $(SRCS) $(SRCSO), $(CC) $(CFLAGS) $(SRC) -MT $(SRC:%.c=%.o) -MM -g0 1>> .depend;)

config.mak:
	./configure

depend: .depend
ifneq ($(wildcard .depend),)
include .depend
endif

SRC2 = $(SRCS)

clean:
	rm -f $(OBJS) $(OBJSO) $(OBJCLI) $(SONAME) *.a .depend TAGS
	rm -f $(SRC2:%.c=%.gcda) $(SRC2:%.c=%.gcno)
	- sed -e 's/ *-fprofile-\(generate\|use\)//g' config.mak > config.mak2 && mv config.mak2 config.mak

distclean: clean
	rm -f config.mak config.h config.log libmpegts.pc
	rm -rf test/

install: $(SONAME)
	install -d $(DESTDIR)$(bindir)
	install -d $(DESTDIR)$(includedir)
	install -d $(DESTDIR)$(libdir)
	install -d $(DESTDIR)$(libdir)/pkgconfig
	install -m 644 libmpegts.h $(DESTDIR)$(includedir)
	install -m 644 libmpegts.a $(DESTDIR)$(libdir)
	install -m 644 libmpegts.pc $(DESTDIR)$(libdir)/pkgconfig
	$(RANLIB) $(DESTDIR)$(libdir)/libmpegts.a
ifeq ($(SYS),MINGW)
	$(if $(SONAME), install -m 755 $(SONAME) $(DESTDIR)$(bindir))
else
	$(if $(SONAME), ln -f -s $(SONAME) $(DESTDIR)$(libdir)/libmpegts.$(SOSUFFIX))
	$(if $(SONAME), install -m 755 $(SONAME) $(DESTDIR)$(libdir))
endif
	$(if $(IMPLIBNAME), install -m 644 $(IMPLIBNAME) $(DESTDIR)$(libdir))

uninstall:
	rm -f $(DESTDIR)$(includedir)/libmpegts.h $(DESTDIR)$(libdir)/libmpegts.a
	rm -f $(DESTDIR)$(libdir)/pkgconfig/libmpegts.pc
	$(if $(SONAME), rm -f $(DESTDIR)$(libdir)/$(SONAME) $(DESTDIR)$(libdir)/libmpegts.$(SOSUFFIX))

etags: TAGS

TAGS:
	etags $(SRCS)
