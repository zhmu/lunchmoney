/*
 * LunchMoney 2.0 Daemon - network.h (Networking Code)
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
#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

/* NET_MAX_PACKET_LEN is the maximum size of an incoming data packet */
#define NET_MAX_PACKET_LEN	8192

/* NETSERVER is yet to come */
class NETSERVER;

/*
 * NETCLIENT is a client attached to a NETSERVER. It provides a convenient way
 * of talking to the attached client.
 */
class NETCLIENT {
    friend		class NETSERVER;

public:
    			NETCLIENT();
    virtual		~NETCLIENT();

    int			accept (int);
    void		shutdown ();
    virtual void	handle_packet (char*, int) = 0;
    char*		get_address();

protected:
    int			get_fd ();
    int			send_packet (char*, int);
    int			is_connected();

    virtual void	on_connect();
    virtual void	on_disconnect();

    void		set_next (NETCLIENT*);
    NETCLIENT*		get_next ();

private:
    int			s;
    struct sockaddr_in	sin;

    int			read_packet (char*, int);

    NETCLIENT*		next;
};

/*
 * NETSERVER is a convienent way of building networking server. It will bind
 * itself to a given port and handle clients as needed.
 *
 */
class NETSERVER {
public:
    NETSERVER();
    virtual		~NETSERVER();

    int			create (int);
    void		poll ();

    virtual void	handle_incoming() = 0;

protected:
    int			get_fd ();

    void		add_client (NETCLIENT*);
    void		remove_client (NETCLIENT*);

private:
    int			s;
    NETCLIENT*		clients;
};

#endif
