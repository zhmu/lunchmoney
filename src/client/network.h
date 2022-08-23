/*
 * LunchMoney Client - (c) 2002 Rink Springer, GPL
 *
 */
#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>

/* NET_MAX_PACKET_LEN is the maximum length of a network packet */
#define NET_MAX_PACKET_LEN	4096

/*
 * NETCLIENT is a client connected to a network service.
 *
 */
class NETCLIENT {
public:
    NETCLIENT ();
    ~NETCLIENT ();
    int connect(char*,int);

    int is_connected(); 
    int	get_fd();

    void shutdown();

    int  send_packet (char*);
    int  get_packet(char*);
    int  waiting_for_data();

    void wait_for_data(char*);
    void piper();

    void enable_incoming();
    void disable_incoming();

private:
    int s, childid, parentid;
    int spair[2];
    struct sockaddr addr;

    int  waiting;
};
#endif
