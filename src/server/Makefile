TARGET:		lmoneyd

OBJS =		main.o lmoney.o network.o lmserver.o lmclient.o player.o \
		crowd.o tools.o protocol.o debug.o game.o playground.o \
		card.o deck.o
CFLAGS +=	-ansi -pedantic -Wall
CFLAGS +=	-g
CC =		c++

lmoneyd:	$(OBJS)
		$(CC) -o lmoneyd $(OBJS)

.cc.o:
		$(CC) -c $(CFLAGS) -o $@ $<

clean:
		rm -f lmoneyd $(OBJS)
