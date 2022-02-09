/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2011 Paul Kocialkowsk <contact@paulk.fr>
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

#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <syslog.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <samsung-ipc.h>

#include "ipc-modem.h"

enum log_target log_target;

int state = MODEM_STATE_LPM;
int seq;
int out_call;

int seq_get(void)
{
	if (seq == 0xff)
		seq = 0x00;

	seq++;

	return seq;
}

void modem_snd_no_mic_mute(struct ipc_client *client)
{
	uint8_t data = 0;

	ipc_client_send(client, seq_get(), IPC_SND_MIC_MUTE_CTRL, IPC_TYPE_SET,
			(void *) &data, 1);
}

void modem_snd_clock_ctrl(struct ipc_client *client)
{
	uint8_t data = 0x01;

	ipc_client_send(client, seq_get(), IPC_SND_CLOCK_CTRL, IPC_TYPE_EXEC,
			(void *) &data, 1);
}

void modem_snd_spkr_volume_ctrl(struct ipc_client *client)
{
	uint16_t data = 0x0411;

	ipc_client_send(client, seq_get(), IPC_SND_SPKR_VOLUME_CTRL,
			IPC_TYPE_SET, (void *) &data, 2);
}

void modem_snd_audio_path_ctrl(struct ipc_client *client)
{
	uint8_t data = 0x01;

	ipc_client_send(client, seq_get(), IPC_SND_AUDIO_PATH_CTRL,
			IPC_TYPE_SET, (void *) &data, 1);
}



void modem_exec_call_out(struct ipc_client *client, char *num)
{
	struct ipc_call_outgoing_data call_out;

	modem_snd_no_mic_mute(client);

	memset(&call_out, 0, sizeof(struct ipc_call_outgoing_data));

	call_out.type = IPC_CALL_TYPE_VOICE;
	call_out.identity = IPC_CALL_IDENTITY_DEFAULT;
	call_out.number_length = strlen(num);
	/* TODO: Add support for prefixes */
	/* 0x21 = +33 */
	call_out.prefix = IPC_CALL_PREFIX_NONE; //0x21;//IPC_CALL_PREFIX_NONE;
	memcpy(call_out.number, num, call_out.number_length);

	ipc_client_send(client, seq_get(), IPC_CALL_OUTGOING, IPC_TYPE_EXEC,
			(void *) &call_out,
			sizeof(struct ipc_call_outgoing_data));

	out_call = 1;

	modem_snd_no_mic_mute(client);
	modem_snd_spkr_volume_ctrl(client);
	modem_snd_audio_path_ctrl(client);
}

void modem_exec_call_answer(struct ipc_client *client)
{
	modem_snd_clock_ctrl(client);

	ipc_client_send(client, seq_get(), IPC_CALL_ANSWER, IPC_TYPE_EXEC, NULL,
			0);

	modem_snd_no_mic_mute(client);
}

void modem_get_call_list(struct ipc_client *client)
{
	ipc_client_send(client, seq_get(), IPC_CALL_LIST, IPC_TYPE_GET, NULL,
			0);

	modem_snd_no_mic_mute(client);
}

void modem_exec_power_normal(struct ipc_client *client)
{
	uint16_t data = 0x0202;

	ipc_client_send(client, seq_get(), IPC_PWR_PHONE_STATE, IPC_TYPE_EXEC,
			(void *) &data, sizeof(data));
}

void modem_set_sms_device_ready(struct ipc_client *client)
{
	ipc_client_send(client, seq_get(), IPC_SMS_DEVICE_READY, IPC_TYPE_SET,
			NULL, 0);
}

void modem_set_sec_pin_status(struct ipc_client *client, char *pin1, char *pin2)
{
	struct ipc_sec_pin_status_request_data pin_status;

	ipc_modem_log(client, MODEM_LOG_INFO, "Sending PIN1 unlock request\n");

	ipc_sec_pin_status_setup(&pin_status, IPC_SEC_PIN_TYPE_PIN1, pin1,
				 pin2);

	ipc_client_send(client, seq_get(), IPC_SEC_PIN_STATUS, IPC_TYPE_SET,
			(void *) &pin_status, sizeof(pin_status));
}

void modem_response_sec(struct ipc_modem_data *data, struct ipc_message *resp)
{
	struct ipc_sec_pin_status_response_data *sim_status;
	unsigned char type;

	switch (resp->command) {
	case IPC_SEC_PIN_STATUS:
		sim_status =
			(struct ipc_sec_pin_status_response_data *)resp->data;

		switch (sim_status->status) {
		case IPC_SEC_PIN_STATUS_CARD_NOT_PRESENT:
			ipc_modem_log(data->client, MODEM_LOG_INFO,
				      "SIM card is definitely absent\n");
			break;
		case IPC_SEC_PIN_STATUS_LOCK_SC:
			switch (sim_status->facility_lock) {
			case IPC_SEC_FACILITY_LOCK_TYPE_SC_PIN1_REQ:
				ipc_modem_log(
					data->client,
					MODEM_LOG_INFO,
					"We need the PIN1 to unlock the card!"
					"\n");
				if (strlen(data->sim_pin) > 0) {
					modem_set_sec_pin_status(data->client,
								 data->sim_pin,
								 NULL);
				} else {
					ipc_modem_log(
						data->client,
						MODEM_LOG_ERROR,
						"No SIM Pin, use --pin\n");
				}
				break;
			case IPC_SEC_FACILITY_LOCK_TYPE_SC_PUK_REQ:
				ipc_modem_log(
					data->client,
					MODEM_LOG_INFO,
					"Please provide the SIM card PUK!"
					"\n");
				break;
			case IPC_SEC_FACILITY_LOCK_TYPE_SC_CARD_BLOCKED:
				ipc_modem_log(
					data->client,
					MODEM_LOG_INFO,
					"Ouch, the SIM Card is blocked.\n");
				break;
			}
			break;
		case IPC_SEC_PIN_STATUS_INIT_COMPLETE:
			ipc_modem_log(data->client, "3", "SIM init complete\n");
			if (state == MODEM_STATE_NORMAL)
				state = MODEM_STATE_SIM_OK;
			break;
		case IPC_SEC_PIN_STATUS_PB_INIT_COMPLETE:
			ipc_modem_log(data->client,
				      MODEM_LOG_INFO,
				      "SIM Phone Book init complete\n");
			break;
		}
		break;
	case IPC_SEC_SIM_ICC_TYPE:
		type = *((char *) resp->data);
		switch (type) {
		case IPC_SEC_SIM_CARD_TYPE_UNKNOWN:
			ipc_modem_log(
				data->client,
				MODEM_LOG_INFO,
				"No SIM card type: unknown (absent?)\n");
			break;
		case IPC_SEC_SIM_CARD_TYPE_SIM:
		case IPC_SEC_SIM_CARD_TYPE_USIM:
			ipc_modem_log(data->client,
				      MODEM_LOG_INFO, "SIM card found\n");
			break;
		}
		break;
	}
}

void modem_response_sms(struct ipc_client *client, struct ipc_message *resp)
{
	switch (resp->command) {
	case IPC_SMS_DEVICE_READY:
		if (state ==  MODEM_STATE_LPM) {
			ipc_modem_log(
				client,
				"4",
				"Modem is ready, requesting normal power mode"
				"\n");
			modem_exec_power_normal(client);
		} else if (state == MODEM_STATE_SIM_OK) {
			ipc_modem_log(client, "5", "Modem is fully ready\n");
			modem_set_sms_device_ready(client);
		}
		break;
	}
}

void modem_response_call(struct ipc_modem_data *data, struct ipc_message *resp)
{
	struct ipc_call_status_data *stat;

	switch (resp->command) {
	case IPC_CALL_LIST:
		/*
		 * if (data->in_call)
		 * modem_exec_call_answer(data->client);
		 * if (out_call)
		 * modem_snd_no_mic_mute(data->client);
		 */
		break;
	case IPC_CALL_INCOMING:
		ipc_modem_log(data->client,
			      MODEM_LOG_INFO, "Got an incoming call!\n");
		data->in_call = 1;
		modem_get_call_list(data->client);
		break;
	case IPC_CALL_STATUS:
		stat = (struct ipc_call_status_data *)resp->data;

		if (stat->status == IPC_CALL_STATUS_DIALING) {
			ipc_modem_log(
				data->client,
				MODEM_LOG_INFO,
				"Sending clock ctrl and restore alsa\n");
			modem_snd_clock_ctrl(data->client);
			/*
			 * system("alsa_ctl -f /data/alsa_state_modem restore");
			 */

			ipc_modem_log(data->client,
				      MODEM_LOG_INFO,
				      "CALL STATUS DIALING!!!\n");

			modem_snd_spkr_volume_ctrl(data->client);
			modem_snd_audio_path_ctrl(data->client);

			modem_get_call_list(data->client);
		}

		if (stat->status == IPC_CALL_STATUS_CONNECTED) {
			ipc_modem_log(data->client,
				      MODEM_LOG_INFO,
				      "CALL STATUS CONNECTED!!!\n");
			modem_snd_no_mic_mute(data->client);
		}

		if (stat->status == IPC_CALL_STATUS_RELEASED) {
			ipc_modem_log(data->client,
				      MODEM_LOG_INFO,
				      "CALL STATUS RELEASED!!!\n");
			modem_snd_no_mic_mute(data->client);
		}
		break;
	}
}

void modem_response_pwr(__attribute__((unused)) struct ipc_client *client,
			struct ipc_message *resp)
{
	int state_n;

	switch (resp->command) {
	case IPC_PWR_PHONE_PWR_UP:
		ipc_modem_log(client, "2", "Phone is powered up (LPM)!\n");
		state = MODEM_STATE_LPM;
		break;

	case IPC_PWR_PHONE_STATE:
		state_n = *((int *)resp->data);
#if 0
		switch (state_n) {
			/* FIXME: Broken */
		case IPC_PWR_PHONE_STATE_NORMAL:
			ipc_modem_log(client,
				      MODEM_LOG_INFO,
				      "Power state is now: NORMAL\n");
			break;
		case IPC_PWR_PHONE_STATE_LPM:
			ipc_modem_log(
				client,
				MODEM_LOG_INFO,
				"Power state is now: LPM (Low Power Mode)?\n");
			break;
		}
#endif
		state = state_n;
		break;

	}
}

void modem_response_net(struct ipc_modem_data *data,
			struct ipc_message *resp)
{
	struct ipc_net_regist_response_data *regi;
	char mnc[6];

	switch (resp->command) {
	case IPC_NET_REGIST:
		regi = (struct ipc_net_regist_response_data *) resp->data;
		if (regi->status == IPC_NET_REGISTRATION_STATUS_HOME)
			ipc_modem_log(
				data->client,
				MODEM_LOG_INFO,
				"Registered with network successfully!\n");
		break;
	case IPC_NET_SERVING_NETWORK:
		memcpy(mnc, (char *)((char *) resp->data + 3), 5);
		mnc[5] = 0;
		ipc_modem_log(data->client,
			      "6",
			      "Registered with network! "
			      "Got PLMN (Mobile Network Code): '%s'\n",
			      mnc);
		if (data->call_done == 0) {
			ipc_modem_log(data->client,
				      MODEM_LOG_INFO,
				      "Requesting outgoing call to %s!\n",
				      data->call_number);
			modem_exec_call_out(data->client, data->call_number);
		}
		data->call_done = 1;
		break;
	}
}

void modem_response_handle(struct ipc_modem_data *data,
			   struct ipc_message *resp)
{
	switch (IPC_GROUP(resp->command)) {
	case IPC_GROUP_NET:
		modem_response_net(data, resp);
		break;
	case IPC_GROUP_PWR:
		modem_response_pwr(data->client, resp);
		break;
	case IPC_GROUP_SEC:
		modem_response_sec(data, resp);
		break;
	case IPC_GROUP_SMS:
		modem_response_sms(data->client, resp);
		break;
	case IPC_GROUP_CALL:
		modem_response_call(data, resp);
		break;
	case IPC_GROUP_DISP:
		if (data->in_call)
			modem_snd_no_mic_mute(data->client);
		break;
	}
}


int modem_read_loop(struct ipc_modem_data *data)
{
	struct ipc_message resp;
	int rc;

	memset(&resp, 0, sizeof(resp));

	while (1) {
		usleep(3000);

		rc = ipc_client_poll(data->client, NULL, NULL);
		if (rc < 0)
			continue;

		rc = ipc_client_recv(data->client, &resp);
		if (rc < 0) {
			ipc_modem_log(
				data->client,
				MODEM_LOG_ERROR,
				"Can't RECV from modem: please run this again"
			       "\n");
			break;
		}

		modem_response_handle(data, &resp);

		if (resp.data != NULL)
			free(resp.data);
	}

	return 0;
}

int modem_dummy_read_loop(__attribute__((unused)) struct ipc_client *client)
{
	while (true) {
		ipc_modem_log(client,
			      MODEM_LOG_INFO, "%s: looping\n", __func__);
		sleep(1);
	}

	return 0;
}

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

int modem_start(struct ipc_client *client)
{
	int rc = -1;

	ipc_client_data_create(client);
	rc = ipc_client_boot(client);
	if (rc < 0)
		return -1;

	usleep(300);

	rc = ipc_client_open(client);
	if (rc < 0)
		return -1;

	rc = ipc_client_power_on(client);
	if (rc < 0)
		return -1;

	return 0;
}

int modem_stop(struct ipc_client *client)
{
	ipc_client_power_off(client);
	ipc_client_close(client);

	return 0;
}

void print_help(void)
{
	printf("usage: ipc-modem <command>\n"
	       "commands:\n"
	       "\tboot                          boot modem only\n"
	       "\tpower-on                      power on the modem only\n"
	       "\tpower-off                     power off the modem only\n"
	       "\tstart                         "
	       "boot modem and start read loop\n"
	       "arguments:\n"
	       "\t--call=[NUMBER]               call NUMBER\n"
	       "\t--debug                       enable debug messages\n"
	       "\t--dry-run                     "
	       "Test the ipc-modem program without talking to the modem.\n"
	       "\t--help                        print this help message\n"
	       "\t--log-target=[stdout|syslog]  "
	       "direct logs to stdout or syslog\n"
	       "\t--pin=[PIN]                   provide SIM card PIN\n");
}

int handle_command(struct ipc_modem_data *data)
{
	int rc = 0;

	switch (data->command) {
	case CMD_POWER_ON:
		if (data->dry_run)
			break;

		rc = ipc_client_power_on(data->client);
		if (rc < 0)
			ipc_modem_log(data->client, "[E]",
				      "[E] Something went wrong "
				      "while powering modem on\n");
		goto modem_quit;
	case CMD_POWER_OFF:
		if (data->dry_run)
			break;

		rc = ipc_client_power_off(data->client);
		if (rc < 0)
			ipc_modem_log(data->client,
				      MODEM_LOG_ERROR,
				      "Something went wrong "
				      "while powering modem off\n");
		goto modem_quit;
	case CMD_BOOT:
		if (data->dry_run)
			break;

		rc = ipc_client_boot(data->client);
		if (rc < 0)
			ipc_modem_log(data->client,
				      MODEM_LOG_ERROR,
				      "Something went wrong "
				      "while bootstrapping modem\n");
		break;
	case CMD_START:
		if (data->dry_run) {
			ipc_modem_log(
				data->client,
				"1",
				"Starting dummy modem_read_loop on %s client\n",
				"FMT");
			modem_dummy_read_loop(data->client);
			break;
		}

		ipc_modem_log(data->client,
			      "0", "Starting modem on FMT client\n");

		rc = modem_start(data->client);
		if (rc < 0) {
			ipc_modem_log(data->client,
				      MODEM_LOG_ERROR,
				      "Something went wrong\n");
			modem_stop(data->client);
			return 1;
		}
		ipc_modem_log(data->client,
			      "1",
			      "Starting modem_read_loop on FMT client\n");
		modem_read_loop(data);

		modem_stop(data->client);
		break;
	default:
		/* We should handle all commands */
		ipc_modem_log(data->client,
			      MODEM_LOG_ERROR,
			      "%s: Unknown command %d\n", __func__,
			      data->command);

		assert(false);
	}

modem_quit:
	if (data->client != 0)
		ipc_client_destroy(data->client);

	return rc;
}

void print_cmdline_opts(struct ipc_modem_data *data)

{
	if (data->debug)
		ipc_modem_log(data->client, MODEM_LOG_INFO, "Debug enabled\n");
	if (data->debug && data->dry_run)
		ipc_modem_log(data->client, MODEM_LOG_INFO, "dry-run mode\n");
}

int main(int argc, char *argv[])
{
	struct ipc_modem_data data;
	int c = 0;
	int opt_i = 0;

	struct option opt_l[] = {
		{"call",       required_argument,  0,  0 },
		{"debug",      no_argument,        0,  0 },
		{"dry-run",    no_argument,        0,  0 },
		{"help",       no_argument,        0,  0 },
		{"log-target", required_argument,  0,  0 },
		{"pin",        required_argument,  0,  0 },
		{0,            0,                  0,  0 }
	};

	bzero((void *)&data, sizeof(data));

	if (argc < 2) {
		print_help();
		exit(1);
	}

	while (true) {
		c = getopt_long(argc, argv, "", opt_l, &opt_i);
		if (c != 0)
			break;

		if (strcmp(opt_l[opt_i].name, "call") == 0) {
			if (optarg) {
				if (strlen(optarg) < 14) {
					assert(strlen(optarg) <
					       sizeof(data.call_number));
					printf("[I] Got call number!\n");
					strcpy(data.call_number, optarg);
				} else {
					printf("[E] "
					       "Call number is too long!\n");
					return 1;
				}
			}
		} else if ((strcmp(opt_l[opt_i].name, "log-target") == 0)) {
			if (optarg && !strcmp(optarg, "syslog")) {
				log_target = LOG_TO_SYSLOG;
			} else if (optarg && strcmp(optarg, "stdout")) {
				printf("[E] Invalid log target '%s'\n", optarg);
				return 1;
			}
		} else if (strcmp(opt_l[opt_i].name, "debug") == 0) {
			data.debug = true;
		} else if (strcmp(opt_l[opt_i].name, "dry-run") == 0) {
			data.dry_run = true;
		} else if (strncmp(opt_l[opt_i].name, "help", 4) == 0) {
			print_help();
			exit(1);
		} else if ((strcmp(opt_l[opt_i].name, "pin") == 0) &&
			   (optarg)) {
			if (strlen(optarg) < 8) {
				assert(strlen(optarg) <
				       sizeof(data.sim_pin));

				printf("[I] Got SIM PIN!\n");
				strcpy(data.sim_pin, optarg);
			} else {
				printf("[E] SIM PIN is too long!\n");
				return 1;
			}
		}
	}

	if (data.dry_run)
		data.client = ipc_client_create(IPC_CLIENT_TYPE_DUMMY);
	else
		data.client = ipc_client_create(IPC_CLIENT_TYPE_FMT);

	if (data.client == 0) {
		printf("[E] Could not create IPC client; aborting ...\n");
		return 1;
	}

	if (data.debug == 0)
		ipc_client_log_callback_register(data.client,
						 modem_log_handler_quiet,
						 NULL);
	else
		ipc_client_log_callback_register(data.client,
						 modem_log_handler,
						 NULL);

	while (optind < argc) {
		if (strncmp(argv[optind], "boot", 9) == 0) {
			data.command = CMD_BOOT;
			break;
		} else if (strncmp(argv[optind], "power-on", 8) == 0) {
			data.command = CMD_POWER_ON;
			break;
		} else if (strncmp(argv[optind], "power-off", 9) == 0) {
			data.command = CMD_POWER_OFF;
			break;
		} else if (strncmp(argv[optind], "start", 5) == 0) {
			data.command = CMD_START;
			break;
		} else {
			ipc_modem_log(data.client,
				      MODEM_LOG_ERROR,
				      "Unknown argument: '%s'\n",
				      argv[optind]);
			print_help();
			return 1;
		}

		optind++;
	}

	if (data.command == CMD_NONE) {
		printf("\n"
		       "Error: No command given. You need to use a command.\n"
		       "       See the help below for more details.\n"
		       "\n");
		print_help();
		return 1;
	}

	print_cmdline_opts(&data);

	return handle_command(&data);
}
