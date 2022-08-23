/*
 * LunchMoney 2.0 Daemon - main.cc ("The Glue that Binds")
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
#include <signal.h>
#include <stdlib.h>
#include "lmoney.h"

// LMONEY is the LunchMoney daemon
LMONEY* lmoney;

/*
 * sig_int (int x)
 *
 * This will be called on interrupt signals.
 *
 */
void
sig_int (int x) {
    // deinitialize all
    lmoney->done();

    // bye
    exit (EXIT_SUCCESS);
}

/*
 * main(int argc, char** argv)
 *
 * This will launch the daemon.
 *
 */
int
main(int argc, char** argv) {
    // create the object
    lmoney = new LMONEY();

    // connect the INTerupt as needed
    signal (SIGINT, sig_int);

    // to initialize, run, deinitialize and die... that's the meaning of life
    lmoney->init();
    lmoney->run();
    lmoney->done();

    // bye bye
    return EXIT_SUCCESS;
}
