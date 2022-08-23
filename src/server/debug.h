/*
 * LunchMoney 2.0 Daemon - debug.h (Debugging code)
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
#ifndef __DEBUG_H__
#define __DEBUG_H__

/* DPRINTF is a wrapper to lmoney->debug->log */
#define DPRINTF lmoney->debug->log

/* DEBUG_xxx are the debugging message types */
#define DEBUG_WARNING		0
#define DEBUG_INFO		1
#define DEBUG_FATAL		2
#define DEBUG_CRITICAL		3

/*
 * DEBUGGER is a convinient way of logging debugging messages.
 *
 */
class DEBUGGER {
public:
    DEBUGGER();

    void	log (int, char*, ...);
};
#endif
