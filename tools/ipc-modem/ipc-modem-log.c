/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2011 Paul Kocialkowsk <contact@paulk.fr>
 * Copyright (C) 2022 Denis 'GNUtoo' Carikli <GNUtoo@cyberdimension.org>
 *
 * libsamsung-ipc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * libsamsung-ipc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libsamsung-ipc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include "ipc-modem-log.h"

static enum log_target log_target;

void modem_log_handler(void *user_data, const char *msg)
{
	int i, l;
	char *message;

	message = strdup(msg);
	l = strlen(message);

	if (l > 1) {
		for (i = l ; i > 0 ; i--) {
			if (message[i] == '\n')
				message[i] = 0;
			else if (message[i] != 0)
				break;
		}

		if (log_target == LOG_TO_STDOUT && user_data)
			printf("[%s] %s\n", (char *)user_data, message);
		else if (log_target == LOG_TO_STDOUT)
			printf("[%s] %s\n", MODEM_LOG_DEBUG, message);
		else if (log_target == LOG_TO_SYSLOG && user_data)
			syslog(LOG_INFO,
			       "[%s] %s\n", (char *)user_data, message);
		else if (log_target == LOG_TO_SYSLOG)
			syslog(LOG_INFO, "[%s] %s\n", MODEM_LOG_DEBUG, message);
	}

	free(message);
}

void modem_log_handler_quiet(__attribute__((unused)) void *user_data,
			     __attribute__((unused)) const char *msg)
{
}

void ipc_modem_log(__attribute__((unused)) struct ipc_client *client,
		   char *prefix, const char *message, ...)
{
	va_list args;
	char buffer[4096];

	va_start(args, message);
	vsnprintf((char *) &buffer, sizeof(buffer), message, args);
	va_end(args);

	modem_log_handler(prefix, buffer);
}

void ipc_modem_set_log_target(enum log_target target)
{
	log_target = target;
}

