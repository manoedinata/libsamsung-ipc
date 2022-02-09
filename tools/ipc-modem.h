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
#ifndef IPC_MODEM_H
#define IPC_MODEM_H

#define MODEM_STATE_LPM     0
#define MODEM_STATE_NORMAL  2
#define MODEM_STATE_SIM_OK  4

#define MODEM_LOG_ERROR "E"
#define MODEM_LOG_INFO  "I"
#define MODEM_LOG_DEBUG "D"

enum command {
	CMD_NONE,
	CMD_START,
	CMD_BOOT,
	CMD_POWER_ON,
	CMD_POWER_OFF,
};

enum log_target {
	LOG_TO_STDOUT,
	LOG_TO_SYSLOG,
};

struct ipc_modem_data {
	struct ipc_client *client;
	char call_number[14];
	char sim_pin[8];
	enum command command;
	bool debug;
	bool dry_run;
	/* State */
	bool call_done;
	bool in_call;
	bool out_call;
};

void ipc_modem_log(struct ipc_client *client,
		   char *prefix, const char *message, ...);

#endif /* IPC_MODEM */
