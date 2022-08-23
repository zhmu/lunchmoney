/*
 * LunchMoney Client - (c) 2002 Rink Springer, GPL
 *
 */
#include "lmoneyc.h"

LMONEYC* lmoneyc;

/*
 * main(int argc, char** argv)
 *
 * What more needs to be said?
 *
 */
int
main(int argc, char** argv) {
    // create the object
    lmoneyc = new LMONEYC();

    // initialize, run and die... that's the meaning of life.
    lmoneyc->init();
    lmoneyc->run();
    lmoneyc->done();

    // bye bye
    delete lmoneyc;
    return 0;
}
