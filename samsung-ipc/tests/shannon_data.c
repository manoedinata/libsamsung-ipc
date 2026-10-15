/*
 * This file is part of libsamsung-ipc.
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

/*
 * Tests for the messages needed to get packet data going on Shannon modems.
 * The expected bytes are the ones that were sent to a Galaxy J4 modem when it
 * attached to LTE and returned an IP configuration, see include/gprs.h and
 * include/net.h for where the layouts come from.
 */

#include <stdio.h>
#include <string.h>

#include <samsung-ipc.h>

#include "shannon_data.h"

#define CHECK(condition)						\
	do {								\
		if (!(condition)) {					\
			printf("[ !! ] %s:%d: check failed: %s\n",	\
			       __FILE__, __LINE__, #condition);		\
			return -1;					\
		}							\
	} while (0)

int test_shannon_command_numbers(__attribute__((unused))
				 struct ipc_client *client)
{
	/* Numbers recovered from the stock RIL, the first three are also
	 * known from older modems and act as a sanity check of the table. */
	CHECK(IPC_GPRS_PS == 0x0D03);
	CHECK(IPC_GPRS_PDP_CONTEXT == 0x0D04);
	CHECK(IPC_NET_SERVICE_DOMAIN_CONFIG == 0x0808);

	CHECK(IPC_GPRS_LTE_ATTACH_APN_INFO == 0x0D14);
	CHECK(IPC_GPRS_SET_APN_INFO == 0x0D1B);
	CHECK(IPC_GPRS_MOBILE_DATA_STATUS == 0x0D1D);
	CHECK(IPC_GPRS_SYNC_PROFILE_INFO == 0x0D25);

	CHECK(!strcmp(ipc_command_string(IPC_GPRS_LTE_ATTACH_APN_INFO),
		      "IPC_GPRS_LTE_ATTACH_APN_INFO"));
	CHECK(!strcmp(ipc_command_string(IPC_GPRS_MOBILE_DATA_STATUS),
		      "IPC_GPRS_MOBILE_DATA_STATUS"));

	return 0;
}

int test_net_service_domain_config_setup(__attribute__((unused))
					 struct ipc_client *client)
{
	struct ipc_net_service_domain_config_data data;
	const unsigned char *bytes = (const unsigned char *) &data;

	CHECK(sizeof(data) == 1);
	CHECK(ipc_net_service_domain_config_setup(NULL, 1) < 0);

	/* What was sent to switch the modem from circuit switched only to
	 * circuit and packet switched. */
	CHECK(ipc_net_service_domain_config_setup(
		&data, IPC_NET_SERVICE_DOMAIN_CONFIG_CS_PS) == 0);
	CHECK(bytes[0] == 0x01);

	return 0;
}

int test_gprs_ps_and_mobile_data_setup(__attribute__((unused))
				       struct ipc_client *client)
{
	struct ipc_gprs_ps_request_set_data ps;
	struct ipc_gprs_mobile_data_status_data mobile_data;
	const unsigned char *bytes;

	CHECK(sizeof(ps) == 3);
	CHECK(ipc_gprs_ps_request_set_setup(NULL, IPC_GPRS_PS_ATTACH) < 0);
	CHECK(ipc_gprs_ps_request_set_setup(&ps, IPC_GPRS_PS_ATTACH) == 0);
	bytes = (const unsigned char *) &ps;
	CHECK(bytes[0] == 0x01 && bytes[1] == 0x00 && bytes[2] == 0x00);

	CHECK(ipc_gprs_ps_request_set_setup(&ps, IPC_GPRS_PS_DETACH) == 0);
	CHECK(bytes[0] == 0x00 && bytes[1] == 0x00 && bytes[2] == 0x00);

	CHECK(sizeof(mobile_data) == 2);
	CHECK(ipc_gprs_mobile_data_status_setup(NULL, 1, 0) < 0);
	CHECK(ipc_gprs_mobile_data_status_setup(&mobile_data, 1, 0) == 0);
	bytes = (const unsigned char *) &mobile_data;
	CHECK(bytes[0] == 0x01 && bytes[1] == 0x00);

	return 0;
}

int test_gprs_lte_attach_apn_info_setup(__attribute__((unused))
					struct ipc_client *client)
{
	struct ipc_gprs_lte_attach_apn_info_data data;
	const unsigned char *bytes = (const unsigned char *) &data;
	char too_long[103];
	size_t i;

	/* The whole message is 194 bytes, offsets from the stock RIL */
	CHECK(sizeof(data) == 194);
	CHECK(offsetof(struct ipc_gprs_lte_attach_apn_info_data, apn) == 3);
	CHECK(offsetof(struct ipc_gprs_lte_attach_apn_info_data, username) ==
	      127);
	CHECK(offsetof(struct ipc_gprs_lte_attach_apn_info_data, password) ==
	      159);
	CHECK(offsetof(struct ipc_gprs_lte_attach_apn_info_data, auth) == 191);
	CHECK(offsetof(struct ipc_gprs_lte_attach_apn_info_data,
		       protocol_roaming) == 192);

	CHECK(ipc_gprs_lte_attach_apn_info_setup(NULL, 1, "internet", NULL,
						 NULL) < 0);
	CHECK(ipc_gprs_lte_attach_apn_info_setup(&data, 1, NULL, NULL,
						 NULL) < 0);

	/* Exactly what was sent to the J4: profile 1, APN "internet", IPv4,
	 * no authentication, everything else zero. */
	CHECK(ipc_gprs_lte_attach_apn_info_setup(&data, 1, "internet", NULL,
						 NULL) == 0);
	CHECK(bytes[0] == 0x01);
	CHECK(bytes[1] == 0x01);
	CHECK(bytes[2] == 0x02);
	CHECK(!memcmp(&bytes[3], "internet", 9));	/* with its NUL */
	for (i = 12; i < 191; i++)
		CHECK(bytes[i] == 0x00);
	CHECK(bytes[191] == 0x00);
	CHECK(bytes[192] == 0x02);
	CHECK(bytes[193] == 0x00);

	/* Credentials end up in their fields and nowhere else */
	CHECK(ipc_gprs_lte_attach_apn_info_setup(&data, 2, "apn", "user",
						 "pass") == 0);
	CHECK(bytes[1] == 0x02);
	CHECK(!memcmp(&bytes[127], "user", 5));
	CHECK(!memcmp(&bytes[159], "pass", 5));
	CHECK(bytes[191] == 0x00);

	/* Names that do not fit are refused, like the stock RIL does */
	memset(too_long, 'a', sizeof(too_long) - 1);
	too_long[sizeof(too_long) - 1] = '\0';
	CHECK(ipc_gprs_lte_attach_apn_info_setup(&data, 1, too_long, NULL,
						 NULL) < 0);
	CHECK(ipc_gprs_lte_attach_apn_info_setup(&data, 1, "apn", too_long,
						 NULL) < 0);
	CHECK(ipc_gprs_lte_attach_apn_info_setup(&data, 1, "apn", NULL,
						 too_long) < 0);

	return 0;
}
