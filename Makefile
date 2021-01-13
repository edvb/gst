# gst - painless gist creator
# See LICENSE file for copyright and license details.

include config.mk

EXE = gst
SRC = gst.c extern/frozen.c
OBJ = $(SRC:.c=.o)

all: options $(EXE)

options:
	@echo $(EXE) build options:
	@echo "CFLAGS  = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"

.o:
	@echo $(LD) $@
	@$(LD) -o $@ $< $(LDFLAGS)

.c.o:
	@echo $(CC) $<
	@$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ): config.h config.mk

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

$(EXE): $(OBJ)
	@echo $(CC) -o $@
	@$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	@echo cleaning
	@rm -f $(OBJ) $(EXE)

install: all
	@echo installing $(EXE) to $(DESTDIR)$(PREFIX)/bin
	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@cp -f $(EXE) $(DESTDIR)$(PREFIX)/bin
	@chmod 755 $(DESTDIR)$(PREFIX)/bin/$(EXE)
	@echo installing manual page to $(DESTDIR)$(MANPREFIX)/man1
	@mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	@cp -f $(EXE).1 $(DESTDIR)$(MANPREFIX)/man1/
	@chmod 644 $(DESTDIR)$(MANPREFIX)/man1/$(EXE).1

uninstall:
	@echo removing $(EXE) from $(DESTDIR)$(PREFIX)/bin
	@rm -f $(DESTDIR)$(PREFIX)/bin/$(EXE)
	@echo removing manual page from $(DESTDIR)$(MANPREFIX)/man1
	@rm -f $(DESTDIR)$(MANPREFIX)/man1/$(EXE).1

man: $(EXE)
	@echo updating man page $(EXE).1
	@markman -nCD -t GST -V "$(EXE) $(VERSION)" -d "`date '+%B %Y'`" \
		-s "`./$(EXE) -h 2>&1 | cut -d' ' -f2-`" README.md > $(EXE).1

.PHONY: all options clean install uninstall man
