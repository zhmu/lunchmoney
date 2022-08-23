/*
 * LunchMoney Client - (c) 2002 Rink Springer, GPL
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "card.h"
#include "deck.h"
#include "lmoneyc.h"

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
	card[i] = 0;
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
 * DECK::get_card (int type)
 *
 * This will retrieve the card [type], or NULL if it does not exist.
 *
 */
CARD*
DECK::get_card (int type) {
    int i;

    // scan them all
    for (i = 0; i < DECK_NUM_CARDS; i++)
	// is the type the same?
	if (card[i]->get_type() == type)
	    // yes. return the card
	    return card[i];

    // there was no such card
    return NULL;
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
    char tmp[1024];
    char* ptr;
    char* ptr2;
    char* end;

    CARD* c;
    char* name;
    int   type;
    int   cat;
    int   damage;
    int   count;
    int   i;

    // open the file
    if ((f = fopen (fname, "rt")) == NULL) {
	// this failed. complain
	WPRINTF ("DECK::load(): file '%s' could not be opened", fname);
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
	    // no. scan for the first '|''
	    ptr2 = strchr (ptr, '|');
	    if (ptr2) {
		// we got it. nullify it
		*ptr2 = 0; 

		// try to resolve it as an integer
		type = strtol (ptr, &end, 10);
		if (!*end) {
		    // this is correct. go
		    ptr = (ptr2 + 1);
		    ptr2 = strchr (ptr, '|');
		    if (ptr2) {
			// we got that too
			*ptr2 = 0; cat = strtol (ptr, &end, 10);
			if (!*end) {
			    // this is correct. go
			    ptr = (ptr2 + 1);
			    ptr2 = strchr (ptr, '|');
			    if (ptr2) {
				// we got that too
				*ptr2 = 0; damage = strtol (ptr, &end, 10);
				if (!*end) {
				    ptr = (ptr2 + 1);
				    ptr2 = strchr (ptr, '|');
				    if (ptr2) {
					// we got that too
					*ptr2 = 0; count = strtol (ptr, &end, 10);
					if (!*end) {
					    // finally... fetch the name
					    name = (end + 1);

					    // add the cards
					    for (i = 0; i < count; i++) {
						c = new CARD (name, type, cat, damage);
						add (c);
					    }
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    }

    // close the file
    fclose (f);

    // do we have the correct number of cards?
    if (count_cards() != DECK_NUM_CARDS) {
	// no. warn the user
	WPRINTF ("Warning: DECK::load(): file '%s' does not have %u cards, but only %u\n", fname, DECK_NUM_CARDS, count_cards());
    }

    // it worked
    return 0;
}
