/*
 * LunchMoney Client - (c) 2002 Rink Springer, GPL
 *
 */
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include "network.h"

/* PLAYER_MAX_xxx are all limitations */
#define PLAYER_MAX_NAME_LEN		64

/*
 * PLAYER is a LunchMoney player.
 *
 */
class PLAYER {
public:
    PLAYER();

    void	set_id(int);
    void	set_life(int);
    void	set_name(char*);

    int		get_id();
    int		get_life();
    char*	get_name();

private:
    int		id;
    char	name[PLAYER_MAX_NAME_LEN];
    int		life;
};
#endif
