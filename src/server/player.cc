/*
 * LunchMoney 2.0 Daemon - player.cc (LunchMoney Player)
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
#include <string.h>
#include "config.h"
#include "lmclient.h"
#include "player.h"
#include "protocol.h"

/*
 * PLAYER::PLAYER (LMCLIENT* c, int xid, char* xname)
 *
 * This will initialize a new player from client [c], with id [xid] and
 * name [xname].
 *
 */
PLAYER::PLAYER (LMCLIENT* c, int xid, char* xname) {
    int i;

    // just copy the id and name, and set everything else to defaults
    id = xid; life = CONF_INITIAL_LIFE; lm = c; next = NULL;
    strncpy (name, xname, PLAYER_MAX_NAME_LEN); defend = 1;

    // reset the cards
    for (i = 0; i < PLAYER_MAX_CARDS; i++)
	card[i] = NULL;
}

/*
 * PLAYER::set_[xxx])([y])
 *
 * This will set the [xxx] property to [y].
 *
 */
void	PLAYER::set_next (PLAYER* y) { next = y; }
void	PLAYER::set_card (int y, CARD* c) { card[y] = c; }
void	PLAYER::set_defend (int y) { defend = y; }

/*
 * PLAYER::get_[xxx]()
 *
 * This will return the [xxx] property.
 *
 */
int		PLAYER::get_id() { return id; }
char*		PLAYER::get_name() { return name; }
int		PLAYER::get_life() { return life; }
PLAYER*		PLAYER::get_next() { return next; }
LMCLIENT*	PLAYER::get_client() { return lm; }
CARD*		PLAYER::get_card(int y) { return card[y]; }

/*
 * PLAYER::scan_card (int type)
 *
 * This will scan for card [type]. It will return the card's position if the
 * card is found or -1 on failure.
 *
 */
int
PLAYER::scan_card (int type) {
    int i;

    // scan all cards
    for (i = 0; i < PLAYER_MAX_CARDS; i++)
	// got a card?
	if (card[i] != NULL)
	    // yes. match?
	    if (card[i]->get_type() == type)
		// yes. return the position
		return i;

    // no such card
    return -1;
}

/*
 * PLAYER::find_card (CARD* c)
 *
 * This will scan for card [c]. It will return the card's position if the
 * card is found or -1 on failure.
 *
 */
int
PLAYER::find_card (CARD* c) {
    int i;

    // scan all cards
    for (i = 0; i < PLAYER_MAX_CARDS; i++)
	// match?
	if (card[i] == c)
	    // yes. return the position
	    return i;

    // no such card
    return -1;
}

/*
 * PLAYER::can_defend()
 *
 * This will return non-zero if the player can defend, otherwise zero.
 */
int PLAYER::can_defend() { return defend; }

/*
 * PLAYER::drop_card (int no)
 *
 * This will cause the player to drop card [no].
 *
 */
void
PLAYER::drop_card (int no) {
    char packet[NET_MAX_PACKET_LEN];

    // are we in a game?
    if (lm->game == NULL)
	// no. leave
	return;

    // inform everyone the card is gone
    snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_RESP_MSG_DROPPED,
	      id, get_card (no)->get_type());
    lm->game->send_packet (packet);

    // away it goes
    set_card (no, lm->game->get_deck()->fetch());
}
