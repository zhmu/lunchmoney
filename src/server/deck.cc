/*
 * LunchMoney 2.0 Daemon - deck.cc (LunchMoney Deck)
 * (c) 2002 Rink Springer
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRENTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "card.h"
#include "debug.h"
#include "deck.h"
#include "lmoney.h"
#include "tools.h"

/*
 * DECK::DECK()
 *
 * This will create an empty deck.
 *
 */
DECK::DECK() {
    int i;

    // no cards on it
    for (i = 0; i < DECK_NUM_CARDS; i++)
	card[i] = NULL;
}

/*
 * DECK::~DECK()
 * 
 * This will free all cards in the deck.
 *
 */
DECK::~DECK() {
    int i;

    // get rid of all cards
    for (i = 0; i < DECK_NUM_CARDS; i++)
	// do we have a card here?
	if (card[i])
	    // yes. get rid of it
	    free (card[i]);
}

/*
 * DECK::clone (DECK* dest)
 *
 * This will clone this deck to [dest].
 *
 */
void
DECK::clone (DECK* dest) {
    int i;

    // handle them all
    for (i = 0; i < DECK_NUM_CARDS; i++)
	// got a card?
	if (card[i])
	    // yes. add it to the other deck
	    dest->add (new CARD (card[i]));
}

/*
 * DECK::find_card (CARD* c)
 *
 * This will find card [c] and return the position within the deck. It will
 * return -1 if the card is not in the deck.
 *
 */
int
DECK::find_card(CARD* c) {
    int i;

    // scan them all
    for (i = 0; i < DECK_NUM_CARDS; i++)
	// match?
	if (card[i] == c)
	    // yes. return the position
	    return i;

    // no such card
    return -1;
}

/*
 * DECK::get_card (int no)
 *
 * This will retrieve card [no].
 *
 */
CARD*
DECK::get_card (int no) {
    return card[no];
}

/*
 * DECK::swap_cards (int c1, int c2)
 *
 * This will swap cards [c1] and [c2].
 *
 */
void
DECK::swap_cards (int c1, int c2) {
    CARD* c;

    c = card[c1];
    card[c1] = card[c2];
    card[c2] = c;
}

/*
 * DECK::shuffle()
 *
 * This will shuffle the cards.
 *
 */
void
DECK::shuffle() {
    int i = 100 + (rand() % 50);
    int n = count_cards();
    int num1, num2;

    while (i--) {
	num1 = rand() % n; num2 = rand() % n;
	swap_cards (num1, num2);
    }
}

/*
 * DECK::add (CARD* c)
 *
 * This will add card [c] to the deck.
 *
 */
void
DECK::add (CARD* c) {
    int   pos = find_card (NULL);

    if (pos < 0)
	return; 
 
   // add the card
   card[pos] = c;
}

/*
 * DECK::count_cards()
 *
 * This will count how much cards we have and return that number.
 *
 */
int
DECK::count_cards() {
    int i, count = 0;

    // keep counting
    for (i = 0; i < DECK_NUM_CARDS; i++)
	if (card[i] != NULL)
	    count++;

    // return the counter
    return count;
}

/*
 * DECK::load (char* fname)
 *
 * This will load a deck from file [fname]. It will return zero on success or
 * a negative number of failure.
 *
 */
int
DECK::load (char* fname) {
    FILE* f;
    char* ptr;
    char  tmp[DECK_MAX_LINE_LEN + 1];

    CARD* c;
    char  name[DECK_MAX_LINE_LEN + 1];
    int   type;
    int   cat;
    int   damage;
    int   count;

    // open the file
    if ((f = fopen (fname, "rt")) == NULL) {
	// this failed. complain
	DPRINTF (DEBUG_INFO, "DECK::load(): file '%s' could not be opened", fname);
	return -1;
    }

    // keep reading the file
    while (fgets (tmp, sizeof (tmp), f)) {
	// zap the newline
	tmp[strlen (tmp) - 1] = 0;
	ptr = tmp;

	// skip any spaces and tabs
	while ((*ptr == ' ') && (*ptr == '\t')) ptr++;

	// do we have a comment line?
	if (*ptr != '#') {
	    // no. grab the card
	    if (format (tmp, "%i|%i|%i|%i|%s", &type, &cat, &damage, &count, name) == 0) {
		// add the card
		c = new CARD (name, type, cat, damage);

		// add the card
		while (count--)
		    add (c);
	    } else {
		DPRINTF (DEBUG_WARNING, "DECK::load(): corrupt line [%s]", tmp);
	    }
	}
    }

    // close the file
    fclose (f);

    // do we have the correct number of cards?
    if (count_cards() != DECK_NUM_CARDS) {
	// no. warn the user
	DPRINTF (DEBUG_WARNING, "Warning: DECK::load(): file '%s' does not have %u cards, but only %u\n", fname, DECK_NUM_CARDS, count_cards());
    }

    // it worked
    return 0;
}

/*
 * DECK::dump()
 *
 * This will dump the deck.
 *
 */
void
DECK::dump() {
    int i;

    // list them all
    for (i = 0; i < DECK_NUM_CARDS; i++)
	if (card[i])
	    printf ("%u. %s\n", i, card[i]->get_name());
}

/*
 * DECK::fetch()
 *
 * This will get the top card from the deck and return it.
 *
 */
CARD*
DECK::fetch () {
    CARD* c;
    int i;

    // get the topmost card
    c = card[0];

    // shift them all up
    for (i = 0; i < DECK_NUM_CARDS - 1; i++)
	card[i] = card[i + 1];

    // put this card at the bottom
    card[DECK_NUM_CARDS - 1] = c;

    // return the card
    return c;
}
