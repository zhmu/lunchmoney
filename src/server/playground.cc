/*
 * LunchMoney 2.0 Daemon - playground.cc (LunchMoney Playground)
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
#include "game.h"
#include "player.h"
#include "playground.h"

/*
 * PLAYGROUND::PLAYGROUND()
 *
 * This will initialize an empty playground.
 *
 */
PLAYGROUND::PLAYGROUND() {
    // no games, yet
    games = NULL; gameid = 1;
}

/*
 * PLAYGROUND::add_game (GAME* g)
 *
 * This will add game [g] to the playground.
 *
 */
void
PLAYGROUND::add_game (GAME* g) {
    GAME* game = games;

    // scan for the end of the chain
    if (game)
	while (game) game = game->get_next();

    // add the new game
    if (game)
	game->set_next (g);
    else
	games = g;
}

/*
 * PLAYGROUND::remove_game (GAME* g)
 *
 * This will remove [g] from the playground. It will *not* delete the game.
 *
 */
void
PLAYGROUND::remove_game (GAME* g) {
    GAME* game = games;
    GAME* prevgame = NULL;

    // scan the games
    while (game) {
	// match?
	if (game == g) {
	    // yes. did we have a previous one?
	    if (prevgame)
		// yes. update the previous game's next game (whoa...)
		prevgame->set_next (g->get_next());
	    else
		// no. this must be the initial first game. change the pointer
		games = g->get_next();

	    // all set
	    return;
	}

	// next
	prevgame = game; game = game->get_next();
    }

    // no game to remove
}

/*
 * PLAYGROUND::find_game_by_id (int id)
 *
 * This will scan the list of games for a game with id [id]. It will return the
 * game object on success or NULL on failure.
 *
 */
GAME*
PLAYGROUND::find_game_by_id (int id) {
    GAME* game = games;

    // scan them all
    while (game) {
	// match?
	if (game->get_id() == id)
	    // yes. return the game
	    return game;

	// next
	game = game->get_next();
    }

    // no such game
    return NULL;
}

/*
 * PLAYGROUND::create_game (char* name, int num)
 *
 * This will create a new game called [name] for [num] players  and give it the
 * ID it needs. The game will also be added to the playground. This will return
 * a pointer to the new game.
 *
 */
GAME*
PLAYGROUND::create_game (char* name, int num) {
    GAME* g;

    // create the game
    g = new GAME (gameid, name, num);

    // add the game to the playground
    add_game (g);

    // increment the id and ensure it does not clash
    gameid++;
    while ((gameid == 0) || (find_game_by_id (gameid) != NULL))
	gameid++;

    // return the game
    return g;
}

/*
 * PLAYGROUND::get_games()
 *
 * This will return the first game we have, or NULL if there are no games. This
 * list can be walked using get_next().
 *
 */
GAME*
PLAYGROUND::get_games() {
    return games;
}
