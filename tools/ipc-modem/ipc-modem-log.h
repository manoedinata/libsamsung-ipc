/*
 * This file is part of libsamsung-ipc.
 *
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
#ifndef IPC_MODEM_LOG_H
#define IPC_MODEM_LOG_H

#include <stdarg.h>

#include <samsung-ipc.h>

#define MODEM_LOG_ERROR "E"
#define MODEM_LOG_INFO  "I"
#define MODEM_LOG_DEBUG "D"

enum log_target {
	LOG_TO_STDOUT,
	LOG_TO_SYSLOG,
};

void ipc_modem_log(struct ipc_client *client,
		   char *prefix, const char *message, ...);
void modem_log_handler(void *user_data, const char *msg);
void modem_log_handler_quiet(void *user_data, const char *msg);
void ipc_modem_set_log_target(enum log_target target);

#endif /* IPC_MODEM_LOG_H */
