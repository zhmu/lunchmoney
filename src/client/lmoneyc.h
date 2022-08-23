/*
 * LunchMoney Client - (c) 2002 Rink Springer, GPL
 *
 */
#ifndef __LMONEY_H__
#define __LMONEY_H__

#include <ncurses.h>
#include <stdio.h>
#include "deck.h"
#include "network.h"
#include "protocol.h"

/* WPRINTF() is a convenient wrapper to printf() something in the window */
#define WPRINTF lmoneyc->wprintf

/*
 * LMONEYC is the main LunchMoney client.
 */
class LMONEYC {
public:
    LMONEYC();

    void init();
    void run();
    void done();

    void stop();

    void wprintf (char*, ...);

    PROTOCOL*	p;
    DECK*	deck;

private:
    WINDOW* main;
    WINDOW* input;
    WINDOW* content;

    int		running;
};

extern LMONEYC* lmoneyc;

#endif
