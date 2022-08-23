/*
 * LunchMoney Client - (c) 2002 Rink Springer, GPL
 *
 */
#ifndef __DECK_H__
#define __DECK_H__

#include <stdio.h>
#include "card.h"

/* DECK_NUM_CARDS is the number of cards allowed on a deck */
#define DECK_NUM_CARDS		110

/*
 * DECK is a full card game deck.
 */
class DECK {
public:
    DECK();

    int		load(char*);

    CARD*	get_card(int);

private:
    int		find_card(CARD*);
    CARD*	card[DECK_NUM_CARDS];

    void	add(CARD*);
    int		count_cards();
};

#endif
