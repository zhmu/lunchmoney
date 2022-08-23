/*
 * LunchMoney 2.0 Daemon - card.cc (LunchMoney Card)
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
    name = strdup (cardname);
    type = cardtype;
    cat = cardcat;
    damage = carddamage;
}

/*
 * CARD::CARD (CARD* source)
 *
 * This will duplicate [source] and return a new card identical to it.
 * 
 */
CARD::CARD (CARD* source) {
    name = source->get_name();
    type = source->get_type();
    cat = source->get_cat();
    damage = source->get_damage();
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

/*
 * CARD::is_defensive()
 *
 * This will return 0 if the card is not defensive, otherwise 1.
 *
 */
int
CARD::is_defensive() {
    // defensive cards are, as you might have guessed, ok :)
    if (cat == CARD_CAT_DEFENSE)
	return 1;

    // humilations are defensive too
    if (type == CARD_TYPE_HUMILIATION)
	return 1;

    // anything else is not
    return 0;
}
