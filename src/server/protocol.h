/*
 * LunchMoney 2.0 Daemon - protocol.h (LunchMoney Protocol)
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
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "lmclient.h"
#include "network.h"

/* PROTOCOL_FUNC is a protocol function */
typedef void (PROTOCOL_FUNC) (LMCLIENT*, char*);

/*
 * PROTOCOL_CMD is a protocol command.
 */
typedef struct {
    char*		cmd;					/* command */
    PROTOCOL_FUNC*	func;					/* function */
} PROTOCOL_CMD;

/* PROTOCOL_RESP_cat_xxx are all responses we know. */
/* A [cat] of 'ERR' means a fatal error */
#define PROTOCOL_RESP_ERR_UNKCMD	"300 Huh?"
#define PROTOCOL_RESP_ERR_SERVEREXIT	"400 Server dieing, bye"
#define PROTOCOL_RESP_ERR_PLAYERSET	"300 Player name already set"
#define PROTOCOL_RESP_ERR_DUPNAME	"300 Duplicate player name"
#define PROTOCOL_RESP_ERR_INVARG	"300 Invalid arguments"
#define PROTOCOL_RESP_ERR_PLAYERNOTSET	"300 Player name must be set first"
#define PROTOCOL_RESP_ERR_ALREADYINGAME	"300 You already are in a game"
#define PROTOCOL_RESP_ERR_MUSTBEINAGAME	"300 You must be in a game first"
#define PROTOCOL_RESP_ERR_NOPLAYER	"300 No such player"
#define PROTOCOL_RESP_ERR_ALREADYINGAME	"300 You already are in a game"
#define PROTOCOL_RESP_ERR_NOSUCHGAME	"300 No such game"
#define PROTOCOL_RESP_ERR_GAMESTARTED	"300 Game has already started"
#define PROTOCOL_RESP_ERR_NOTURN	"300 It's not your turn"
#define PROTOCOL_RESP_ERR_NOCARD	"300 You don't have that card"
#define PROTOCOL_RESP_ERR_CANTHURTSELF	"300 You shouldn't attack yourself"

/* A [cat] of 'MSG' means an informational message */
#define PROTOCOL_RESP_MSG_HELLO		"100 Welcome!"
#define PROTOCOL_RESP_MSG_BYE		"100 See ya!"
#define PROTOCOL_RESP_MSG_PLAYEROK	"100 Player name successfully set"
#define PROTOCOL_RESP_MSG_GAMEOK	"100 New game successfully created"
#define PROTOCOL_RESP_MSG_GAMESTARTS	"200 The game has began"
#define PROTOCOL_RESP_MSG_GAMEINFO	"101 %u,%s,%u,%u"
#define PROTOCOL_RESP_MSG_GAMELISTSENT	"100 All sent"
#define PROTOCOL_RESP_MSG_PLAYERINFO	"102 %u,%s,%u"
#define PROTOCOL_RESP_MSG_JOINOK	"100 Game joined"
#define PROTOCOL_RESP_MSG_PLAYERJOIN	"202 %u: Player joined this game"
#define PROTOCOL_RESP_MSG_PLAYERLEAVE	"203 %u: Player left this game"
#define PROTOCOL_RESP_MSG_CARDINFO	"103 %u,%s"
#define PROTOCOL_RESP_MSG_CARDSOK	"100 Cards sent"
#define PROTOCOL_RESP_MSG_CARDPLAYED	"100 Card played"
#define PROTOCOL_RESP_MSG_ACTION	"201 %u,%u,%u,%u"
#define PROTOCOL_RESP_MSG_YOURTURN	"204 It's your turn"
#define PROTOCOL_RESP_MSG_DEFEND	"205 Defend thyself"
#define PROTOCOL_RESP_MSG_DEFENDED	"206 %u,%u"
#define PROTOCOL_RESP_MSG_IDLEOK	"100 You successfully did nothing"
#define PROTOCOL_RESP_MSG_DROPPED	"207 %u,%u"
#define PROTOCOL_RESP_MSG_IDLED		"209 %u"
#define PROTOCOL_RESP_MSG_DROPOK	"100 Cards dropped"

/*
 * PROTOCOL is the LunchMoney Protocol Handler.
 */
class PROTOCOL {
public:
    static	void		handle_command (LMCLIENT*, char*, char*);

    static	PROTOCOL_FUNC	cmd_unknown;
    static	PROTOCOL_FUNC	cmd_quit;
    static	PROTOCOL_FUNC	cmd_player;
    static	PROTOCOL_FUNC	cmd_newgame;
    static	PROTOCOL_FUNC	cmd_listgames;
    static	PROTOCOL_FUNC	cmd_playerinfo;
    static	PROTOCOL_FUNC	cmd_joingame;
    static	PROTOCOL_FUNC	cmd_getcards;
    static	PROTOCOL_FUNC	cmd_action;
    static	PROTOCOL_FUNC	cmd_defend;
    static	PROTOCOL_FUNC	cmd_idle;
    static	PROTOCOL_FUNC	cmd_drop;
};
#endif
