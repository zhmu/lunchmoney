/*
 * LunchMoney Client - (c) 2002 Rink Springer, GPL
 *
 */
#include <curses.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "card.h"
#include "config.h"
#include "command.h"
#include "lmoneyc.h"

/* commands[] is the table of all commands we support */
CMDENTRY commands[] = { { "QUIT", 	&COMMAND::cmd_quit },
			{ "CONNECT",	&COMMAND::cmd_connect },
			{ "CON",	&COMMAND::cmd_connect },
			{ "DISCONNECT",	&COMMAND::cmd_disconnect },
			{ "DISC",	&COMMAND::cmd_disconnect },
			{ "PLAYER",	&COMMAND::cmd_player },
			{ "LIST",	&COMMAND::cmd_list },
			{ "JOIN",	&COMMAND::cmd_join },
			{ "NEW",	&COMMAND::cmd_new },
			{ "PLAYERS",	&COMMAND::cmd_players },
			{ "CARDS",	&COMMAND::cmd_cards },
			{ "C",		&COMMAND::cmd_cards },
			{ "DROP",	&COMMAND::cmd_drop },
			{ "D",		&COMMAND::cmd_drop },
			{ "NOTHING",	&COMMAND::cmd_nothing },
			{ "NO",		&COMMAND::cmd_nothing },
			{ "SKIP",	&COMMAND::cmd_skip },
			{ "KICK2",	&COMMAND::cmd_kick2 },
			{ "KICK4",	&COMMAND::cmd_kick4 },
			{ "K2",		&COMMAND::cmd_kick2 },
			{ "K4",		&COMMAND::cmd_kick4 },
			{ "UPPERCUT",	&COMMAND::cmd_uppercut },
			{ "UP",		&COMMAND::cmd_uppercut },
			{ "UPPERCUT2",	&COMMAND::cmd_uppercut2 },
			{ "UP2",	&COMMAND::cmd_uppercut },
			{ "HOOK",	&COMMAND::cmd_hook },
			{ "PIPE",	&COMMAND::cmd_pipe },
			{ "KNIFE",	&COMMAND::cmd_knife },
			{ "HAMMER",	&COMMAND::cmd_hammer },
			{ "FIRSTAID",	&COMMAND::cmd_firstaid },
			{ "FA",		&COMMAND::cmd_firstaid },
			{ "AID",	&COMMAND::cmd_firstaid },
			{ "BIGCOMBO",	&COMMAND::cmd_bigcombo },
			{ "BC",		&COMMAND::cmd_bigcombo },
			{ "DODGE",	&COMMAND::cmd_dodge },
			{ "DO",		&COMMAND::cmd_dodge },
			{ "BLOCK",	&COMMAND::cmd_block },
			{ "BL",		&COMMAND::cmd_block },
			{ "FREEDOM",	&COMMAND::cmd_freedom },
			{ "FR",		&COMMAND::cmd_freedom },
			/*{ "GRAB",	&COMMAND::cmd_grab },
			{ "GR",		&COMMAND::cmd_grab },
			{ "CHOKE",	&COMMAND::cmd_choke },
			{ "CH",		&COMMAND::cmd_choke },
			{ "HEADLOCK",	&COMMAND::cmd_headlock },
			{ "HL",		&COMMAND::cmd_headlock },
			{ "HL",		&COMMAND::cmd_headlock },*/
			{ "JAB",	&COMMAND::cmd_jab },
			{ "HOOK",	&COMMAND::cmd_hook },
			{ "HO",		&COMMAND::cmd_hook },
			{ "STOMP",	&COMMAND::cmd_stomp },
			{ "ST",		&COMMAND::cmd_stomp },
			{ "ELBOW",	&COMMAND::cmd_elbow },
			{ "EL",		&COMMAND::cmd_elbow },
			{ "ROUNDHOUSE",	&COMMAND::cmd_roundhouse },
			{ "RH",		&COMMAND::cmd_roundhouse },
			{ "BACKFIST",	&COMMAND::cmd_backfist },
			{ "SP",		&COMMAND::cmd_backfist },
			{ "HAILMARY",	&COMMAND::cmd_hailmary },
			{ "MARY",	&COMMAND::cmd_hailmary },
			{ "HM",		&COMMAND::cmd_hailmary },
			/*{ "HUMILATION",	&COMMAND::cmd_humilation },
			{ "HU",		&COMMAND::cmd_humilation },*/
			{ "HEADBUTT",	&COMMAND::cmd_headbutt },
			{ "HB",		&COMMAND::cmd_headbutt },
			{ "PIMPSLAP",	&COMMAND::cmd_pimpslap },
			{ "PIMP",	&COMMAND::cmd_pimpslap },
			{ "PS",		&COMMAND::cmd_pimpslap },
			{ NULL,		NULL } };

/*
 * COMMAND::handle (char* cmd)
 *
 * This will handle command [cmd].
 *
 */
void
COMMAND::handle (char* cmd) {
    char* ptr;
    CMDENTRY* ce = commands;

    // scan for a space 
    ptr = strchr (cmd, ' ');
    if (ptr != NULL) {
	// isolate the command
	*ptr = 0; ptr++;
	while ((*ptr == ' ') || (*ptr == '\t'))
	    ptr++;
    } else {
	// let [ptr] point to the leading space
	ptr = strchr (cmd, 0);
    }

    // [cmd] is now the command, and [ptr] are the arguments.
    // scan for the command
    while (ce->cmd) {
	// match?
	if (!strcasecmp (ce->cmd, cmd)) {
	    // yes. call the function
	    ce->func (ptr);

	    // bye
	    return;
	}

	// next
	ce++;
    }

    // what's this?
    WPRINTF ("Unknown command");
}

/*
 * COMMAND::cmd_quit(char* arg)
 *
 * This will quit the LunchMoney game.
 * 
 */
void
COMMAND::cmd_quit (char* arg) {
    // bye bye
    lmoneyc->stop();
    WPRINTF ("Bye bye");
}

/*
 * COMMAND::cmd_connect(char* arg)
 *
 * This will connect to a LunchMoney server.
 * 
 */
void
COMMAND::cmd_connect (char* arg) {
    char* ptr;
    int   portno = 0;

    // is there a colon within the argument?
    ptr = strchr (arg, ':');
    if (ptr != NULL) {
	// yes. isolate it
	*ptr = 0; ptr++;

	// fetch the port number
	portno = strtol (ptr, &ptr, 0);
	if (*ptr) {
	    // this failed. complain
	    WPRINTF ("Invalid port number specified");
	    return;
	}
    }

    // got a port number?
    if (!portno)
	// no. use the default
	portno = CONF_DEFAULT_PORT;

    // inform the client
    WPRINTF ("Connecting to [%s:%u]...", arg, portno);

    // connect!
    if (lmoneyc->p->connect (arg, portno) < 0) {
	// this failed. complain
	WPRINTF ("Connection failed");
	return;
    }

    // we are in
    WPRINTF ("Connected to [%s:%u]", arg, portno);

    // XXX
    cmd_player ("Rink");
}

/*
 * COMMAND::cmd_disconnect(char* arg)
 *
 * This will tell the server we're going.
 *
 */
void
COMMAND::cmd_disconnect(char* arg) {
    // are we connected?
    if (lmoneyc->p->is_connected() < 0) {
	// no. complain
	WPRINTF ("Can't disconnect, we are not connected");
	return;
    }

    // bye bye
    lmoneyc->p->disconnect();
    WPRINTF ("Disconnected");
}

/*
 * COMMAND::cmd_player(char* arg)
 *
 * This will tell the server our intended player name.
 *
 */
void
COMMAND::cmd_player(char* arg) {
    // got an argument? 
    if (!*arg) {
	// no. complain
	WPRINTF ("You must supply a player name");
	return;
    }

    // set the name
    if (lmoneyc->p->set_player (arg) < 0)
	// this failed. complain
	return;

    // it worked. inform the client
    WPRINTF ("Welcome to the game, %s!", arg);
}

/*
 * COMMAND::cmd_list(char* arg)
 *
 * This will query the server for its games.
 *
 */
void
COMMAND::cmd_list(char* arg) {
    GAME g;

    // are we connected?
    if (lmoneyc->p->is_connected() < 0) {
	// no. complain
	WPRINTF ("You are not connected to a server");
	return;
    }

    // query the server for the games
    lmoneyc->p->query_games();

    // list them all
    WPRINTF ("Games on this server");
    while (lmoneyc->p->fetch_game (&g)) {
	WPRINTF ("%u. %s [%u/%u]", g.id, g.name, g.numplayers, g.maxplayers);
    }
}

/*
 * COMMAND::cmd_join(char* arg)
 *
 * This will join a game on the server.
 *
 */
void
COMMAND::cmd_join(char* arg) {
    int id;
    char* ptr;

    // got any arguments?
    if (!*arg) {
	// no. complain
	WPRINTF ("You must supply a game ID");
	return;
    }

    // fetch the id
    id = strtol (arg, &ptr, 10);
    if (*ptr) {
	// there is bogus after the game id. complain
	WPRINTF ("You must supply a numeric game ID");
	return;
    }

    // are we connected?
    if (lmoneyc->p->is_connected() < 0) {
	// no. complain
	WPRINTF ("You are not connected to a server");
	return;
    }

    // go
    if (lmoneyc->p->join_game (id) < 0)
	// this failed. return
	return;

    // it worked!
    WPRINTF ("Game successfully joined");

    // show info about the players
    cmd_players (NULL);
}

/*
 * COMMAND::cmd_new (char* arg)
 *
 * This will create a new game.
 *
 */
void
COMMAND::cmd_new (char* arg) {
    char* ptr;
    int   numplayers;

    // scan for the ','
    ptr = strchr (arg, ',');
    if (ptr == NULL) {
	// there is none. wrong command syntax
	WPRINTF ("Incorrect usuage");
	return;
    }

    // isolate the name
    *ptr = 0; ptr++;

    // fetch the number of players
    numplayers = strtol (ptr, &ptr, 10);
    if (*ptr) {
	// there is garbage after the number. complain
	WPRINTF ("Incorrect usuage");
	return;
    }

    // create a new game
    if (lmoneyc->p->new_game (numplayers, arg) < 0)
	// no luck. too bad
	return;

    // we're inside a new game now
    WPRINTF ("New game successfully started");

    // show info about the players
    cmd_players (NULL);
}

/*
 * COMMAND::cmd_players(char* arg)
 *
 * This will list all players in this game.
 *
 */
void
COMMAND::cmd_players (char* arg) {
    int i;
    PLAYER *p;

    // inform the user about the upcoming data
    WPRINTF ("Player list");

    // scan them all
    for (i = 1; i <= PROTOCOL_MAX_PLAYERS; i++) {
	// get the player
	p = lmoneyc->p->player[i];

	// does this player exist?
	if (p->get_id()) {
	    // yes. display the player
	    WPRINTF ("%u. %s (%u life)", p->get_id(), p->get_name(), p->get_life());
	}
    }
}

/*
 * COMMAND::cmd_cards(char* arg)
 *
 * This will list the player's cards.
 *
 */
void
COMMAND::cmd_cards (char* arg) {
    int i;
    CARD* c;

    // inform the user about what we are going to do
    WPRINTF ("Playing cards");

    // ensure we have the newest cards
    lmoneyc->p->query_cards();

    // list them all
    for (i = 0; i < PROTOCOL_MAX_CARDS; i++) {
	// get the card
	c = lmoneyc->p->card[i];

	// valid card?
	if (c != NULL)
	    // yes. show the card
	    WPRINTF ("%u. %s", i + 1, c->get_name());
    }
}

/*
 * COMMAND::action (char* name, int cardno, int extra)
 *
 * This will 'use' card [cardno] with extra flags [extra] towards player [name].
 *
 */
void
COMMAND::action (char* name, int cardno, int extra) {
    PLAYER* player;

    // do we actually *have* that card?
    if (lmoneyc->p->scan_card (cardno) == NULL) {
	// no. complain
	WPRINTF ("You don't have a '%s' card, so you can't use it. Bummer", lmoneyc->deck->get_card (cardno)->get_name());
	return;
    }

    // fetch the player's id
    player = lmoneyc->p->scan_player (name);
    if (player == NULL) {
	// there is no such player. complain
	WPRINTF ("Player '%s' does not seem to exist. Try 'PLAYERS' to see who is in this game", name);
	return;
    }

    // do it
    lmoneyc->p->action (player->get_id(), cardno, extra);
}

/*
 * COMMAND::defend (int cardno)
 *
 * This will 'use' defensive card [cardno].
 *
 */
void
COMMAND::defend (int cardno) {
    // do we actually *have* that card?
    if (lmoneyc->p->scan_card (cardno) == NULL) {
	// no. complain
	WPRINTF ("You don't have a '%s' card, so you can't use it. Bummer", lmoneyc->deck->get_card (cardno)->get_name());
	return;
    }

    // do it
    lmoneyc->p->defend (cardno);
}

/*
 * COMMAND::cmd_drop(char* arg)
 *
 * This will drop some cards.
 *
 */
void
COMMAND::cmd_drop (char* arg) {
    char* ptr;
    int   i, j, mask = 0;

    if (!*arg) {
	WPRINTF ("Wrong usuage");
	return;
    }

    ptr = arg;
    while (*ptr) {
	i = strtol (ptr, &ptr, 10);
	if (*ptr) ptr++;
	if ((i > 0) && (i < 6)) {
	    j = 1; while (--i) j *= 2;
	    mask += j;
	} else {
	    WPRINTF ("You can only drop cards 1-5 (you don't have more anyway)");
	    return;
	}
    }

    // bye
    lmoneyc->p->drop (mask);

    // fetch the new cards
    cmd_cards(NULL);
}

/*
 * COMMAND::cmd_firstaid(char* arg)
 *
 * This will firstaid ourself.
 *
 */
void
COMMAND::cmd_firstaid (char* arg) {
    char* ptr;
    int   no;

    if (!*arg) {
	WPRINTF ("Wrong usuage");
	return;
    }

    // fetch the number
    no = strtol (ptr, &ptr, 10);
    if (*ptr) {
	// this failed. complain
	WPRINTF ("Wrong usuage");
	return;
    }

    // go for it
    lmoneyc->p->firstaid (no);
}

/*
 * COMMAND::cmd_skip (char* arg)
 *
 * This will skip a turn.
 *
 */
void COMMAND::cmd_skip (char* arg) { 
    // go
    lmoneyc->p->skip_turn();
}

/*
 * COMMAND::cmd_[xxx] (char* arg)
 *
 * This will handle a specific card.
 *
 */
void COMMAND::cmd_kick2 (char* arg) { action (arg, CARD_TYPE_KICK2, 0); }
void COMMAND::cmd_kick4 (char* arg) { action (arg, CARD_TYPE_KICK4, 0); }
void COMMAND::cmd_uppercut (char* arg) { action (arg, CARD_TYPE_UPPERCUT, 0); }
void COMMAND::cmd_uppercut2 (char* arg) { action (arg, CARD_TYPE_UPPERCUT2, 0); }
void COMMAND::cmd_hook (char* arg) { action (arg, CARD_TYPE_HOOK, 0); }
void COMMAND::cmd_elbow (char* arg) { action (arg, CARD_TYPE_ELBOW, 0); }
void COMMAND::cmd_pipe (char* arg) { action (arg, CARD_TYPE_PIPE, 0); }
void COMMAND::cmd_hammer (char* arg) { action (arg, CARD_TYPE_HAMMER, 0); }
void COMMAND::cmd_knife (char* arg) { action (arg, CARD_TYPE_KNIFE, 0); }
void COMMAND::cmd_jab (char* arg) { action (arg, CARD_TYPE_JAB, 0); }
void COMMAND::cmd_stomp (char* arg) { action (arg, CARD_TYPE_STOMP, 0); }
void COMMAND::cmd_bigcombo (char* arg) { action (arg, CARD_TYPE_BIGCOMBO, 0); }
void COMMAND::cmd_hailmary (char* arg) { action (arg, CARD_TYPE_HAILMARY, 0); }

void COMMAND::cmd_roundhouse (char* arg) { action (arg, CARD_TYPE_ROUNDHOUSE, 0); }
void COMMAND::cmd_backfist (char* arg) { action (arg, CARD_TYPE_SPINBACKFIST, 0); }
void COMMAND::cmd_headbutt (char* arg) { action (arg, CARD_TYPE_HEADBUTT, 0); }
void COMMAND::cmd_pimpslap (char* arg) { action (arg, CARD_TYPE_PIMPSLAP, 0); }

void COMMAND::cmd_dodge (char* arg) { defend (CARD_TYPE_DODGE); }
void COMMAND::cmd_block (char* arg) { defend (CARD_TYPE_BLOCK); }
void COMMAND::cmd_freedom (char* arg) { defend (CARD_TYPE_FREEDOM); }

/*
 * COMMAND::cmd_nothing (char* arg)
 *
 * This will cause the player to do 'Totally Nothing At All'.
 *
 */
void
COMMAND::cmd_nothing (char* arg) {
    lmoneyc->p->idle();
}
