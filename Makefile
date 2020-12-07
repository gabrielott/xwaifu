PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

CFLAGS = -Wall
LDLIBS = -lX11 -lXext -lImlib2
OBJS = xwaifu.o

xwaifu: $(OBJS)
	$(CC) -o xwaifu $(LDLIBS) $(OBJS)

xwaifu.o:

.PHONY: clean install uninstall
clean:
	rm -f $(OBJS) xwaifu

install: xwaifu
	@echo 'INSTALL xwaifu'
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp xwaifu $(DESTDIR)$(PREFIX)/bin/
	chmod 755 $(DESTDIR)$(PREFIX)/bin/xwaifu
	@echo 'INSTALL xwaifu.1'
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp xwaifu.1 $(DESTDIR)$(MANPREFIX)/man1/
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/xwaifu.1

uninstall:
	@echo 'REMOVE xwaifu'
	rm -f $(DESTDIR)$(PREFIX)/bin/xwaifu
	@echo 'REMOVE xwaifu.1'
	rm -f $(DESTDIR)$(MANPREFIX)/man1/xwaifu.1
