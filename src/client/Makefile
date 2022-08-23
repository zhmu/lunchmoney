TARGET:		lmoney

OBJS =		main.o lmoneyc.o network.o command.o protocol.o player.o \
		card.o deck.o
LIBS =		-lncurses
CFLAGS +=	-ansi -pedantic -Wall
CFLAGS +=	-g
CC =		c++

lmoney:		$(OBJS)
		$(CC) -o lmoney $(OBJS) $(LIBS)

.cc.o:
		$(CC) -c $(CFLAGS) -o $@ $<

clean:
		rm -f lmoney $(OBJS)
