/*
 * LunchMoney 2.0 Daemon - lmclient.h (LunchMoney Game Client)
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
#ifndef __LMCLIENT_H__
#define __LMCLIENT_H__

#include "game.h"
#include "player.h"
#include "network.h"

/* PROTOCOL is yet to come */
class PROTOCOL;

/*
 * LMCLIENT is the actual LunchMoney daemon client.
 */
class LMCLIENT : public NETCLIENT {
    friend class PROTOCOL;
    friend class PLAYER;

public:
			LMCLIENT();
			~LMCLIENT();

    void		handle_packet (char*, int);
    void		send_packet (char*);

protected:
    void		on_connect();
    void		on_disconnect();

    PLAYER*		player;
    GAME*		game;
};
#endif
