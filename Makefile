APP = vdereplay

CFLAGS += -Wall -g
LDFLAGS += -l vdeplug -l pcap

PREFIX ?= /usr/local
BINDIR ?= $(DESTDIR)$(PREFIX)/bin
MANDIR ?= $(DESTDIR)$(PREFIX)/share/man/man1


all: $(APP) $(APP).1

$(APP): $(APP).c
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

.md.1:
	pandoc -s -t man -o $@ $<

install: $(APP) $(APP).1
	install -c -d $(BINDIR)
	install -c -m 755 $(APP) $(BINDIR)
	install -c -d $(MANDIR)
	install -c -m 644 $(APP).1 $(MANDIR)

uninstall:
	-rm -f $(BINDIR)/$(APP)
	-rm -f $(MANDIR)/$(APP).1
	
clean:
	-rm -f *~

proper: clean
	-rm -f $(APP) $(APP).1

.PHONY: all install uninstall clean proper

.SUFFIXES: .md .1
