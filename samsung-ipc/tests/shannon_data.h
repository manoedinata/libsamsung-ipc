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

#ifndef __TESTS_SHANNON_DATA_H__
#define __TESTS_SHANNON_DATA_H__

#include <stddef.h>

int test_shannon_command_numbers(struct ipc_client *client);
int test_net_service_domain_config_setup(struct ipc_client *client);
int test_gprs_ps_and_mobile_data_setup(struct ipc_client *client);
int test_gprs_lte_attach_apn_info_setup(struct ipc_client *client);

#endif /* __TESTS_SHANNON_DATA_H__ */
