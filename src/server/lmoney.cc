/*
 * LunchMoney 2.0 Daemon - lmoney.cc (Main LunchMoney module)
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
#include <sysexits.h>
#include <stdlib.h>
#include "config.h"
#include "crowd.h"
#include "lmoney.h"

/*
 * LMONEY::LMONEY()
 *
 * This will initialize the LunchMoney class.
 *
 */
LMONEY::LMONEY() {
    // no objects under us yet
    server = NULL; debug = NULL; crowd = NULL; deck = NULL;
}

/*
 * LMONEY::init()
 *
 * This will initialize our LunchMoney daemon. It will die on any error.
 *
 */
void
LMONEY::init() {
    // initialize the debugger
    debug = new DEBUGGER();

    // initialize the main deck
    deck = new DECK();
    if (deck->load (CONF_CARD_DATAFILE) < 0) {
	// this failed. complain
	fprintf (stderr, "LMONEY::init(): unable to load card datafile '%s', exiting\n", CONF_CARD_DATAFILE);
	exit (EX_CONFIG);
    }

    // initialize the crowd of players
    crowd = new CROWD();

    // initalize the playground of games
    playground = new PLAYGROUND();

    // initialize the server
    server = new LMSERVER();
    if (server->create (CONF_DEFAULT_PORTNO) < 0) {
	// this failed. complain
	fprintf (stderr, "LMONEY::init(): unable to create network server socket\n");
	exit (EX_UNAVAILABLE);
    }
}

/*
 * LMONEY::run()
 *
 * This will run our LunchMoney daemon.
 *
 */
void
LMONEY::run() {
    // keep running the server
    while (1)
	server->poll();
}

/*
 * LMONEY::done()
 *
 * This will deinitialize our LunchMoney game.
 *
 */
void
LMONEY::done() {
    // get rid of the server
    if (server)
	delete server;

    // get rid of the playground
    if (playground)
	delete playground;

    // get rid of the crowd
    if (crowd)
	delete crowd;

    // get rid of the debugger
    if (debug)
	delete debug;
}
