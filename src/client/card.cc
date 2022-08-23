/*
 * LunchMoney Daemon - (c) 2002 Rink Springer, GPL
 *
 */
#include <stdio.h>
#include <string.h>
#include "card.h"

/*
 * CARD::CARD (char* cardname, int cardtype, int cardcat, int carddamage)
 *
 * This will create a card the information supplied.
 *
 */
CARD::CARD (char* cardname, int cardtype, int cardcat, int carddamage) {
    // set us up
    strncpy (name, cardname, CARD_MAX_NAME_LEN);
    type = cardtype;
    cat = cardcat;
    damage = carddamage;
}

/*
 * CARD::CARD ()
 *
 * This will create a blank card/
 *
 */
CARD::CARD () {
    // set us up
    name[0] = 0;
    type = -1;
    cat = -1;
    damage = 0;
}

/*
 * CARD::get_[xxx]()
 *
 * This will get the [xxx] field and return it.
 *
 */
char* CARD::get_name() { return name; }
int   CARD::get_type() { return type; }
int   CARD::get_damage() { return damage; }
int   CARD::get_cat() { return cat; }
