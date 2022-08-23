/*
 * LunchMoney 2.0 Daemon - protocol.cc (LunchMoney Protocol)
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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "lmclient.h"
#include "lmoney.h"
#include "network.h"
#include "protocol.h"
#include "tools.h"

/* protocol_cmdtab[] is the table of all supported commands */
PROTOCOL_CMD protocol_cmdtab[] = {
		{ "QUIT",		&PROTOCOL::cmd_quit },
		{ "PLAYER",		&PROTOCOL::cmd_player },
		{ "NEWGAME",		&PROTOCOL::cmd_newgame },
		{ "JOINGAME",		&PROTOCOL::cmd_joingame },
		{ "LISTGAMES",		&PROTOCOL::cmd_listgames },
		{ "PLAYERINFO",		&PROTOCOL::cmd_playerinfo },
		{ "GETCARDS",		&PROTOCOL::cmd_getcards },
		{ "ACTION",		&PROTOCOL::cmd_action },
		{ "DEFEND",		&PROTOCOL::cmd_defend },
		{ "IDLE",		&PROTOCOL::cmd_idle },
		{ "DROP",		&PROTOCOL::cmd_drop },
		{ NULL,	NULL }
	};

/*
 * PROTOCOL::handle_command (LMCLIENT* lm, char* cmd, char* arg)
 *
 * This will handle command [cmd] with arguments [arg] for client [lm].
 *
 */
void
PROTOCOL::handle_command (LMCLIENT* lm, char* cmd, char* arg) {
    PROTOCOL_CMD* pcmd = (PROTOCOL_CMD*)&protocol_cmdtab;

    // scan all commands
    while (pcmd->cmd) {
	// match?
	if (!strcasecmp (cmd, pcmd->cmd)) {
	    // yes. call the handler
	    pcmd->func (lm, arg);

	    // bye bye
	    return;
	}

	// next
	pcmd++;
    }

    // what is this?
    PROTOCOL::cmd_unknown (lm, arg);
}

/*
 * PROTOCOL::cmd_unknown (LMCLIENT* lm, char* arg)
 *
 * This will handle an unknown command with argument [arg] from [lm].
 *
 */
void
PROTOCOL::cmd_unknown (LMCLIENT* lm, char* arg) {
    lm->send_packet (PROTOCOL_RESP_ERR_UNKCMD);
}

/*
 * PROTOCOL::cmd_quit (LMCLIENT* lm, char* arg)
 *
 * This will handle a quit command with argument [arg] from [lm].
 *
 */
void
PROTOCOL::cmd_quit (LMCLIENT* lm, char* arg) {
    // tell the client it's bye bye time
    lm->send_packet (PROTOCOL_RESP_MSG_BYE);

    // get rid of the client
    lm->shutdown();
}

/*
 * PROTOCOL::cmd_player (LMCLIENT* lm, char* arg)
 *
 * This will handle a player command with argument [arg] from [lm].
 *
 */
void
PROTOCOL::cmd_player (LMCLIENT* lm, char* arg) {
    char* ptr = arg;

    // is the player already set?
    if (lm->player != NULL) {
	// yes. complain
	lm->send_packet (PROTOCOL_RESP_ERR_PLAYERSET);
	return;
    }

    // is a name given at all?
    if (!*arg) {
	// no. complain
	lm->send_packet (PROTOCOL_RESP_ERR_INVARG);
	return;
    }

    // is the player name valid?
    while (*ptr) {
	// digit, number or _?
	if ((!isdigit (*ptr)) && (!isalpha (*ptr) && (*ptr != '_'))) {
	    // no. complain
	    lm->send_packet (PROTOCOL_RESP_ERR_INVARG);
	    return;
	}

	// next
	ptr++;
    }
    
    // does this name look familiar?
    if (lmoney->crowd->find_player_by_name (arg) != NULL) {
	// yes. reject the duplicate name
	lm->send_packet (PROTOCOL_RESP_ERR_DUPNAME);
	return;
    }

    // create a new player
    lm->player = lmoney->crowd->create_player (lm, arg);

    // all set
    lm->send_packet (PROTOCOL_RESP_MSG_PLAYEROK);

    // debugging
    DPRINTF (DEBUG_INFO, "PROTOCOL::cmd_player(): player '%s' joined the game", lm->player->get_name());
}

/*
 * PROTOCOL::cmd_newgame (LMCLIENT* lm, char* arg)
 *
 * This will handle a new game command with argument [arg] from [lm].
 *
 */
void
PROTOCOL::cmd_newgame (LMCLIENT* lm, char* arg) {
    char name[NET_MAX_PACKET_LEN];
    int	 numplayers;

    // is the player name set?
    if (lm->player == NULL) {
	// no. complain
	lm->send_packet (PROTOCOL_RESP_ERR_PLAYERNOTSET);
	return;
    }

    // are we already inside a game?
    if (lm->game != NULL) {
	// yes. complain
	lm->send_packet (PROTOCOL_RESP_ERR_ALREADYINGAME);
	return;
    }

    // fetch the arguments
    if (format (arg, "%s,%i", name, &numplayers) < 0) {
	// this failed. complain
	lm->send_packet (PROTOCOL_RESP_ERR_INVARG);
	return;
    }

    // is the number of players acceptable?
    if ((numplayers < 2) || (numplayers > GAME_MAX_PLAYERS)) {
	// no. complain
	lm->send_packet (PROTOCOL_RESP_ERR_INVARG);
	return;
    }

    // add the game
    lm->game = lmoney->playground->create_game (name, numplayers);

    // join the game
    lm->game->add_player (lm->player);

    // all set
    lm->send_packet (PROTOCOL_RESP_MSG_GAMEOK);

    // debugging
    DPRINTF (DEBUG_INFO, "PROTOCOL::cmd_player(): player '%s' started a new game '%s' with %u players", lm->player->get_name(), lm->game->get_name(), lm->game->get_numplayers());
}

/*
 * PROTOCOL::cmd_listgames (LMCLIENT* lm, char* arg)
 *
 * This will handle sending a game list to [lm].
 *
 */
void
PROTOCOL::cmd_listgames (LMCLIENT* lm, char* arg) {
    char packet[NET_MAX_PACKET_LEN]; 
    GAME* game = lmoney->playground->get_games();

    // while we have a game, send the info
    while (game) {
	// build the packet and send it
	snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_RESP_MSG_GAMEINFO,
		  game->get_id(), game->get_name(), game->get_numplayers(),
		  game->get_player_count());
	lm->send_packet (packet);

	// next
	game = game->get_next();
    }

    // all sent
    lm->send_packet (PROTOCOL_RESP_MSG_GAMELISTSENT);

    // debugging
    DPRINTF (DEBUG_INFO, "PROTOCOL::cmd_listgames(): game list request answered");
}

/*
 * PROTOCOL::cmd_playerinfo (LMCLIENT* lm, char* arg)
 *
 * This will handle a request for information with argument [arg] for [lm].
 *
 */
void
PROTOCOL::cmd_playerinfo (LMCLIENT* lm, char* arg) {
    char packet[NET_MAX_PACKET_LEN]; 
    int playerno;
    PLAYER* p;

    // format the arguments
    if (format (arg, "%i", &playerno) < 0) {
	// this failed. complain
	lm->send_packet (PROTOCOL_RESP_ERR_INVARG);
	return;
    }

    // is the player number within range?
    if ((playerno < 1) || (playerno > 4)) {
	// this failed. complain
	lm->send_packet (PROTOCOL_RESP_ERR_INVARG);
	return;
    }

    // are we inside a game?
    if (lm->game == NULL) {
	// no. complain
	lm->send_packet (PROTOCOL_RESP_ERR_MUSTBEINAGAME);
	return;
    }

    // fetch the player information
    p = lm->game->get_player (playerno - 1);
    if (p == NULL) {
	// this failed. complain
	lm->send_packet (PROTOCOL_RESP_ERR_NOPLAYER);
	return;
    }

    // send the info
    snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_RESP_MSG_PLAYERINFO,
	      p->get_id(), p->get_name(), p->get_life());
    lm->send_packet (packet);

    // debugging
    DPRINTF (DEBUG_INFO, "PROTOCOL::cmd_playerinfo(): information about player %u requested", playerno);
}

/*
 * PROTOCOL::cmd_joingame (LMCLIENT* lm, char* arg)
 *
 * This will handle a join game command with argument [arg] from [lm].
 *
 */
void
PROTOCOL::cmd_joingame (LMCLIENT* lm, char* arg) {
    int gameid;
    GAME* g = NULL;

    // format the arguments
    if (format (arg, "%i", &gameid) < 0) {
	// this failed. complain
	lm->send_packet (PROTOCOL_RESP_ERR_INVARG);
	return;
    }

    // are we inside a game?
    if (lm->game != NULL) {
	// yes. complain
	lm->send_packet (PROTOCOL_RESP_ERR_ALREADYINGAME);
	return;
    }

    // fetch the game info
    g = lmoney->playground->find_game_by_id (gameid);
    if (g == NULL) {
	// there is no such game. complain
	lm->send_packet (PROTOCOL_RESP_ERR_NOSUCHGAME);
	return;
    }

    // has this game already started?
    if (g->has_started()) {
	// yes. complain
	lm->send_packet (PROTOCOL_RESP_ERR_GAMESTARTED);
	return;
    }

    // tell the player it went ok
    lm->send_packet (PROTOCOL_RESP_MSG_JOINOK);

    // add the player
    g->add_player (lm->player);

    // try to start the game
    g->start();

    // assign the game to the user
    lm->game = g;

    // debugging
    DPRINTF (DEBUG_INFO, "PROTOCOL::cmd_joingame(): player '%s' joined game '%s'", lm->player->get_name(), lm->game->get_name());
}

/*
 * PROTOCOL::cmd_getcards (LMCLIENT* lm, char* arg)
 *
 * This will handle a get cards command from [lm].
 *
 */
void
PROTOCOL::cmd_getcards (LMCLIENT* lm, char* arg) {
    char packet[NET_MAX_PACKET_LEN]; 
    CARD* c;
    int i;

    // do we have any arguments?
    if (*arg) {
	// yes. complain
	lm->send_packet (PROTOCOL_RESP_ERR_INVARG);
	return;
    }

    // are we inside a game?
    if (lm->game == NULL) {
	// no. complain
	lm->send_packet (PROTOCOL_RESP_ERR_MUSTBEINAGAME);
	return;
    }

    // send the player the cards
    for (i = 0; i < PLAYER_MAX_CARDS; i++) {
	// grab the card
	c = lm->player->get_card (i);
	if (c != NULL) {
	    // this worked. send the info
	    snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_RESP_MSG_CARDINFO,
		      c->get_type(), c->get_name());
	    lm->send_packet (packet);

	    // debugging
	    DPRINTF (DEBUG_INFO, "PROTOCOL::cmd_getcards(): player '%s' has a '%s'", lm->player->get_name(), c->get_name());
	}
    }

    // all done
    lm->send_packet (PROTOCOL_RESP_MSG_CARDSOK);

    // debugging
    DPRINTF (DEBUG_INFO, "PROTOCOL::cmd_getcards(): player '%s' requested cards", lm->player->get_name());
}

/*
 * PROTOCOL::cmd_action (LMCLIENT* lm, char* arg)
 *
 * This will handle action [arg] from [lm].
 *
 */
void
PROTOCOL::cmd_action (LMCLIENT* lm, char* arg) {
    int		cardid, playerid, extra;
    PLAYER*	p;
    CARD*	c;
    int		cardpos;

    // are we inside a game?
    if (lm->game == NULL) {
	// no. complain
	lm->send_packet (PROTOCOL_RESP_ERR_MUSTBEINAGAME);
	return;
    }

    // fetch the arguments
    if (format (arg, "%i,%i,%i", &cardid, &playerid, &extra) < 0) {
	// yes. complain
	lm->send_packet (PROTOCOL_RESP_ERR_INVARG);
	return;
    }

    // is the player id within range?
    if ((playerid < 1) || (playerid > 4)) {
	// no. complain
	lm->send_packet (PROTOCOL_RESP_ERR_INVARG);
	return;
    }

    // is it our turn?
    if (lm->game->get_cur_player() != lm->player) {
	// no. complain
	lm->send_packet (PROTOCOL_RESP_ERR_NOTURN);
	return;
    }

    // get the player
    p = lm->game->get_player_by_id (playerid);
    if (p == NULL) {
	// this player is invalid. complain
	lm->send_packet (PROTOCOL_RESP_ERR_NOPLAYER);
	return;
    }

    // get the card
    cardpos = lm->player->scan_card (cardid);
    if (cardpos < 0) {
	// the player does not have the card. complain
	lm->send_packet (PROTOCOL_RESP_ERR_NOCARD);
	return;
    }
    c = lm->player->get_card (cardpos);

    // does the player try to hurt himself?
    if (p == lm->player) {
	// yes. complain
	lm->send_packet (PROTOCOL_RESP_ERR_CANTHURTSELF);
	return;
    }

    // let the game figure it out
    lm->game->action (c, lm->player, p, extra);
}

/*
 * PROTOCOL::cmd_defend (LMCLIENT* lm, char* arg)
 *
 * This will handle defense [arg] from [lm].
 *
 */
void
PROTOCOL::cmd_defend (LMCLIENT* lm, char* arg) {
    int		cardid;
    CARD*	c;
    int		cardpos;

    // are we inside a game?
    if (lm->game == NULL) {
	// no. complain
	lm->send_packet (PROTOCOL_RESP_ERR_MUSTBEINAGAME);
	return;
    }

    // fetch the arguments
    if (format (arg, "%i", &cardid) < 0) {
	// yes. complain
	lm->send_packet (PROTOCOL_RESP_ERR_INVARG);
	return;
    }

    // are we defending?
    if (lm->game->get_defender() != lm->player) {
	// no. complain
	lm->send_packet (PROTOCOL_RESP_ERR_NOTURN);
	return;
    }

    // get the card
    cardpos = lm->player->scan_card (cardid);
    if (cardpos < 0) {
	// the player does not have the card. complain
	lm->send_packet (PROTOCOL_RESP_ERR_NOCARD);
	return;
    }
    c = lm->player->get_card (cardpos);

    // let the game figure it out
    lm->game->defend (c);
}

/*
 * PROTOCOL::cmd_idle (LMCLIENT* lm, char* arg)
 *
 * This will handle an idle command from [lm].
 *
 */
void
PROTOCOL::cmd_idle (LMCLIENT* lm, char* arg) {
    // are we inside a game?
    if (lm->game == NULL) {
	// no. complain
	lm->send_packet (PROTOCOL_RESP_ERR_MUSTBEINAGAME);
	return;
    }

    // handle the idle command
    lm->game->idle (lm->player);
}

/*
 * PROTOCOL::cmd_drop (LMCLIENT* lm, char* arg)
 *
 * This will handle card drop arguments [arg] from [lm].
 *
 */
void
PROTOCOL::cmd_drop (LMCLIENT* lm, char* arg) {
    int		mask;

    // are we inside a game?
    if (lm->game == NULL) {
	// no. complain
	lm->send_packet (PROTOCOL_RESP_ERR_MUSTBEINAGAME);
	return;
    }

    // fetch the arguments
    if (format (arg, "%i", &mask) < 0) {
	// yes. complain
	lm->send_packet (PROTOCOL_RESP_ERR_INVARG);
	return;
    }

    // is it our turn?
    if ((lm->game->get_cur_player() != lm->player) || (lm->game->get_defender() != NULL)) {
	// no. complain
	lm->send_packet (PROTOCOL_RESP_ERR_NOTURN);
	return;
    }


    // claim we did it
    lm->send_packet (PROTOCOL_RESP_MSG_DROPOK);

    // drop the cards as needed
    if (mask & 16) lm->player->drop_card (4);
    if (mask &  8) lm->player->drop_card (3);
    if (mask &  4) lm->player->drop_card (2);
    if (mask &  2) lm->player->drop_card (1);
    if (mask &  1) lm->player->drop_card (0);

    // next player
    lm->game->next_player();
}
