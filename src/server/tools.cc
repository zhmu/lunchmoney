/*
 * LunchMoney 2.0 Daemon - tools.cc (Toolkit Functions)
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
#include <stdarg.h>
#include <stdlib.h>

/*
 * format (char* packet, char* fmt, ...)
 *
 * This will format [packet] into [...] according to [fmt]. It will
 * return 0 if all elements could be parsed or -1 if not.
 *
 * This was written because scanf() appears to choke on things like
 * '%u,%s,%u', which makes it useless for us.
 *
 * Notice: This function does not support things like '%s%u%s'. We don't
 * need them anyway, so we don't care.
 *
 */
int
format (char* packet, char* fmt, ...) {
    va_list va;
    char*   ptr;
    int*    i;

    // handle all arguments
    va_start (va, fmt); 
    
    // handle all arguments
    while (*fmt) {
	// figure out the type
	if (*fmt == '%') {
	    // we got a special formatter. handle it
	    switch (*++fmt) {
		case 's': // string
			  ptr = va_arg (va, char*);
			  while ((*packet) && (*packet != *(fmt + 1))) {
			      *ptr++ = *packet++;
			  }
			  *ptr = 0;
			  break;
		case 'i': // integer
			  i = va_arg (va, int*); 
			  *i = strtol (packet, &ptr, 10);
			  if ((*ptr) && (*ptr != *(fmt + 1))) {
			      // garbage after the number, discard
			      va_end (va);
			      return -1;
			  }

			  // decrement the pointer so we won't skip the char
			  packet = ptr;
			  break;
	    }

	    // next
	    fmt++;
	} else {
	    while ((*fmt) && (*packet) && (*fmt != '%') && (*fmt == *packet)) {
	        fmt++; packet++;
	    }

	    // hit a new control char or end of the string?
	    if ((*fmt) && (*fmt != '%')) {
	        // no. complain
		va_end (va);
		return -1;
	    }
	}
    }

    // we're all done
    va_end (va);

    // if we hit the end of the string, all went ok
    return (*packet) ? -1 : 0;
}
