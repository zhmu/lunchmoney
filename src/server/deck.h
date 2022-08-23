/*
 * LunchMoney Daemon - (c) 2002 Rink Springer, GPL
 *
 */
#ifndef __DECK_H__
#define __DECK_H__

#include <stdio.h>
#include "card.h"

/* DECK_NUM_CARDS is the number of cards allowed on a deck */
#define DECK_NUM_CARDS		110

/* DECK_MAX_LINE_LEN is the maximum length of a line */
#define DECK_MAX_LINE_LEN	1024

/*
 * DECK is a full card game deck.
 */
class DECK {
public:
		DECK();
		~DECK();

    int		load(char*);
    void	add(CARD*);
    void	clone(DECK*);
    void	shuffle();
    void	dump();

    CARD*	fetch();
    CARD*	get_card(int);

private:
    CARD*	card[DECK_NUM_CARDS];

    int		find_card (CARD*);
    int		count_cards();

    void	swap_cards(int,int);
};
#endif
