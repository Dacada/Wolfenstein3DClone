LIB = -lGLEW -lGLU -lGL -lfreeimage -lm

CC = gcc
CFLAGS = -IWolfenstein3DClone/lib/include -Wall -Wextra -Werror -std=gnu11 -Wformat\
         -Wformat-security -D_FORTIFY_SOURCE=2 $(shell pkg-config --cflags glfw3)
LDFLAGS = $(shell pkg-config --static --libs glfw3) $(LIB)

.PHONY: all dbg prf clean

all: CFLAGS += -O3
all: | release
all: release/main

dbg: CFLAGS += -Og
dbg: | debug
dbg: debug/main

prf: CFLAGS += -pg
prf: | profile
prf: profile/main

clean:
	rm -f debug/main release/main profile/main

release/main: Wolfenstein3DClone/src/main.c Wolfenstein3DClone/lib/Engine3D.a
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
debug/main: Wolfenstein3DClone/src/main.c Wolfenstein3DClone/lib/Engine3D.a
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
profile/main: Wolfenstein3DClone/src/main.c Wolfenstein3DClone/lib/Engine3D.a
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

debug:
	mkdir -p debug
release:
	mkdir -p release
profile:
	mkdir -p profile
