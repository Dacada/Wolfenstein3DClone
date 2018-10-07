SRCDIR = Wolfenstein3DClone/src
INCDIR = Wolfenstein3DClone/include

LIB = -lGLEW -lGLU -lGL -lfreeimage -lm

CC = gcc
CFLAGS = -IWolfenstein3DClone/lib/include -I$(INCDIR) -Wall -Wextra -Werror -std=gnu11 -Wformat\
         -Wformat-security -D_FORTIFY_SOURCE=2 $(shell pkg-config --cflags glfw3)
LDFLAGS = $(shell pkg-config --static --libs glfw3) $(LIB)

SRCFILES = $(wildcard $(SRCDIR)/*.c)
INCFILES = $(wildcard $(INCDIR)/*.h)
DBGOBJFILES = $(patsubst $(SRCDIR)/%, debug/%, $(SRCFILES:.c=.o))
RELOBJFILES = $(patsubst $(SRCDIR)/%, release/%, $(SRCFILES:.c=.o))
PRFOBJFILES = $(patsubst $(SRCDIR)/%, profile/%, $(SRCFILES:.c=.o))

FINAL = wlfstn

.PHONY: all dbg prf clean

all: CFLAGS += -O3
all: | release
all: release/$(FINAL)

dbg: CFLAGS += -Og -fsanitize=address -fno-omit-frame-pointer
dbg: | debug
dbg: debug/$(FINAL)

prf: CFLAGS += -pg
prf: | profile
prf: profile/$(FINAL)

clean:
	rm -f debug/*.o release/*.o profile/*.o
veryclean: clean
	rm -f debug/$(FINAL) release/$(FINAL) profile/$(FINAL)

release/$(FINAL): $(RELOBJFILES) Wolfenstein3DClone/lib/Engine3D.a
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
debug/$(FINAL): $(DBGOBJFILES) Wolfenstein3DClone/lib/Engine3D.a
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
profile/$(FINAL): $(PRFOBJFILES) Wolfenstein3DClone/lib/Engine3D.a
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

debug:
	mkdir -p debug
release:
	mkdir -p release
profile:
	mkdir -p profile

debug/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)
release/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)
profile/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)
