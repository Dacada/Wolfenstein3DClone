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

FINAL = wlfstn

.PHONY: all dbg clean

all: CFLAGS += -O3
all: | release
all: release/$(FINAL)

dbg: CFLAGS += -Og -DDEBUG -fsanitize=address -fsanitize=undefined -fsanitize=leak -fno-omit-frame-pointer
dbg: | debug
dbg: debug/$(FINAL)

clean:
	rm -f debug/*.o release/*.o
veryclean: clean
	rm -f debug/$(FINAL) release/$(FINAL)

release/$(FINAL): $(RELOBJFILES) Wolfenstein3DClone/lib/Engine3D.a
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
debug/$(FINAL): $(DBGOBJFILES) Wolfenstein3DClone/lib/Engine3D_debug.a
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

debug:
	mkdir -p debug
release:
	mkdir -p release

debug/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)
release/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)
