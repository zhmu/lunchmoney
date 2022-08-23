/*
 * LunchMoney Client - (c) 2002 Rink Springer, GPL
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "player.h"

/*
 * PLAYER::PLAYER()
 *
 * This will initialize a blank player.
 *
 */
PLAYER::PLAYER() {
    // no id, no name, no life...
    id = 0; name[0] = 0; life = 0;
}

/*
 * PLAYER::set_[xxx](...)
 *
 * This will set the [xxx] property to [...].
 *
 */
void PLAYER::set_id (int x) { id = x; }
void PLAYER::set_life (int x) { life = x; }
void PLAYER::set_name (char* x) { strncpy (name, x, PLAYER_MAX_NAME_LEN); }

/*
 * PLAYER::get_[xxx])(...)
 *
 * This will return the [xxx] property.
 *
 */
int PLAYER::get_id() { return id; }
int PLAYER::get_life() { return life; }
char* PLAYER::get_name() { return name; }
