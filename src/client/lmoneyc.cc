/*
 * LunchMoney Client - (c) 2002 Rink Springer, GPL
 *
 */
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "command.h"
#include "lmoneyc.h"

/*
 * LMONEYC::LMONEYC()
 *
 * This will construct our object.
 *
 */
LMONEYC::LMONEYC() {
    // reset all pointers to null
    p = NULL; main = NULL; input = NULL; content = NULL; deck = NULL;
}

/*
 * LMONEYC::init()
 *
 * This will initialize the LunchMoney client.
 * 
 */
void
LMONEYC::init() {
    // initialize the protocol
    p = new PROTOCOL();

    // initialize curses (the library :)
    initscr(); cbreak();
    nonl(); intrflush (stdscr, FALSE);
    keypad (stdscr, TRUE);

    // create the windows
    main = newwin (LINES - 1, COLS, 0, 0);
    if (main == NULL) {
	fprintf (stderr, "lmoneyc: couldn't create main window\n");
	exit (1);
    }
    content = subwin (main, LINES - 3, COLS - 2, 1, 1);
    if (content == NULL) {
	fprintf (stderr, "lmoneyc: couldn't create content window\n");
	exit (1);
    }
    input = newwin (1, COLS, LINES - 1, 0);
    if (input == NULL) {
	fprintf (stderr, "lmoneyc: couldn't create input window\n");
	exit (1);
    }

    // draw a cute box for the main window
    box (main, 0, 0);

    // enable scrolling
    scrollok (content, TRUE);
   
    // redraw all windows
    wrefresh (main); wrefresh (content); wrefresh (input);

    // tell the user about us
    WPRINTF ("LunchMoney Client - (c) 2002 Rink Springer, GPL");

    // load the deck
    deck = new DECK();
    deck->load (CONF_DECK_FILENAME);
}

/*
 * LMONEYC::wprintf (char* cmd, ...)
 *
 * This will printf(cmd, [...]) into the content window and refresh it.
 *
 */
void
LMONEYC::wprintf (char* cmd, ...) {
    char* ptr;
    va_list ap;

    // build the buffer
    va_start (ap, cmd);
    vasprintf (&ptr, cmd, ap);
    va_end (ap);

    // print it
    waddstr (content, ptr);
    waddstr (content, "\n");

    // free the buffer
    free (ptr);

    // refresh the buffer
    wrefresh (content);
}

/*
 * LMONEYC::stop()
 *
 * This will simply set the [running] field to zero, indicating we should
 * stop.
 *
 */
void
LMONEYC::stop() {
    running = 0;
}

/*
 * LMONEYC::run()
 *
 * This will run the LunchMoney client.
 *
 */
void
LMONEYC::run() {
    char cmd[CONF_MAX_COMMAND_LEN];

    // we are running
    running = 1;

    // keep going
    while (running) {
	// let's get the command from our master
	wclear (input);
	waddstr (input, ">");
	echo(); mvwgetnstr (input, 0, 2, cmd, CONF_MAX_COMMAND_LEN); noecho();

	// echo the command
	WPRINTF ("> %s", cmd);

	// handle the command
	COMMAND::handle (cmd);
    }
}

/*
 * LMONEYC::done()
 *
 * This will deinitialize the LunchMoney client.
 *
 *
 */
void
LMONEYC::done() {
    // got a protocol
    if (p)
	// yes. bye
	delete p;

    // lift the curse :)
    endwin();
}
