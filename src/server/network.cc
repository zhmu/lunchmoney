/*
 * LunchMoney 2.0 Daemon - network.cc (Networking Code)
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "network.h"

/*
 * NETSERVER::NETSERVER()
 *
 * This will create a blank, unitialized network server.
 *
 */
NETSERVER::NETSERVER() {
    // we don't have a socket or clients... yet
    s = -1; clients = NULL;
}

/*
 * NETSERVER::~NETSERVER()
 *
 * This will deinitialize the network service, as far as needed.
 *
 */
NETSERVER::~NETSERVER() {
    NETCLIENT* client = clients;
    NETCLIENT* nextclient;

    // ditch all clients
    while (client) {
	// bye
	client->shutdown(); nextclient = client->get_next();
	delete client;

	// next
	client = nextclient;
    }

    // do we have a socket?
    if (s != -1)
	// yes. bye
	close (s);
}

/*
 * NETSERVER::get_fd()
 *
 * This will return the file descriptor of the server socket.
 *
 */
int
NETSERVER::get_fd() {
    return s;
}

/*
 * NETSERVER::create (int portno)
 *
 * This will create a networking service and bind it to TCP port [portno]. It
 * will return 0 on success or -1 on failure.
 *
 */
int
NETSERVER::create (int portno) {
    struct sockaddr_in sin;

    // create a socket
    s = socket (AF_INET, SOCK_STREAM, 0);
    if (s < 0)
	// this failed. complain
	return -1;

    // build the structure for bind()
    bzero (&sin, sizeof (struct sockaddr_in));
    sin.sin_len = sizeof (struct sockaddr_in);
    sin.sin_family = AF_INET;
    sin.sin_port = htons (portno);

    // bind the socket
    if (bind (s, (struct sockaddr*)&sin, sizeof (struct sockaddr_in)) < 0)
	// this failed. complain
	return -1;

    // now, tell the socket to be nice and listen
    if (listen (s, 3) < 0)
	// this failed. complain
	return -1;

    // all went well
    return 0;
}

/*
 * NETSERVER::add_client (NETCLIENT* c)
 *
 * This will add client [c] to the list of clients.
 *
 */
void
NETSERVER::add_client (NETCLIENT* c) {
    NETCLIENT* client = clients;

    // scan for the end of the chain
    if (client)
	while (client->get_next()) client = client->get_next();

    // add the client
    if (client)
	client->set_next (c);
    else
	clients = c;
}

/*
 * NETSERVER::remove_client (NETCLIENT* c)
 *
 * This will remove [c] from the list of clients.
 *
 */
void
NETSERVER::remove_client (NETCLIENT* c) {
    NETCLIENT* client = clients;
    NETCLIENT* prevclient = NULL;

    // scan the clients
    while (client) {
	// match?
	if (client == c) {
	    // yes. did we have a previous one?
	    if (prevclient)
		// yes. update the previous client's next client (whoa...)
		prevclient->set_next (c->get_next());
	    else
		// no. this must be the initial first client. change the pointer
		clients = c->get_next();

	    // all set
	    return;
	}

	// next
	prevclient = client; client = client->get_next();
    }

    // no client to remove
}

/*
 * NETSERVER::poll()
 *
 * This will 'keep an eye' on the socket. If any client gets data, it will
 * handle this data as needed. If a new client attaches, this function will
 * call the the virtual function 'handle_incoming', which should take care of
 * the new client.
 *
 */
void
NETSERVER::poll() {
    fd_set	fds;
    NETCLIENT*	client = clients;
    int 	fd, max_fd = s;
    char	packet[NET_MAX_PACKET_LEN];
    int		sz;

    // if we don't have a socket, leave
    if (s < 0)
	return;

    // initialize the set
    FD_ZERO (&fds);
    FD_SET (s, &fds);

    // add all clients
    while (client) {
	// fetch the file descriptor
	fd = client->get_fd();

	// add this one
	FD_SET (fd, &fds);

	// set a new max file descriptor, if needed
	if (max_fd < fd)
	    max_fd = fd;

	// next
	client = client->get_next();
    }

    // wait until we have data
    if (select (max_fd + 1, &fds, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)NULL) < 0)
	// this failed. leave
	return;

    // do we have a new connection?
    if (FD_ISSET (s, &fds)) {
	// yes. handle the new connection and leave
	handle_incoming();
	return;
    }

    // scan the client table (we must figure out who should have the data)
    client = clients;
    while (client) {
	// match?
	if (FD_ISSET (client->get_fd(), &fds)) {
	    // yes. read the packet
	    sz = client->read_packet (packet, NET_MAX_PACKET_LEN);
	    if (sz > 0) {
		// this worked. get the client to handle the data
		client->handle_packet (packet, sz);

		// is this client unconnected now?
		if (!client->is_connected()) {
		    // yes. get rid of it
		    remove_client (client);
		    delete client;
		}
	    } else {
		// the connection is probably dead. remove the client
		remove_client (client);
		delete client;
	    }

	    // bye bye
	    return;
	}

	// next
	client = client->get_next();
    }
}

/*
 * NETCLIENT::NETCLIENT()
 *
 * This will initialize a blank, unattached network client.
 *
 */
NETCLIENT::NETCLIENT() {
    // not attached... yet
    s = -1; next = NULL;
}

/*
 * NETCLIENT::~NETCLIENT()
 *
 * This will deinitialize the network client.
 *
 */
NETCLIENT::~NETCLIENT() {
    // are we attached?
    if (s != -1)
	// yes. close it down
	close (s);
}

/*
 * NETCLIENT::set_next (NETCLIENT* n)
 *
 * This will set our next client to [n].
 *
 */
void
NETCLIENT::set_next (NETCLIENT* n) {
    next = n;
}

/*
 * NETCLIENT::get_next()
 *
 * This will retrieve our next client.
 *
 */
NETCLIENT*
NETCLIENT::get_next() {
    return next;
}

/*
 * NETCLIENT::accept (int fd)
 *
 * This will accept a connection from [fd] and set the client up according to
 * it. This will return 0 on success or -1 on failure.
 *
 */
int
NETCLIENT::accept (int fd) {
    socklen_t sz = sizeof (struct sockaddr_in);

    // accept a connection
    s = ::accept (fd, (struct sockaddr*)&sin, &sz);
    if (s < 0)
	// this failed. complain
	return -1;

    // call the on_connect() function
    on_connect();

    // this worked
    return 0;
}

/*
 * NETCLIENT::read_packet (char* dest, int maxlen)
 *
 * This will read up to [maxlen] bytes into [dest]. It will return -1 on
 * failure or the number of bytes read on success.
 *
 */
int
NETCLIENT::read_packet (char* dest, int maxlen) {
    // fetch the data
    return recv (s, dest, maxlen, 0);
}

/*
 * NETCLIENT::send_packet (char* dest, int len)
 *
 * This will send up [len] bytes from [dest] to the client. It will return
 * 0 on success or -1 on failure.
 *
 */
int
NETCLIENT::send_packet (char* dest, int len) {
    // go
    return (send (s, dest, len, 0) == len) ? 0 : -1;
}

/*
 * NETCLIENT::get_fd()
 *
 * This will return the file descriptor of this client.
 *
 */
int
NETCLIENT::get_fd() {
    return s;
}

/*
 * NETCLIENT::is_connected()
 *
 * This will return 1 if we are connected or 0 if not.
 *
 */
int
NETCLIENT::is_connected() {
    return (s == -1) ? 0 : 1;
}

/*
 * NETCLIENT::shutdown()
 *
 * This will get rid of the connection.
 * 
 */
void
NETCLIENT::shutdown() {
    // are we connected?
    if (s != -1) {
	// yes. call the on_disconnect() function
	on_disconnect();

	// close the socket
	close (s);

	// now, we are no longer connected
	s = -1;
    }
}

/*
 * NETCLIENT::get_address()
 *
 * This returns the IP address of the client or a question mark if we are not
 * connected.
 *
 */
char*
NETCLIENT::get_address() {
    // are we connected?
    if (s < 0)
	// no. return a question mark
	return "?";

    // return the address
    return inet_ntoa (sin.sin_addr);
}

/*
 * NETCLIENT::on_connect()
 *
 * This will be called when a new connection has been made.
 *
 */
void
NETCLIENT::on_connect() {
    // nothing by default
}

/*
 * NETCLIENT::on_disconnect()
 *
 * This will be called when the client has disconnected.
 *
 */
void
NETCLIENT::on_disconnect() {
    // nothing by default
}
