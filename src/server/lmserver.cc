/*
 * LunchMoney 2.0 Daemon - lmserver.cc (LunchMoney Server)
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
#include "lmclient.h"
#include "lmserver.h"

/*
 * LMSERVER::handle_incoming()
 *
 * This needs to take care of the actual incoming connections.
 */
void
LMSERVER::handle_incoming() {
    LMCLIENT* client = new LMCLIENT();

    // try to accept the connection
    if (client->accept (get_fd()) < 0) {
	// this failed. remove the client and leave
	delete client;
	return;
    }

    // add the client to the list
    add_client (client);
}
