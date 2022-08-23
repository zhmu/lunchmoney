/*
 * LunchMoney 2.0 Daemon - playground.h (LunchMoney Playground)
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
#ifndef __PLAYGROUND_H__
#define __PLAYGROUND_H__

#include "game.h"

/*
 * PLAYGROUND is the collection of all LunchMoney games.
 */
class PLAYGROUND {
public:
    PLAYGROUND();

    GAME*		create_game (char*,int);
    void		remove_game (GAME*);

    GAME*		find_game_by_id (int);
    GAME*		get_games();

private:
    GAME*		games;
    int			gameid;

    void		add_game (GAME*);
};
#endif
