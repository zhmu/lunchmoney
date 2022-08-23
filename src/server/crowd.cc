/*
 * LunchMoney 2.0 Daemon - crowd.cc (LunchMoney Players Crowd)
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
#include "crowd.h"
#include "player.h"

/*
 * CROWD::CROWD()
 *
 * This will initialize an empty crowd (OK, so this does sound a bit stupid...)
 *
 */
CROWD::CROWD() {
    // no players, yet
    players = NULL; playerid = 1;
}

/*
 * CROWD::add_player (PLAYER* p)
 *
 * This will add player [p] to the crowd.
 *
 */
void
CROWD::add_player (PLAYER* p) {
    PLAYER* player = players;

    // scan for the end of the chain
    if (player)
	while (player) player = player->get_next();

    // add the new player
    if (player)
	player->set_next (p);
    else
	players = p;
}

/*
 * CROWD::remove_player (PLAYER* p)
 *
 * This will remove [p] from the crowd. It will *not* delete the player.
 *
 */
void
CROWD::remove_player (PLAYER* p) {
    PLAYER* player = players;
    PLAYER* prevplayer = NULL;

    // scan the players
    while (player) {
	// match?
	if (player == p) {
	    // yes. did we have a previous one?
	    if (prevplayer)
		// yes. update the previous client's next client (whoa...)
		prevplayer->set_next (p->get_next());
	    else
		// no. this must be the initial first player. change the pointer
		players = p->get_next();

	    // all set
	    return;
	}

	// next
	prevplayer = player; player = player->get_next();
    }

    // no player to remove
}

/*
 * CROWD::find_player_by_name (char* name)
 *
 * This will scan the list of players for a player called [name]. It will
 * return the player object on success or NULL on failure.
 *
 */
PLAYER*
CROWD::find_player_by_name (char* name) {
    PLAYER* player = players;

    // scan them all
    while (player) {
	// match?
	if (!strcasecmp (name, player->get_name()))
	    // yes. return the player
	    return player;

	// next
	player = player->get_next();
    }

    // no such player
    return NULL;
}

/*
 * CROWD::find_player_by_id (int id)
 *
 * This will scan the list of players for a player with id [id]. It will
 * return the player object on success or NULL on failure.
 *
 */
PLAYER*
CROWD::find_player_by_id (int id) {
    PLAYER* player = players;

    // scan them all
    while (player) {
	// match?
	if (player->get_id() == id)
	    // yes. return the player
	    return player;

	// next
	player = player->get_next();
    }

    // no such player
    return NULL;
}

/*
 * CROWD::create_player (LMCLIENT* lm, char* name)
 *
 * This will create a new player called [name] from [lm], and give it the ID it
 * needs. The player will also be added to the crowd. This will return a pointer
 * to the new player.
 *
 */
PLAYER*
CROWD::create_player (LMCLIENT* lm, char* name) {
    PLAYER* p;

    // create the player
    p = new PLAYER (lm, playerid, name);

    // add the player to the crowd
    add_player (p);

    // increment the id and ensure it does not clash
    playerid++;
    while ((playerid == 0) || (find_player_by_id (playerid) != NULL))
	playerid++;

    // return the player
    return p;
}
