/*
 * LunchMoney 2.0 Daemon - player.h (LunchMoney Player)
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
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "card.h"
#include "player.h"

/* LMCLIENT is yet to come */
class LMCLIENT;

/* PLAYER_MAX_NAME_LEN is the maximum length of a player's name */
#define PLAYER_MAX_NAME_LEN	32

/* PLAYER_MAX_CARDS is the number of cards a player has */
#define PLAYER_MAX_CARDS	5

/*
 * PLAYER is a LunchMoney player.
 */
class PLAYER {
public:
    PLAYER (LMCLIENT*, int, char*);

    int		get_id	 ();
    char*	get_name ();
    int		get_life ();
    PLAYER*	get_next ();
    LMCLIENT*	get_client();
    CARD*	get_card (int);

    int		scan_card (int);
    int		find_card (CARD*);

    void	set_next (PLAYER*);
    void	set_card (int, CARD*);

    int		can_defend();
    void	set_defend(int);

    void	drop_card (int);

private:
    int		id;
    char	name[PLAYER_MAX_NAME_LEN];
    int		life;
    int		defend;

    CARD*	card[PLAYER_MAX_CARDS];

    PLAYER*	next;
    LMCLIENT*	lm;
};
#endif
