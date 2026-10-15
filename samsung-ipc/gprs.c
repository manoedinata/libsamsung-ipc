/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2014 Paul Kocialkowski <contact@paulk.fr>
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

#include <stdlib.h>
#include <string.h>

#include <samsung-ipc.h>

/*
 * The modem expects exactly these message sizes (taken from the stock RIL),
 * make the build fail if a structure ever stops matching them.
 */
typedef char ipc_gprs_ps_request_set_data_size_check[
	(sizeof(struct ipc_gprs_ps_request_set_data) == 3) ? 1 : -1];
typedef char ipc_gprs_mobile_data_status_data_size_check[
	(sizeof(struct ipc_gprs_mobile_data_status_data) == 2) ? 1 : -1];
typedef char ipc_gprs_lte_attach_apn_info_data_size_check[
	(sizeof(struct ipc_gprs_lte_attach_apn_info_data) == 194) ? 1 : -1];

int ipc_gprs_define_pdp_context_setup(
	struct ipc_gprs_define_pdp_context_data *data, unsigned char enable,
	unsigned char cid, const char *apn)
{
	if (data == NULL || apn == NULL)
		return -1;

	memset(data, 0, sizeof(struct ipc_gprs_define_pdp_context_data));
	data->enable = enable;
	data->cid = cid;
	data->magic = 0x02;

	strncpy((char *) data->apn, apn, sizeof(data->apn));

	return 0;
}

int ipc_gprs_pdp_context_request_set_setup(
	struct ipc_gprs_pdp_context_request_set_data *data,
	unsigned char enable, unsigned char cid, const char *username,
	const char *password)
{
	if (data == NULL)
		return -1;

	memset(data, 0, sizeof(struct ipc_gprs_pdp_context_request_set_data));
	data->enable = enable;
	data->cid = cid;

	if (enable && username != NULL && password != NULL) {
		data->magic1[2] = 0x13;
		data->magic2 = 0x01;

		strncpy((char *) data->username, username,
			sizeof(data->username));
		strncpy((char *) data->password, password,
			sizeof(data->password));
	}

	return 0;
}

int ipc_gprs_port_list_setup(struct ipc_gprs_port_list_data *data)
{
	/* FIXME: These are only known-to-work values used on most devices */
	unsigned char magic[] = { 0x02, 0x04, 0x16, 0x00, 0x17, 0x00, 0x87,
				  0x00, 0xBD, 0x01 };

	if (data == NULL)
		return -1;

	memset(data, 0, sizeof(struct ipc_gprs_port_list_data));
	memcpy(data->magic, magic, sizeof(magic));

	return 0;
}

int ipc_gprs_ps_request_set_setup(struct ipc_gprs_ps_request_set_data *data,
				  unsigned char attach)
{
	if (data == NULL)
		return -1;

	memset(data, 0, sizeof(struct ipc_gprs_ps_request_set_data));
	data->attach = attach;

	return 0;
}

int ipc_gprs_mobile_data_status_setup(
	struct ipc_gprs_mobile_data_status_data *data, unsigned char enabled,
	unsigned char roaming_enabled)
{
	if (data == NULL)
		return -1;

	memset(data, 0, sizeof(struct ipc_gprs_mobile_data_status_data));
	data->enabled = enabled;
	data->roaming_enabled = roaming_enabled;

	return 0;
}

int ipc_gprs_lte_attach_apn_info_setup(
	struct ipc_gprs_lte_attach_apn_info_data *data,
	unsigned char profile_id, const char *apn, const char *username,
	const char *password)
{
	if (data == NULL || apn == NULL)
		return -1;

	/* The stock RIL refuses names that do not fit in the fields */
	if (strlen(apn) > sizeof(data->apn))
		return -1;
	if (username != NULL && strlen(username) > sizeof(data->username))
		return -1;
	if (password != NULL && strlen(password) > sizeof(data->password))
		return -1;

	memset(data, 0, sizeof(struct ipc_gprs_lte_attach_apn_info_data));
	data->type = 1;
	data->profile_id = profile_id;
	data->protocol = IPC_GPRS_PROTOCOL_IPV4;
	data->protocol_roaming = IPC_GPRS_PROTOCOL_IPV4;

	strncpy((char *) data->apn, apn, sizeof(data->apn));

	if (username != NULL)
		strncpy((char *) data->username, username,
			sizeof(data->username));
	if (password != NULL)
		strncpy((char *) data->password, password,
			sizeof(data->password));

	return 0;
}
