/*
 * LunchMoney Client - (c) 2002 Rink Springer, GPL
 *
 */
#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <ncurses.h>
#include <stdio.h>
#include "network.h"

/* CMD is a command entry */
typedef void (CMD) (char*);

/* CMDENTRY is a command table entry */
typedef struct {
    char*	cmd;
    CMD*	func;
} CMDENTRY;

/*
 * COMMAND will take care of the input command parsing.
 */
class COMMAND {
public:
    static void	handle(char*);

    static CMD cmd_quit;
    static CMD cmd_connect;
    static CMD cmd_disconnect;
    static CMD cmd_player;
    static CMD cmd_list;
    static CMD cmd_join;
    static CMD cmd_new;
    static CMD cmd_players;
    static CMD cmd_cards;
    static CMD cmd_skip;
    static CMD cmd_drop;

    static CMD cmd_kick2;
    static CMD cmd_kick4;
    static CMD cmd_uppercut;
    static CMD cmd_uppercut2;
    static CMD cmd_firstaid;
    static CMD cmd_bigcombo;
    static CMD cmd_headbutt;

    static CMD cmd_pipe;
    static CMD cmd_knife;
    static CMD cmd_hammer;

    static CMD cmd_grab;
    static CMD cmd_choke;
    static CMD cmd_headlock;

    static CMD cmd_jab;
    static CMD cmd_hook;
    static CMD cmd_stomp;
    static CMD cmd_elbow;

    static CMD cmd_roundhouse; 
    static CMD cmd_pimpslap;

    static CMD cmd_hailmary;
    static CMD cmd_humilation;

    static CMD cmd_swap;
    static CMD cmd_dodge;
    static CMD cmd_block;
    static CMD cmd_freedom;
    static CMD cmd_backfist;
    static CMD cmd_nothing;

    static void action (char*, int, int);
    static void defend (int);
};

#endif
