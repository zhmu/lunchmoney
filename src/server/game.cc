/*
 * LunchMoney 2.0 Daemon - game.cc (LunchMoney Game)
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
#include "deck.h"
#include "game.h"
#include "player.h"
#include "protocol.h"
#include "lmoney.h"

/*
 * GAME::GAME (int xid, char* xname, int xnum)
 *
 * This will initialize a blank game with id [xid], name [xname] for [xnum]
 * players.
 *
 */
GAME::GAME(int xid, char* xname, int xnum) { 
    int i;

    // no players, nothing
    for (i = 0; i < GAME_MAX_PLAYERS; i++)
	player[i] = NULL;
    next = NULL; started = 0; curplayer = 0; curdefender = -1;
    curcard = NULL; curstatus = 0; curextra = 0;

    // copy the id and name
    id = xid; numplayers = xnum;
    strncpy (name, xname, GAME_MAX_NAME_LEN);

    // create a deck and shuffle it
    deck = new DECK();
    lmoney->deck->clone (deck);
    deck->shuffle();
}

/*
 * GAME::~GAME()
 *
 * This will deinitialize the game and free all the memory.
 *
 */
GAME::~GAME() {
    // do we have a deck?
    if (deck)
	// yes. get rid of it
	delete deck;
}

/*
 * GAME::set_[xxx])([y])
 *
 * This will set the [xxx] property to [y].
 *
 */
void	GAME::set_next (GAME* y) { next = y; }

/*
 * GAME::get_[xxx]()
 *
 * This will return the [xxx] property.
 *
 */
int	GAME::get_id() { return id; }
int	GAME::get_numplayers() { return numplayers; }
char*	GAME::get_name() { return name; }
PLAYER*	GAME::get_player(int i) { return player[i]; }
GAME*	GAME::get_next() { return next; }
int	GAME::has_started() { return started; }
PLAYER* GAME::get_cur_player() { return player[curplayer]; }
DECK*	GAME::get_deck() { return deck; }
PLAYER*	GAME::get_defender() { return (curdefender != -1) ? player[curdefender] : NULL; }
CARD*	GAME::get_card() { return curcard; }


/*
 * GAME::add_player (PLAYER* p)
 *
 * This will add [p] as a player of this game.
 *
 */
void
GAME::add_player (PLAYER* p) {
    char packet[NET_MAX_PACKET_LEN]; 
    int i = 0;

    // find a free spot
    while (i < GAME_MAX_PLAYERS) {
	// empty?
	if (player[i] == NULL)
	    // yes. break out
	    break;

	// next
	i++;
    }

    // did we have an empty spot?
    if (i == GAME_MAX_PLAYERS)
	// no. leave
	return;

    // inform everyone about this new player
    snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_RESP_MSG_PLAYERJOIN,
		      p->get_id());
    send_packet (packet);

    // finally, add the player
    player[i] = p;
}

/*
 * GAME::remove_player (PLAYER* p)
 *
 * This will remove [p] from the game.
 *
 */
void
GAME::remove_player (PLAYER* p) {
    int i = 0;
    char packet[NET_MAX_PACKET_LEN]; 

    // find the position
    while (i < GAME_MAX_PLAYERS) {
	// found the player?
	if (player[i] == p)
	    // yes. mark the spot as unused
	    player[i] = NULL;

	// next
	i++;
    }

    // inform everyone about the departure
    snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_RESP_MSG_PLAYERLEAVE,
		      p->get_id());
    send_packet (packet);
}

/*
 * GAME::get_player_count()
 *
 * This will count the number of players in the game and return that number.
 *
 */
int
GAME::get_player_count() {
    int i = 0, count = 0;

    // count them all
    while (i < GAME_MAX_PLAYERS) {
	// got a player?
	if (player[i] != NULL)
	    // yes. increment the counter
	    count++;

	// next
	i++;
    }

    // return the counter
    return count;
}

/*
 * GAME::join_game (PLAYER* p)
 *
 * This will cause player [p] to join this game. This will take care of
 * settings everything up as needed. This will return zero on success or -1
 * on failure.
 * 
 */
int
GAME::join_game (PLAYER* p) {
    // is the game full? 
    if (get_player_count() == numplayers)
	// yes. failure
	return -1;

    // add the player
    add_player (p);

    // all done
    return 0;
}

/*
 * GAME::send_packet (char* packet)
 *
 * This will send packet [packet] to everyone in the game.
 *
 */
void
GAME::send_packet (char* packet) {
    int i = 0;

    // handle all players
    while (i < GAME_MAX_PLAYERS) {
	// got a player?
	if (player[i] != NULL)
	    // yes. send the player
	    player[i]->get_client()->send_packet (packet);

	// next
	i++;
    }
}

/*
 * GAME::start()
 *
 * This will attempt to start the game, if needed.
 *
 */
void
GAME::start() {
    // is the game full?
    if (get_player_count() != numplayers)
	// no. bye
	return;

    // set the started flag
    started = 1;

    // give everyone their cards
    give_cards();

    // tell everyone we're
    send_packet (PROTOCOL_RESP_MSG_GAMESTARTS);
}

/*
 * GAME::give_cards()
 *
 * This will deal out all cards as needed.
 *
 */
void
GAME::give_cards() {
    int i, j;

    // handle all players
    for (i = 0; i < GAME_MAX_PLAYERS; i++)
	// valid player?
	if (player[i] != NULL) {
	    // yes. handle the cards
	    for (j = 0; j < PLAYER_MAX_CARDS; j++)
		// got a card?
		if (player[i]->get_card (j) == NULL)
		    // no. give the player a card
		    player[i]->set_card (j, deck->fetch());
	}
}

/*
 * GAME::next_player()
 *
 * This will give the turn to the next player.
 *
 */
void
GAME::next_player() {
    // increment the current player
    curplayer++;
    if (curplayer == GAME_MAX_PLAYERS)
	curplayer = 0;

    // ensure we have a valid player
    while ((player[curplayer] == NULL) && (curplayer != GAME_MAX_PLAYERS))
	curplayer++;

    // reached the end?
    if (curplayer == GAME_MAX_PLAYERS) {
	// yes. start over
        curplayer = 0;
        while (player[curplayer] == NULL)
	    curplayer++;
    }

    // logging
    DPRINTF (DEBUG_INFO, "GAME::next_player(): it is player %s's turn now", player[curplayer]->get_name());

    // inform the player
    player[curplayer]->get_client()->send_packet (PROTOCOL_RESP_MSG_YOURTURN);
}

/*
 * GAME::find_player (PLAYER* p)
 *
 * This will return the position of player [p], or -1 if there is no such
 * player.
 *
 */
int
GAME::find_player (PLAYER* p) {
    int i;

    // browse all players
    for (i = 0; i < GAME_MAX_PLAYERS; i++)
	// match?
	if (player[i] == p)
	    // yes. return the player
	    return i;

    // no such player
    return -1;
}


/*
 * GAME::get_player_by_id (int id)
 *
 * This will scan for a player with ID [id[. It will return the player on
 * success or NULL on failure.
 *
 */
PLAYER*
GAME::get_player_by_id(int id) {
    int i;

    // browse all players
    for (i = 0; i < GAME_MAX_PLAYERS; i++)
	// got a player here?
	if (player[i] != NULL)
	    // yes. got an id match?
	    if (player[i]->get_id() == id)
		// yes. return the player
		return player[i];

    // no such player
    return NULL;
}

/*
 * GAME::action (CARD* c, PLAYER* attacker, PLAYER* defender, int extra)
 *
 * This will handle player [attacker] playing card [c] on player [victim]
 * with flags [extra].
 *
 */
void
GAME::action (CARD* c, PLAYER* attacker, PLAYER* victim, int extra) {
    char packet[NET_MAX_PACKET_LEN];
    int  cardpos;

    // find the card
    cardpos = attacker->find_card (c);
    if (cardpos < 0)
	// no such card. complain
	return;

    // remove this card from the player's hand and get a new one
    attacker->set_card (cardpos, deck->fetch());

    // tell the player this went ok
    attacker->get_client()->send_packet (PROTOCOL_RESP_MSG_CARDPLAYED);

    // tell everyone what happened
    snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_RESP_MSG_ACTION,
	      attacker->get_id(), victim->get_id(), c->get_type(), extra);
    send_packet (packet);

    // can we defend?
    if (victim->can_defend()) {
	// yes. set the game defender, status and card
	curdefender = find_player (victim);
	curcard = c;
	curextra = extra;

	// inform the player he has a chance of survival
	victim->get_client()->send_packet (PROTOCOL_RESP_MSG_DEFEND);
    } else {
	// no. next player
	next_player();
    }

    // debugging
    DPRINTF (DEBUG_INFO, "GAME::action(): player '%s' played a '%s' on '%s' [flags %u]", attacker->get_name(), c->get_name(), victim->get_name(), extra);
}

/*
 * GAME::defend (CARD* c)
 *
 * This will try to defend with a [c] card.
 *
 */
void
GAME::defend (CARD* c) {
    char packet[NET_MAX_PACKET_LEN];
    int  cardpos;
    PLAYER* defender = get_defender();

    // got a defender?
    if (defender == NULL)
	// no. complain
	return;

    // find the card
    cardpos = defender->find_card (c);
    if (cardpos < 0)
	// no such card. complain
	return;

    // remove this card from the player's hand and get a new one
    DPRINTF (DEBUG_INFO, "GAME::defend(): need to discard %u", cardpos);
    defender->set_card (cardpos, deck->fetch());

    // tell the player this went ok
    defender->get_client()->send_packet (PROTOCOL_RESP_MSG_CARDPLAYED);

    // tell everyone what happened
    snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_RESP_MSG_DEFENDED,
	      defender->get_id(), c->get_type());
    send_packet (packet);

    // no more defender
    curdefender = -1; curcard = NULL; curextra = 0;

    // next player
    next_player();
}

/*
 * GAME::idle(PLAYER* p)
 *
 * This will cause player [p] to idle.
 *
 */
void
GAME::idle(PLAYER* p) {
    char packet[NET_MAX_PACKET_LEN];

    // is it this player's turn?
    if (get_cur_player() == p) {
	// yes. tell the user the idling went ok
	p->get_client()->send_packet (PROTOCOL_RESP_MSG_IDLEOK);

	// next plyer is on the move now
	next_player();

	// cya
	return;
    }

    // do we need to defend?
    if (get_defender() == p) {
	// yes. tell the user the idling went ok
	p->get_client()->send_packet (PROTOCOL_RESP_MSG_IDLEOK);

	// tell everyone about this user's sloppyness
	snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_RESP_MSG_IDLED,
		  get_defender()->get_id());
	send_packet (packet);

	// no one is defending now
	curdefender = -1;

	// next turn
	next_player();

	// cya
	return;
    }

    // it's not our turn. complain
    p->get_client()->send_packet (PROTOCOL_RESP_ERR_NOTURN);
}
