/*
 * LunchMoney 2.0 Daemon - game.h (LunchMoney Game)
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
#ifndef __GAME_H__
#define __GAME_H__

#include "deck.h"

/* GAME_MAX_PLAYERS is the maximum number of players in a single game */
#define GAME_MAX_PLAYERS	4

/* GAME_MAX_NAME_LEN is the maximum length of a game name */
#define GAME_MAX_NAME_LEN	64

/* PLAYER is yet to come */
class PLAYER;

/*
 * GAME is a LunchMoney game.
 *
 */
class GAME {
public:
		GAME(int,char*,int);
		~GAME();

    void	set_next (GAME*);

    int		get_id();
    char*	get_name();
    int		get_numplayers();
    int		get_player_count();
    GAME*	get_next();
    DECK*	get_deck();
    PLAYER*	get_defender();
    CARD*	get_card();

    int		has_started();

    void	add_player (PLAYER*);
    void	remove_player (PLAYER*);
    PLAYER*	get_player (int);
    PLAYER*	get_cur_player ();
    PLAYER*	get_player_by_id(int);

    void	start();
    int		join_game (PLAYER*);

    void	give_cards ();
    void	send_packet (char*);
    void	next_player ();

    void	action (CARD*, PLAYER*, PLAYER*, int);
    void	defend (CARD*);
    void	idle(PLAYER*); 

private:
    int		id;
    char	name[GAME_MAX_NAME_LEN];
    int		numplayers;
    PLAYER*	player[GAME_MAX_PLAYERS];
    DECK*	deck;

    int		started;
    int		curplayer;

    CARD*	curcard;
    int		curdefender;
    int		curstatus;
    int		curextra;

    GAME*	next;

    int		find_player (PLAYER*);
};
#endif
