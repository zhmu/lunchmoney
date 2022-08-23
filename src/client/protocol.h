/*
 * LunchMoney Client - (c) 2002 Rink Springer, GPL
 *
 */
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include "card.h"
#include "player.h"
#include "network.h"

/* PROTOCOL_CMD_xxx are the commands */
#define PROTOCOL_CMD_QUIT		"QUIT"		/* bye */
#define PROTOCOL_CMD_PLAYER		"PLAYER %s"	/* set player name */
#define PROTOCOL_CMD_LISTGAMES		"LISTGAMES"	/* game list */
#define PROTOCOL_CMD_JOINGAME		"JOINGAME %u"	/* join game */
#define PROTOCOL_CMD_PLAYERINFO		"PLAYERINFO %u"	/* player info */
#define PROTOCOL_CMD_GETCARDS		"GETCARDS"	/* get cards */
#define PROTOCOL_CMD_ACTION		"ACTION %u,%u,%u" /* action! */
#define PROTOCOL_CMD_SKIP		"SKIP"		/* skip turn */
#define PROTOCOL_CMD_NEWGAME		"NEWGAME %s,%u"	/* new game */
#define PROTOCOL_CMD_DEFEND		"DEFEND %u"	/* defend */
#define PROTOCOL_CMD_DROP		"DROP %u"	/* drop cards */
#define PROTOCOL_CMD_AID		"AID %u"	/* aid */
#define PROTOCOL_CMD_NOTHING		"IDLE"		/* do nothing */

/* PROTOCOL_CODE_xxx are all incoming codes we care about */
#define PROTOCOL_CODE_SUCCESS		100		/* success */
#define PROTOCOL_CODE_GAMESTARTED	200		/* game started */
#define PROTOCOL_CODE_GAMEACTION	201		/* game action */
#define PROTOCOL_CODE_PLAYERJOIN	202		/* player join */
#define PROTOCOL_CODE_PLAYERLEAVE	203		/* player leave */
#define PROTOCOL_CODE_YOURTURN		204		/* your turn */
#define PROTOCOL_CODE_DEFEND		205		/* defend yourself */
#define PROTOCOL_CODE_DEFENDED		206		/* defended */
#define PROTOCOL_CODE_DROP		207		/* card drop */
#define PROTOCOL_CODE_AID		208		/* aid */
#define PROTOCOL_CODE_IDLE		209		/* idler */
#define PROTOCOL_CODE_AIDED		210		/* aided */
#define PROTOCOL_CODE_FAIL		300		/* failure */
#define PROTOCOL_CODE_FATAL		400		/* fatal! */

/* PROTOCOL_MAX_xxx are all limitations */
#define PROTOCOL_MAX_GAME_LEN		64
#define PROTOCOL_MAX_PLAYERS		4
#define PROTOCOL_MAX_CARDS		5

/*
 * GAME is a LunchMoney game
 *
 */
typedef struct {
    int  id;
    char name[PROTOCOL_MAX_GAME_LEN];
    int  numplayers;
    int  maxplayers;
} GAME;

/*
 * PROTOCOL is responsible for the LunchMoney client-server protocol.
 *
 */
class PROTOCOL {
public:
    PROTOCOL();
    ~PROTOCOL();

    int		connect (char*,int);
    int		is_connected();
    void	disconnect();

    int		set_player(char*);

    void	query_games();
    void	query_players();
    void	query_cards();
    int		join_game(int);
    int		new_game (int,char*);
    int		fetch_game(GAME*);
    CARD*	scan_card(int);
    PLAYER*	scan_player(char*);
    PLAYER*	find_player(int);
    void	action (int,int,int);
    void	defend (int);
    void	skip_turn();
    void	drop(int);
    void	firstaid(int);
    void	idle();

    void	handle_game_action(char*);
    void	handle_join(char*);
    void	handle_leave(char*);
    void	handle_defend(char*);
    void	handle_defended(char*);
    void	handle_turn(char*);
    void	handle_drop(char*);
    void	handle_fatal(char*);
    void	handle_aid(char*);
    void	handle_idle(char*);
    void	handle_aided(char*);

    void	handle_incoming(char*);
    int		incoming ();

    int		has_started();

    NETCLIENT* nc;

    PLAYER*	player[PROTOCOL_MAX_PLAYERS + 1];
    CARD*	card[PROTOCOL_MAX_CARDS];

    char*	failptr;

private:
    int		started, success, fail;

    char*	playername;
    char	last_incoming[NET_MAX_PACKET_LEN];
};

#endif
