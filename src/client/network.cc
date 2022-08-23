/*
 * LunchMoney Client - (c) 2002 Rink Springer, GPL
 *
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "lmoneyc.h"
#include "network.h"

/*
 * sig_urg_wrap(int x)
 *
 * This is a SIGURG wrapper to lmoneyc->p->incoming().
 *
 */
void
sig_urg_wrap (int x) {
    lmoneyc->p->incoming();
}

/*
 * NETCLIENT::NETCLIENT()
 *
 * This will create a blank client session.
 *
 */
NETCLIENT::NETCLIENT() {
    // no socket today... we feel so empty now...
    s = -1; childid = -1;
}

/*
 * NETCLIENT::~NETCLIENT()
 *
 * This will get rid of the client session.
 *
 */
NETCLIENT::~NETCLIENT() {
    // shut the client down
    shutdown();
}

/*
 * NETCLIENT::is_connected()
 *
 * This will return zero if we connected or -1 if we are not.
 *
 */
int
NETCLIENT::is_connected() {
    return (s == -1) ? -1 : 0;
}

/*
 * NETCLIENT::shutdown()
 *
 * This will shut the client down, and get rid of any pesky children that
 * might be lying around...
 *
 */
void
NETCLIENT::shutdown() {
    // do we have a valid socket?
    if (s != -1)
	// yes. bye bye
	close (s);

    // do we have a child?
    if (childid != -1)
	// yes. kill it (and pray we won't get prosecuted for it)
	kill (childid, SIGKILL);

    // no longer connected now, and no infants anymore
    childid = s = -1;
}

/*
 * NETCLIENT::connect (char* hostip, int port)
 *
 * This will try to connect to host [hostip] on port [port]. It will return
 * zero on success or -1 on failure.
 *
 */
int
NETCLIENT::connect (char* hostip, int port) {
    struct sockaddr_in sin;
    struct hostent* he;
    int i;

    // create the socket
    s = socket (PF_INET, SOCK_STREAM, 0);
    if (s < 0)
	// this failed. bail out
	return -1;

    // fetch the address
    he = gethostbyname2 (hostip, PF_INET);
    if (he == NULL)
	// this failed. bail out
	return -1;

    // build the address
    bzero (&sin, sizeof (struct sockaddr_in));
    sin.sin_len = sizeof (struct sockaddr_in);
    sin.sin_family = he->h_addrtype;
    sin.sin_port = htons (port);
    bcopy (he->h_addr, &sin.sin_addr, he->h_length);

    // try to connect
    if (::connect (s, (struct sockaddr*)&sin, sizeof (struct sockaddr_in)) < 0)
	return -1;

    // this all worked perfectly. create a socket pair and fork()
    if (pipe ((int*)&spair) < 0) {
	// damn, there goes the dream... complain
	close (s); s = -1;
	return -1;
    }

    // we need a non-blocking input pipe
    if (fcntl (spair[0], F_SETFL, O_NONBLOCK, 1) < 0) {
	// this failed. complain (geez, is that socket a blockhead or what?)
	close (spair[0]); close (spair[1]); close (s); s = -1;
	return -1;
    }

    // fetch our parent id
    parentid = getpid();

    // always initially incoming().
    disable_incoming();

    // fork
    i = fork();
    if (i < 0) {
	// this failed. argh!
	close (spair[0]); close (spair[1]); close (s); s = -1;
	return -1;
    }

    // figure our position out
    if (i == 0) {
	// we are the child! let's act like a dumb infant and keep polling
	// for data, bugging our parent when we have some...
	piper();

	// our young life has ended now...
	exit (0);
    }

    // save our child id
    childid = i;

    // all set and good to go
    return 0;
}

/*
 * NETCLIENT::get_[xxx]()
 *
 * This will return the [xxx] property.
 *
 */
int NETCLIENT::get_fd() { return s; }

/*
 * NETCLIENT::send_packet (char* data)
 *
 * This will send nul-terminated [data] to the client. It will return zero
 * on success or -1 on failure.
 *
 */
int
NETCLIENT::send_packet (char* data) {
    // build the new packet
    char ptr[NET_MAX_PACKET_LEN];

    // got a valid socket?
    if (s == -1)
	// no. complain
	return -1;

    // format the packet
    sprintf (ptr, "%s\n", data);

    // it's all up to send() now
    return ((unsigned int)send (s, ptr, strlen (ptr), 0) == strlen (ptr)) ? 0 : -1;
}

/*
 * NETCLIENT::get_packet(char* data)
 *
 * This will read up to NET_MAX_PACKET_LEN bytes into buffer [data]. It will
 * return 0 on success or -1 on failure.
 *
 */
int
NETCLIENT::get_packet(char* data) {
    int size, i;

    // read the data
    size = read (spair[0], data, NET_MAX_PACKET_LEN);

    // got any data?
    if (size < 0)
	// no. bail out
	return -1;

    // kill any newlines
    i = size - 1;
    while ((data[i] == 0xa) || (data[i] == 0xd))
	data[i--] = 0;

    // this worked
    return 0;
}

/*
 * NETCLIENT::wait_for_data(char* reply)
 *
 * This will wait for a reply, which will be copied into [reply] using
 * get_packet().
 *
 */
void
NETCLIENT::wait_for_data(char* reply) {
    fd_set fd;

    // select() until we have data
    FD_ZERO (&fd);
    FD_SET (spair[0], &fd);
    select (spair[0] + 1, &fd, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)NULL);

    // grab the data
    get_packet (reply);
}

/*
 * NETCLIENT::piper()
 *
 * This is the 'main' procedure for the child. It will simply keep reading any
 * data from the socket and tell the parent about it.
 *
 */
void
NETCLIENT::piper() {
    fd_set fd;
    int size;
    char data[NET_MAX_PACKET_LEN];

    // initialize the set
    FD_ZERO (&fd);
    FD_SET (s, &fd);

    // keep going
    while (1) {
	// keep polling
	if (select (s + 1, &fd, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)NULL) < 0)
	    return;

	// yay, we have data. get it
	size = recv (s, data, NET_MAX_PACKET_LEN, 0);
	if (size > 0) {
	    // tell our parent about it
	    write (spair[1], data, size);

	    // daddddy!!!!!
	    kill (parentid, SIGURG);
	} else
	    // the socket probably died. bye
	    return;
    }
}

/*
 * NETCLIENT::disable_incoming()
 *
 * This will cause the incoming() routine not to be called whenever there is
 * data.
 *
 */
void
NETCLIENT::disable_incoming() {
    signal (SIGURG, SIG_IGN);
}

/*
 * NETCLIENT::enable_incoming()
 *
 * This will cause the incoming() routine to be called whenever there is data.
 *
 */
void
NETCLIENT::enable_incoming() {
    signal (SIGURG, sig_urg_wrap);
}
