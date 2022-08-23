/*
 * LunchMoney 2.0 Daemon - crowd.h (LunchMoney Players Crowd)
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
#ifndef __CROWD_H__
#define __CROWD_H__

#include "player.h"

/* LMCLIENT is yet to come */
class LMCLIENT;

/*
 * CROWD is the collection of all LunchMoney players.
 */
class CROWD {
public:
    CROWD();

    PLAYER*		create_player (LMCLIENT*, char*);
    void		remove_player (PLAYER*);

    PLAYER*		find_player_by_name (char*);
    PLAYER*		find_player_by_id (int);

private:
    PLAYER*		players;
    int			playerid;

    void		add_player (PLAYER*);
};
#endif
