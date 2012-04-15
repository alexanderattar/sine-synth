# Makefile - Mac OS/Linux
# 

CC      = gcc
CFLAGS  = -g -std=c99 -Wall -lncurses -lportaudio
EXE  = sinePlayer
SRCS = sinePlayer.c
HDRS = sinePlayer.h

$(EXE):	$(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $(SRCS) -o $@

clean:
	rm -f *~ core $(EXE) *.o
