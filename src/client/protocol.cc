/*
 * LunchMoney Client - (c) 2002 Rink Springer, GPL
 *
 */
#include <curses.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "command.h"
#include "lmoneyc.h"
#include "protocol.h"

/*
 * PROTOCOL::PROTOCOL()
 *
 * This will initialize our protocol object.
 *
 */
PROTOCOL::PROTOCOL() {
    int i;

    // initialize the client
    nc = new NETCLIENT();

    // create the players
    for (i = 1; i <= PROTOCOL_MAX_PLAYERS; i++)
	player[i] = new PLAYER();

    // build the cards
    for (i = 0; i < PROTOCOL_MAX_CARDS; i++)
	card[i] = NULL;
}

/*
 * PROTOCOL::~PROTOCOL()
 *
 * This will get rid of the protocol object.
 *
 */
PROTOCOL::~PROTOCOL() {
    int i;

    // destroy the players
    for (i = 1; i <= PROTOCOL_MAX_PLAYERS; i++)
	delete player[i];

    // a farewell to the client
    delete nc;
}

/*
 * PROTOCOL::connect (char* hostip, int portno)
 *
 * This will try to connect to host or IP address [hostip] with port [portno].
 * Any failure will be printed on the window as needed. This will return
 * 0 on success or -1 on failure.
 *
 */
int
PROTOCOL::connect (char* hostip, int portno) {
    char packet[NET_MAX_PACKET_LEN];

    // the game has, of course, not started
    started = success = fail = 0;

    // ensure the client is unconnected first
    nc->shutdown();

    // connect
    if (nc->connect (hostip, portno) < 0)
	// this failed. complain
	return -1;

    // wait for the welcome wagon message and read it
    nc->wait_for_data(packet);

    // accepted?
    if (packet[0] != '1') {
	// no. complain
	WPRINTF ("Server does not want us [%s]", packet);

	// bye bye
	nc->shutdown();
	return -1;
    }

    // enable incoming
    nc->enable_incoming();

    // all set
    return 0;
}

/*
 * PROTOCOL::is_connected()
 *
 * This will return 0 if we are connected or -1 if not.
 *
 */
int
PROTOCOL::is_connected() {
    return nc->is_connected();
}

/*
 * PROTOCOL::has_started()
 *
 * This will return zero if the game has not started, or non-zero if it has.
 *
 */
int
PROTOCOL::has_started() {
    return started;
}

/*
 * PROTOCOL::disconnect()
 *
 * This will disconnect us from the server.
 *
 */
void
PROTOCOL::disconnect() {
    char packet[NET_MAX_PACKET_LEN];

    // are we connected?
    if (is_connected() < 0)
	// no. just leave
	return;

    // we don't want incoming() to disturb us
    nc->disable_incoming();

    // tell the server it's bye bye time
    nc->send_packet (PROTOCOL_CMD_QUIT);

    // wait for the message (not that we care, we discard it anyway) 
    nc->wait_for_data(packet);

    // say bye bye to the socket
    nc->shutdown();
}

/*
 * PROTOCOL::set_player (char* name)
 *
 * This will tell the server our name is [name]. It will return zero on
 * success or -1 on failure.
 *
 */
int
PROTOCOL::set_player (char* name) {
    char packet[NET_MAX_PACKET_LEN];

    // are we connected?
    if (is_connected() < 0) {
	// no. complain
	WPRINTF ("We are not connected to a server");
	return -1;
    }

    // we don't want incoming() to disturb us
    nc->disable_incoming();

    // construct the command and send it
    snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_CMD_PLAYER, name);
    nc->send_packet (packet);

    // fetch the response
    nc->wait_for_data(packet);

    // incoming() is welcome to bug us again
    nc->enable_incoming();

    // accepted?
    if (packet[0] != '1') {
	// no. complain
	WPRINTF ("Player name [%s] not accepted by server: [%s]", name, packet + 4);
	return -1;
    }

    // store the name
    playername = strdup (name);

    // all set
    return 0;
}

/*
 * PROTOCOL::query_games()
 *
 * This will query the server for the games it has.
 *
 */
void
PROTOCOL::query_games() {
    // we don't want incoming() to disturb us
    nc->disable_incoming();

    // just send the packet. wow, that was *hard*! :)
    nc->send_packet (PROTOCOL_CMD_LISTGAMES);
}

/*
 * PROTOCOL::fetch_game(GAME* g)
 *
 * This will fetch the game information in [g]. This should be called after
 * query_games(). It will return 1 on success or 0 on failure.
 *
 */
int
PROTOCOL::fetch_game(GAME* g) {
    char  packet[NET_MAX_PACKET_LEN];
    char* ptr;
    char* ptr2;

    // wait for the data
    nc->wait_for_data(packet);

    // grab the code
    if (strncmp (packet, "101 ", 4)) {
	// this is a wrong packet. discard it
	nc->enable_incoming();
	return 0;
    }

    // scan for a ','
    ptr = strchr (packet, ',');
    if (ptr == NULL) {
	// yuck, broken packet. discard it
	nc->enable_incoming();
	return 0;
    }
    *ptr = 0; ptr++;

    // isolate the game id
    g->id = strtol (packet + 4, &ptr2, 10);
    if (*ptr2) {
	// there was junk after the number. discard the packet
	nc->enable_incoming();
	return 0;
    }

    // isolate the game name
    ptr2 = strchr (ptr, ',');
    if (ptr2 == NULL) {
	// yuck, broken packet. discard it
	nc->enable_incoming();
	return 0;
    }
    *ptr2 = 0; ptr2++;

    // fetch the game name
    strncpy (g->name, ptr, PROTOCOL_MAX_GAME_LEN);

    // isolate the number of players
    ptr = strchr (ptr2, ',');
    if (ptr == NULL) {
	// yuck, broken packet. discard it
	nc->enable_incoming();
	return 0;
    }
    *ptr = 0; ptr++;
    
    // fetch the number of players needed
    g->maxplayers = strtol (ptr2, &ptr2, 10);
    if (*ptr2) {
	// there was junk after the number. discard the packet
	nc->enable_incoming();
	return 0;
    }

    // fetch the current number of players
    g->numplayers = strtol (ptr, &ptr2, 10);
    if (*ptr2) {
	// there was junk after the number. discard the packet
	nc->enable_incoming();
	return 0;
    }

    // all set
    return 1;
}

/*
 * PROTOCOL::join_game (int id)
 *
 * This will try to join game [id]. It will return 0 on success or -1 on
 * failure.
 *
 */
int
PROTOCOL::join_game (int id) {
    char packet[NET_MAX_PACKET_LEN];

    // we don't want incoming() to disturb us
    nc->disable_incoming();

    // construct the command and send it
    snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_CMD_JOINGAME, id);
    nc->send_packet (packet);

    // send the command and wait for the response
    nc->wait_for_data (packet);

    // incoming() is welcome to bug us again
    nc->enable_incoming();

    // did it work?
    if (packet[0] != '1') {
	// no. complain
	WPRINTF ("Couldn't join game - [%s]", packet + 4);
	return -1;
    }

    // wait for a short while
    sleep (1);

    // ensure we don't have any incoming messages
    while (!incoming());

    // query the players
    query_players();

    // all went ok
    return 0;
}

/*
 * PROTOCOL::query_players()
 *
 * This will query the game for player info.
 *
 */
void
PROTOCOL::query_players() {
    char packet[NET_MAX_PACKET_LEN];
    int i, id, life;
    char* name;
    char* ptr;
    char* ptr2;

    // we don't want incoming() to disturb us
    nc->disable_incoming();

    // handle all players
    for (i = 1; i <= PROTOCOL_MAX_PLAYERS; i++) {
	// kill the player
	player[i]->set_id (0);

	// build and send the packet
	snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_CMD_PLAYERINFO, i);
	nc->send_packet (packet);

	// wait for a reply
evil:
	nc->wait_for_data (packet);

	// successful reply?
	if (packet[0] == '1') {
	    // yes. scan for a ','
	    ptr = strchr (packet, ',');
	    if (ptr != NULL) {
		// got it. isolate the id
		*ptr = 0; ptr++;
		id = strtol (packet + 4, &ptr2, 10);
		if (!*ptr2) {
		    // got it. scan for another ','
		    ptr2 = strchr (ptr, ',');
		    if (ptr2 != NULL) {
			// got it. isolate the name
			*ptr2 = 0; ptr2++;
			name = ptr;

			// isolate the life
			life = strtol (ptr2, &ptr, 10);
			if (!*ptr) {
			    // all set. set the player up
			    player[i]->set_id (id);
			    player[i]->set_life (life);
			    player[i]->set_name (name);
			}
		    }
		}
	    }
	} else {
	    // *not* an error?
	    if (packet[0] != '3') {
		/// nope. probably an incoming packet. deal with it
		handle_incoming (packet);

		// aaiiieee! how gross...
		goto evil;
	    }
	}
    }

    // ensure all data is gone
    nc->get_packet (packet);

    // incoming() is welcome to bug us again
    nc->enable_incoming();
}

/*
 * PROTOCOL::query_cards()
 *
 * This will fetch our playing cards.
 *
 */
void
PROTOCOL::query_cards() {
    int i, id;
    char packet[NET_MAX_PACKET_LEN];
    char* ptr;

    // we don't want incoming() to disturb us
    nc->disable_incoming();

    // query the card info
    nc->send_packet (PROTOCOL_CMD_GETCARDS);

    // handle all cards
    i = 0;
    while (i < PROTOCOL_MAX_CARDS) {
	// wait for a reply
	nc->wait_for_data (packet);
	ptr = packet;

	// handle the entire packet
	while (*ptr) {
	    // card information packet?
	    if (*ptr == '1') {
		// yes. isolate the id
		id = strtol (ptr + 4, &ptr, 10);
		if (*ptr == ',') {
		    // we got the card!
		    card[i] = lmoneyc->deck->get_card (id);
		    i++;
		}
	    }

	    // keep going now until we either have the end or we have a newline
	    while ((*ptr != 0) && (*ptr != '\n')) { ptr++; }

	    // skip over a newline
	    if (*ptr == '\n') ptr++;
	}
    }

    // incoming() is welcome to bug us again
    nc->enable_incoming();
}

/*
 * PROTOCOL::scan_card (int type)
 *
 * This will scan whether we have a [type] card. It will return the card if we
 * do, or NULL if we don't.
 *
 */
CARD*
PROTOCOL::scan_card (int type) {
    int i;

    // scan all cards
    for (i = 0; i < PROTOCOL_MAX_CARDS; i++)
	// match?
	if (card[i] != NULL)
	    if (card[i]->get_type() == type)
		// yes. we got it
		return card[i];

    // no match
    return NULL;
}

/*
 * PROTOCOL::scan_player(char* name)
 *
 * This will scan player [name] and return a pointer to the player. If the
 * player is not found, NULL will be returned.
 *
 */
PLAYER*
PROTOCOL::scan_player(char* name) {
    int i;

    // scan all players
    for (i = 1; i <= PROTOCOL_MAX_PLAYERS; i++)
	// match?
	if (!strcmp (player[i]->get_name(), name))
	    // yes. return the player
	    return player[i];

    // no such player
    return NULL;
}

/*
 * PROTOCOL::find_player(int id)
 *
 * This will scan player [id] and return a pointer to the player. If the
 * player is not found, NULL will be returned.
 *
 */
PLAYER*
PROTOCOL::find_player(int id) {
    int i;

    // scan all players
    for (i = 1; i <= PROTOCOL_MAX_PLAYERS; i++)
	// match?
	if (player[i]->get_id() == id)
	    // yes. return the player
	    return player[i];

    // this failed
    return NULL;
}

/*
 * PROTOCOL::action (int playerid, int cardid, int extra)
 *
 * This will send an ACTION command with the supplied parameters to the
 * server and handle the reply.
 *
 */
void
PROTOCOL::action (int playerid, int cardid, int extra) {
    char packet[NET_MAX_PACKET_LEN];

    // we don't want incoming() to disturb us
    nc->disable_incoming();

    // build the packet
    snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_CMD_ACTION, cardid,
	      playerid, extra);

    // send it
    nc->send_packet (packet);

    // wait for a reply
    nc->wait_for_data (packet);

    // incoming() is welcome to bug us
    nc->enable_incoming();

    // success?
    if (packet[0] != '1') {
	// no. complain
	WPRINTF ("Unable to play card: [%s]", packet);
    }

    // incoming() is welcome to bug us
    nc->enable_incoming();
}

/*
 * PROTOCOL::defend (int cardid)
 *
 * This will cause you to defend yourself using card [cardid].
 *
 */
void
PROTOCOL::defend (int cardid) {
    char packet[NET_MAX_PACKET_LEN];

    // we don't want incoming() to disturb us
    nc->disable_incoming();

    // build the packet
    snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_CMD_DEFEND, cardid);

    // send it
    nc->send_packet (packet);

    // wait for the reply
    nc->wait_for_data (packet);

    // incoming() is welcome to bug us again
    nc->enable_incoming();

    // success?
    if (packet[0] != '1') {
	// no. complain
	WPRINTF ("Unable to play defensive card: [%s]", packet);
	return;
    }
}

/*
 * PROTOCOL::skip_turn()
 *
 * This will skip our turn.
 *
 */
void
PROTOCOL::skip_turn() {
    char packet[NET_MAX_PACKET_LEN];

    // we don't want incoming() to disturb us
    nc->disable_incoming();

    // send it
    nc->send_packet (PROTOCOL_CMD_SKIP);

    // wait for the reply
    nc->wait_for_data (packet);

    // incoming() is welcome to bug us again
    nc->enable_incoming();

    // success?
    if (packet[0] != '1') {
	// no. complain
	WPRINTF ("Unable to skip turn: [%s]", packet);
	return;
    }

    WPRINTF ("Turn skipped");
}

/*
 * PROTOCOL::handle_game_action (char* packet)
 *
 * This will handle game action [packet].
 *
 */
void
PROTOCOL::handle_game_action (char* packet) {
    int playerid, victimid, cardid, extra;
    char* ptr;
    char* ptr2;
    PLAYER* attacker;
    PLAYER* victim;

    // scan for a ','
    ptr = strchr (packet, ',');
    if (ptr == NULL)
	// this packet is bogus. discard it
	return;

    // isolate the player id
    *ptr = 0; ptr++;
    playerid = strtol (packet + 4, &ptr2, 10);
    if (*ptr2)
	// there was bogus after the number. discard the packet
	return;

    // isolate the victim id
    ptr2 = strchr (ptr, ',');
    if (ptr2 == NULL)
	// this packet is bogus. discard it
	return;
    *ptr2 = 0; ptr2++;

    // fetch the victim id
    victimid = strtol (ptr, &ptr, 10);
    if (*ptr)
	// there was bogus after the number. discard the packet
	return;

    // isolate the card id
    ptr = strchr (ptr2, ',');
    if (ptr == NULL)
	// this packet is bogus. discard it
	return;
    *ptr = 0; ptr++;

    // fetch the card id
    cardid = strtol (ptr2, &ptr2, 10);
    if (*ptr2)
	// there was bogus after the number. discard the packet
	return;

    // fetch the extras
    extra = strtol (ptr, &ptr, 10);
    if (*ptr)
	// there was bogus after the number. discard the packet
	return;

    // fetch the attacker's name
    attacker = find_player (playerid);
    if (attacker == NULL) {
	// ?
	WPRINTF ("[WARNING] Player %u attacks, but not in *this* game!", playerid);
	return;
    }

    // fetch the victim's name
    victim = find_player (victimid);
    if (victim == NULL) {
	// ?
	WPRINTF ("[WARNING] Player %u is hurt, but not in *this* game!", victimid);
	return;
    }

    // are *we* being hurt?
    if (!strcmp (victim->get_name(), playername)) {
	// yes. show that
	WPRINTF ("%s damages you with a %s", attacker->get_name(), lmoneyc->deck->get_card (cardid)->get_name());
    } else if (!strcmp (attacker->get_name(), playername)) {
	// we are hurting someone else
	WPRINTF ("You damage %s with a %s", victim->get_name(), lmoneyc->deck->get_card (cardid)->get_name());
    } else {
	// we're left behind...
	WPRINTF ("%s damages %s with a %s", attacker->get_name(), victim->get_name(), lmoneyc->deck->get_card (cardid)->get_name());
    }
}

/*
 * PROTOCOL::new_game (int maxplayers, char* name)
 *
 * This will try to create a new game with a maximum of [maxplayer] players
 * and a title of [name]. It will return zero on success or -1 on failure.
 *
 */
int
PROTOCOL::new_game (int maxplayers, char* name) {
    char packet[NET_MAX_PACKET_LEN];

    // we don't want incoming() to disturb us
    nc->disable_incoming();

    // construct the command and send it
    snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_CMD_NEWGAME, name, maxplayers);
    nc->send_packet (packet);

    // send the command and wait for the response
    nc->wait_for_data (packet);

    // incoming() is welcome to bug us again
    nc->enable_incoming();

    // did it work?
    if (packet[0] != '1') {
	// no. complain
	WPRINTF ("Couldn't create a new game - [%s]", packet + 4);
	return -1;
    }

    // query the players
    query_players();

    // it worked
    return 0;
}

/*
 * PROTOCOL::drop (int mask)
 *
 * This will drop cards according to [mask].
 *
 */
void
PROTOCOL::drop (int mask) {
    char  packet[NET_MAX_PACKET_LEN];

    // we don't want incoming() to disturb us
    nc->disable_incoming();

    // construct the command and send it
    snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_CMD_DROP, mask);
    nc->send_packet (packet);

    // send the command and wait for the response
    nc->wait_for_data (packet);

    // incoming() is welcome to bug us again
    nc->enable_incoming();

    // did it work?
    if (packet[0] != '1') {
	// no. complain
	WPRINTF ("Couldn't drop cards - [%s]", packet + 4);
    }
}

/*
 * PROTOCOL::handle_join (char* packet)
 *
 * This will handle join packet [packet].
 *
 */
void
PROTOCOL::handle_join (char* packet) {
    char* ptr;
    int   playerid;
    PLAYER* p;

    // scan for a colon
    ptr = strchr (packet, ':');
    if (ptr == NULL)
	// there is none. corrupt packet, discard
	return;
    *ptr = 0;

    // fetch the player id
    playerid = strtol (packet + 4, &ptr, 10);
    if (*ptr)
	// humm, there is junk after the number. discard
	return;

    // re-query the players
    query_players();

    // inform the client
    p = find_player (playerid);
    if (p != NULL)
	WPRINTF ("Player [%s] joined the game", p->get_name());
}

/*
 * PROTOCOL::handle_leave (char* packet)
 *
 * This will handle leave packet [packet].
 *
 */
void
PROTOCOL::handle_leave (char* packet) {
    char* ptr;
    int   playerid;
    PLAYER* p;

    // scan for a colon
    ptr = strchr (packet, ':');
    if (ptr == NULL)
	// there is none. corrupt packet, discard
	return;
    *ptr = 0;

    // fetch the player id
    playerid = strtol (packet + 4, &ptr, 10);

    // inform the client
    p = find_player (playerid);
    if (p != NULL)
	WPRINTF ("Player [%s] left the game", p->get_name());

    // re-query the players
    query_players();
}

/*
 * PROTOCOL::handle_defend (char* packet)
 *
 * This will handle incoming defend packet [packet].
 *
 */
void
PROTOCOL::handle_defend (char* packet) {
    WPRINTF ("You can defend yourself against this attack, try DOdge, BLock, FReedom, HUmilation or NOthing.");
}

/*
 * PROTOCOL::handle_defended (char* packet)
 *
 * This will handle incoming defended packet [packet].
 *
 */
void
PROTOCOL::handle_defended (char* packet) {
    char* ptr;
    char* ptr2;
    PLAYER* p;
    int   playerid, cardid;

    // isolate the player name
    ptr = strchr (packet, ',');
    if (ptr == NULL)
	// this failed. corrupt packet... discard
	return;
    *ptr = 0; ptr++;

    // fetch the player's id
    playerid = strtol (packet + 4, &ptr2, 10);
    if (*ptr2)
	// there was bogus after the player id. discard
	return;

    // grab the card id
    cardid = strtol (ptr, &ptr2, 10);
    if (*ptr2)
	// there was bogus after the card id. discard
	return;

    // grab the player info
    p = find_player (playerid);
    if (p == NULL) {
	// ?
	WPRINTF ("[WARNING] Player %u defends, but not in *this* game!", playerid);
	return;
    }

    // was it a serious defense?
    if (cardid == CARD_TYPE_NOTHING) {
	// no. laugh at the user
	if (!strcmp (p->get_name(), playername))
	    WPRINTF ("You do nothing and happily let your opponent hurt you");
	else
	    WPRINTF ("%s does nothing and enjoys the pain", p->get_name());

	return;
    }

    // it was serious. show that
    if (!strcmp (p->get_name(), playername))
	WPRINTF ("You manage to deflect the attack with a mighty %s card!", lmoneyc->deck->get_card (cardid)->get_name());
    else
	WPRINTF ("%s manages to deflect the attack with a mighty %s card!", p->get_name(), lmoneyc->deck->get_card (cardid)->get_name());
}

/*
 * PROTOCOL::handle_turn (char* packet)
 *
 * This will handle incoming turn packet [packet].
 *
 */
void
PROTOCOL::handle_turn (char* packet) {
    WPRINTF ("It's your turn");
}

/*
 * PROTOCOL::handle_fatal (char* packet)
 *
 * This will handle incoming fatal packet [packet].
 *
 */
void
PROTOCOL::handle_fatal(char* packet) {
    WPRINTF ("FATAL server response: [%s], closing connection", packet + 4);
    nc->shutdown();
}

/*
 * PROTOCOL::handle_drop (char* packet)
 *
 * This will handle an incoming drop packet [packet].
 */
void
PROTOCOL::handle_drop (char* packet) {
    char* ptr;
    char* ptr2;
    int   playerid, cardid;
    PLAYER* p;

    // isolate the player id
    ptr = strchr (packet, ',');
    if (ptr == NULL)
	// the packet is bogus. discard it
	return;
    *ptr = 0; ptr++;

    // grab the player id
    playerid = strtol (packet + 4, &ptr2, 10);
    if (*ptr2)
	// there is bogus after the id. discard the packet
	return;

    // grab the card id
    cardid = strtol (ptr, &ptr, 10);
    if (*ptr)
	// there is bogus after the id. discard the packet
	return;

    // grab the player info
    p = find_player (playerid);
    if (p == NULL) {
	// ?
	WPRINTF ("[WARNING] Player %u drops a card, but not in *this* game!", playerid);
	return;
    }

    // show the msssage
    if (!strcmp (p->get_name(), playername))
	WPRINTF ("You drop a %s card", lmoneyc->deck->get_card (cardid)->get_name());
    else
	WPRINTF ("%s drops a %s card", p->get_name(), lmoneyc->deck->get_card (cardid)->get_name());
}

/*
 * PROTOCOL::handle_idle (char* packet)
 *
 * This will handle an incoming idle packet [packet].
 */
void
PROTOCOL::handle_idle (char* packet) {
    char* ptr;
    int   playerid;
    PLAYER* p;

    // grab the player id
    playerid = strtol (packet + 4, &ptr, 10);
    if (*ptr)
	// there is bogus after the id. discard the packet
	return;

    // grab the player info
    p = find_player (playerid);
    if (p == NULL) {
	// ?
	WPRINTF ("[WARNING] Player %u idles, but not in *this* game!", playerid);
	return;
    }

    // show the msssage
    if (!strcmp (p->get_name(), playername))
	WPRINTF ("You don't care");
    else
	WPRINTF ("%s doesn't seem to care", p->get_name());
}

/*
 * PROTOCOL::firstaid (int no)
 *
 * This will try to use [no] First Aids.
 *
 */
void
PROTOCOL::firstaid (int no) {
    char  packet[NET_MAX_PACKET_LEN];

    // we don't want incoming() to disturb us
    nc->disable_incoming();

    // construct the command and send it
    snprintf (packet, NET_MAX_PACKET_LEN, PROTOCOL_CMD_AID, no);
    nc->send_packet (packet);

    // send the command and wait for the response
    nc->wait_for_data (packet);

    // incoming() is welcome to bug us again
    nc->enable_incoming();

    // did it work?
    if (packet[0] != '1') {
	// no. complain
	WPRINTF ("Unable to use First Aids - [%s]", packet + 4);
	return;
    }
}

/*
 * PROTOCOL::handle_aid (char* packet)
 *
 * This will handle an incoming first aid packet [packet].
 */
void
PROTOCOL::handle_aid (char* packet) {
    char* ptr;
    char* ptr2;
    int   playerid, points;
    PLAYER* p;

    // isolate the player id
    ptr = strchr (packet, ',');
    if (ptr == NULL)
	// the packet is bogus. discard it
	return;
    *ptr = 0; ptr++;

    // grab the player id
    playerid = strtol (packet + 4, &ptr2, 10);
    if (*ptr2)
	// there is bogus after the id. discard the packet
	return;

    // grab the number of points
    points = strtol (ptr, &ptr, 10);
    if (*ptr)
	// there is bogus after the id. discard the packet
	return;

    // grab the player info
    p = find_player (playerid);
    if (p == NULL) {
	// ?
	WPRINTF ("[WARNING] Player %u heals himself but not in *this* game!", playerid);
	return;
    }

    // show the msssage
    if (!strcmp (p->get_name(), playername))
	WPRINTF ("You try to heal %u points...", points);
    else
	WPRINTF ("%s tries to heal %u points... you may HUMilatie this or do NOthing", p->get_name(), points);
}

/*
 * PROTOCOL::handle_aided (char* packet)
 *
 * This will handle an incoming first aid success packet [packet].
 */
void
PROTOCOL::handle_aided (char* packet) {
    char* ptr;
    char* ptr2;
    int   playerid, points;
    PLAYER* p;

    // isolate the player id
    ptr = strchr (packet, ',');
    if (ptr == NULL)
	// the packet is bogus. discard it
	return;
    *ptr = 0; ptr++;

    // grab the player id
    playerid = strtol (packet + 4, &ptr2, 10);
    if (*ptr2)
	// there is bogus after the id. discard the packet
	return;

    // grab the number of points
    points = strtol (ptr, &ptr, 10);
    if (*ptr)
	// there is bogus after the id. discard the packet
	return;

    // grab the player info
    p = find_player (playerid);
    if (p == NULL) {
	// ?
	WPRINTF ("[WARNING] Player %u heals himself but not in *this* game!", playerid);
	return;
    }

    // show the msssage
    if (!strcmp (p->get_name(), playername))
	WPRINTF ("You successfully healed %u points", points);
    else
	WPRINTF ("%s successfully healed %u points", p->get_name(), points);
}

/*
 * PROTOCOL::idle()
 *
 * This will tell the server we're not doing anything.
 *
 */
void
PROTOCOL::idle() {
    char packet[NET_MAX_PACKET_LEN];

    // we don't want incoming() to disturb us
    nc->disable_incoming();

    // send the command
    nc->send_packet (PROTOCOL_CMD_NOTHING);

    // send the command and wait for the response
    nc->wait_for_data (packet);

    // incoming() is welcome to bug us again
    nc->enable_incoming();

    // did it work?
    if (packet[0] != '1')
	// no. complain
	WPRINTF ("Couldn't idle... waaah - [%s]", packet + 4);
}

/*
 * PROTOCOL::handle_incoming(char* packet)
 *
 * This will handle an incoming packet [packet].
 *
 */
void
PROTOCOL::handle_incoming (char* packet) {
    int i = 0;
    char* mainptr;
    char* nextptr;
    char* ptr;

    // handle the packet
    mainptr = packet;
    while (*mainptr) {
	// fetch the code
	i = strtol (mainptr, &ptr, 10);
	if (*ptr != ' ')
	    // this packet is corrupt. discard it
	    return;

	// scan for the end
	nextptr = mainptr;
	while ((*nextptr != '\n') && (*nextptr)) nextptr++;
	if (*nextptr) { *nextptr = 0; nextptr++; }

	// figure out what the code is
	switch (i) {
	    case PROTOCOL_CODE_SUCCESS: // generic ok message
					success = 1;
					break;
	case PROTOCOL_CODE_GAMESTARTED: // the game began!
					started = 1;
					WPRINTF ("The game has STARTED!");
					break;
         case PROTOCOL_CODE_GAMEACTION: // handle the action
				 	handle_game_action (mainptr);
					break;
         case PROTOCOL_CODE_PLAYERJOIN: // player join
	 			        handle_join (mainptr);
					break;
        case PROTOCOL_CODE_PLAYERLEAVE: // player join
	 			        handle_leave (mainptr);
					break;
           case PROTOCOL_CODE_YOURTURN: // your turn
					handle_turn (mainptr);
					break;
             case PROTOCOL_CODE_DEFEND:	// defend thyself
					handle_defend (mainptr);
					break;
           case PROTOCOL_CODE_DEFENDED:	// defended
					handle_defended (mainptr);
					break;
	        case PROTOCOL_CODE_AID:	// aid
					handle_aid (mainptr);
					break;
	       case PROTOCOL_CODE_DROP: // drop
					handle_drop (mainptr);
					break;
	       case PROTOCOL_CODE_IDLE: // idler
					handle_idle (mainptr);
					break;
	      case PROTOCOL_CODE_AIDED:	// aided
					handle_aided (mainptr);
					break;
              case PROTOCOL_CODE_FATAL: // fatal error
					handle_fatal (mainptr);
					break;
	        case PROTOCOL_CODE_FAIL: // generic failure message
					fail = 1;
					break;
			       default: // ?
					WPRINTF ("PROTOCOL::incoming(): unknown code [%i] in packet [%s]", i, packet);
	}

	// next
	mainptr = nextptr;
    }
}

/*
 * PROTOCOL::incoming()
 *
 * This will be called whenever there is incoming data and we are not explicitly
 * waiting for it. It will return 0 if it did something and -1 if it didn't
 * do anything.
 *
 */
int
PROTOCOL::incoming() {
    char packet[NET_MAX_PACKET_LEN];

    // fetch the data
    if (nc->get_packet (packet) < 0)	
	return -1;

    // pass it over
    handle_incoming (packet);

    // we did something
    return 0;
}
