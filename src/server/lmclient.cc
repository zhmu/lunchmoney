/*
 * LunchMoney 2.0 Daemon - lmclient.cc (LunchMoney Game Client)
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
#include <stdlib.h>
#include <string.h>
#include "lmclient.h"
#include "lmoney.h"
#include "protocol.h"
#include "tools.h"

/*
 * LMCLIENT::LMCLIENT()
 *
 * This will be called whenever a new client connects.
 *
 */
LMCLIENT::LMCLIENT() {
    // we don't have a player or game, yet
    player = NULL; game = NULL;
}

/*
 * LMCLIENT::on_connect()
 *
 * This will be called whenever a new connection is made.
 *
 */
void
LMCLIENT::on_connect() {
    // say hi to the client
    send_packet (PROTOCOL_RESP_MSG_HELLO);

    // logging
    DPRINTF (DEBUG_INFO, "LMCLIENT()::on_connect(): new connection from %s", get_address());
}

/*
 * LMCLIENT::on_disconnect()
 *
 * This will be called whenever the client has disconnected.
 *
 */
void
LMCLIENT::on_disconnect() {
    // logging
    DPRINTF (DEBUG_INFO, "LMCLIENT()::on_disconnect(): connection to %s lost", get_address());
}

/*
 * LMCLIENT::~LMCLIENT()
 *
 * This will be called whenever a new client closes the connection.
 *
 */
LMCLIENT::~LMCLIENT() {
    // is a game started?
    if (game) {
	// yes. remove this player from the game
	game->remove_player (player);

	// logging
	DPRINTF (DEBUG_INFO, "LMCLIENT::~LMCLIENT(): player '%s' left game '%s'", player->get_name(), game->get_name());

	// does the game have any players left?
	if (game->get_player_count() == 0) {
	    // no. log this first
	    DPRINTF (DEBUG_INFO, "LMCLIENT::~LMCLIENT(): game '%s' removed because there are no players left", game->get_name());

	    // remove the game
	    lmoney->playground->remove_game (game);

	    // delete the game
	    delete game;

	    // no more game
	    game = NULL;
	}
    }

    // is a player attached to this client?
    if (player) {
	// yes. remove the player from the crowd
	lmoney->crowd->remove_player (player);

	// debugging
	DPRINTF (DEBUG_INFO, "LMCLIENT::~LMCLIENT(): player '%s' left the game", player->get_name());

	// bye bye
	delete player;
    }

    // bye
    shutdown();
}

/*
 * LMCLIENT::send_packet (char* data)
 *
 * This will send string [data] to the client, with a newline attached to it.
 *
 */
void
LMCLIENT::send_packet (char* data) {
    char packet[NET_MAX_PACKET_LEN];

    // add a newline
    snprintf (packet, NET_MAX_PACKET_LEN, "%s\n", data);

    // off it goes
    NETCLIENT::send_packet (packet, strlen (packet));
}

/*
 * LMCLIENT::handle_packet (char* packet, int len)
 *
 * This will handle up to [len] bytes of packet [packet].
 *
 */
void
LMCLIENT::handle_packet (char* packet, int len) {
    char* arg;

    // make sure the packet is properly zero-terminated
    packet[len--] = 0;

    // zap any newlines
    while ((packet[len] == 0x0a) || (packet[len] == 0x0d))
	packet[len--] = 0;

    // do we have arguments?
    arg = strchr (packet, ' ');
    if (arg != NULL) {
	// yes. isolate them
	*arg = 0; arg++;

	// skip over any whitespace
	while (*arg == ' ')
	    arg++;
    } else
	// no. set [arg] to the nul then
	arg = strchr (packet, 0);

    // heavy debugging
    DPRINTF (DEBUG_INFO, "LMCLIENT::handle_packet(): packet [%s %s]", packet, arg);

    // let the protocol code handle it
    PROTOCOL::handle_command (this, packet, arg);
}
