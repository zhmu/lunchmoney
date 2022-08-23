/*
 * LunchMoney 2.0 Daemon - lmoney.h (Main LunchMoney module)
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
#ifndef __LMSERVER_H__
#define __LMSERVER_H__

#include "network.h"

/*
 * LMSERVER is the actual LunchMoney daemon server.
 */
class LMSERVER : public NETSERVER {
public:
    void		handle_incoming();
};

#endif
