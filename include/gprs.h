/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2011-2014 Paul Kocialkowski <contact@paulk.fr>
 *
 * Based on Tizen libslp-tapi:
 * Copyright (C) 2011 Samsung Electronics Co., Ltd.
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

#include <samsung-ipc.h>

#ifndef __SAMSUNG_IPC_GPRS_H__
#define __SAMSUNG_IPC_GPRS_H__

/*
 * Commands
 */

#define IPC_GPRS_DEFINE_PDP_CONTEXT				0x0D01
#define IPC_GPRS_QOS						0x0D02
#define IPC_GPRS_PS						0x0D03
#define IPC_GPRS_PDP_CONTEXT					0x0D04
#define IPC_GPRS_ENTER_DATA					0x0D05
#define IPC_GPRS_SHOW_PDP_ADDR					0x0D06
#define IPC_GPRS_MS_CLASS					0x0D07
#define IPC_GPRS_3G_QUAL_SRVC_PROFILE				0x0D08
#define IPC_GPRS_IP_CONFIGURATION				0x0D09
#define IPC_GPRS_DEFINE_SEC_PDP_CONTEXT			0x0D0A
#define IPC_GPRS_TFT						0x0D0B
#define IPC_GPRS_HSDPA_STATUS					0x0D0C
#define IPC_GPRS_CURRENT_SESSION_DATA_COUNTER			0x0D0D
#define IPC_GPRS_DATA_DORMANT					0x0D0E
#define IPC_GPRS_PIN_CTRL					0x0D0F
#define IPC_GPRS_CALL_STATUS					0x0D10
#define IPC_GPRS_PORT_LIST					0x0D11

/*
 * Commands only known from Shannon modems (LTE capable, IPC protocol 4.1).
 *
 * The numbers and names were recovered from the stock RIL (libsec-ril.so):
 * the function that turns a command number into a name is a jump table
 * indexed by the sub command, and the table agrees with every number above
 * and in net.h. Only the commands that have a structure further down have
 * had their payload decoded, the others are listed so that they can be named
 * in logs. Numbers 0x1E to 0x20 are not named by the RIL.
 */
#define IPC_GPRS_LTE_QOS_PROFILE				0x0D12
#define IPC_GPRS_NW_INITIATED_PDN_DISCONNECT			0x0D13
#define IPC_GPRS_LTE_ATTACH_APN_INFO				0x0D14
#define IPC_GPRS_EPDG_HANDOVER					0x0D15
#define IPC_GPRS_EPDG_STATUS					0x0D16
#define IPC_GPRS_FD_INFORMATION				0x0D17
#define IPC_GPRS_OPERATOR_RESERVED_PCO				0x0D18
#define IPC_GPRS_LTE_CA_STATUS					0x0D19
#define IPC_GPRS_BACKOFF_TIMER					0x0D1A
#define IPC_GPRS_SET_APN_INFO					0x0D1B
#define IPC_GPRS_TRAFFIC_CHANNEL_STATUS			0x0D1C
#define IPC_GPRS_MOBILE_DATA_STATUS				0x0D1D
#define IPC_GPRS_IMS_TEST_MODE					0x0D21
#define IPC_GPRS_SET_ALWAYS_ON_PDN				0x0D22
#define IPC_GPRS_LCE_INFO					0x0D23
#define IPC_GPRS_LTE_ADVANCED_ICON				0x0D24
#define IPC_GPRS_SYNC_PROFILE_INFO				0x0D25

/*
 * Values
 */

#define IPC_GPRS_FAIL_CAUSE_NONE				0x0000
#define IPC_GPRS_FAIL_CAUSE_REL_BY_USER			0x0001
#define IPC_GPRS_FAIL_CAUSE_REGULAR_DEACTIVATION		0x0002
#define IPC_GPRS_FAIL_CAUSE_LLC_SNDCP				0x0003
#define IPC_GPRS_FAIL_CAUSE_INSUFFICIENT_RESOURCE		0x0004
#define IPC_GPRS_FAIL_CAUSE_UNKNOWN_APN			0x0005
#define IPC_GPRS_FAIL_CAUSE_UNKNOWN_PDP_ADDRESS		0x0006
#define IPC_GPRS_FAIL_CAUSE_USER_AUTH_FAILED			0x0007
#define IPC_GPRS_FAIL_CAUSE_ACT_REJ_GGSN			0x0008
#define IPC_GPRS_FAIL_CAUSE_ACT_REJ_UNSPECIFIED		0x0009
#define IPC_GPRS_FAIL_CAUSE_SVC_OPTION_NOT_SUPPORTED		0x000A
#define IPC_GPRS_FAIL_CAUSE_SVC_NOT_SUBSCRIBED			0x000B
#define IPC_GPRS_FAIL_CAUSE_SVC_OPT_OUT_ORDER			0x000C
#define IPC_GPRS_FAIL_CAUSE_NSAPI_USED				0x000D
#define IPC_GPRS_FAIL_CAUSE_QOS_NOT_ACCEPTED			0x000E
#define IPC_GPRS_FAIL_CAUSE_NETWORK_FAILURE			0x000F
#define IPC_GPRS_FAIL_CAUSE_REACT_REQUIRED			0x0010
#define IPC_GPRS_FAIL_CAUSE_FEATURE_NOT_SUPPORTED		0x0011
#define IPC_GPRS_FAIL_CAUSE_TFT_FILTER_ERROR			0x0012
#define IPC_GPRS_FAIL_CAUSE_UNKNOWN_PDP_CONTEXT		0x0013
#define IPC_GPRS_FAIL_CAUSE_INVALID_MSG			0x0014
#define IPC_GPRS_FAIL_CAUSE_PROTOCOL_ERROR			0x0015
#define IPC_GPRS_FAIL_CAUSE_MOBILE_FAILURE_ERROR		0x0016
#define IPC_GPRS_FAIL_CAUSE_TIMEOUT_ERROR			0x0017
#define IPC_GPRS_FAIL_CAUSE_UNKNOWN_ERROR			0x0018

#define IPC_GPRS_HSDPA_STATUS_NONE				0x00
#define IPC_GPRS_HSDPA_STATUS_HSDPA				0x01
#define IPC_GPRS_HSDPA_STATUS_HSPAP				0x02

#define IPC_GPRS_STATUS_NOT_ENABLED				0x00
#define IPC_GPRS_STATUS_ENABLED				0x01
#define IPC_GPRS_STATUS_DISABLED				0x03

/*
 * Protocol byte used by Shannon modems in the PDP context related commands
 * (see ipc_gprs_define_pdp_context_data's magic and the attach profile
 * below). 0x02 is IPv4 only. The RIL also produces 0x04, 0x05 and 0x06 for
 * its other protocol types and samsungipcd uses 0x06 to ask for IPv6
 * support, but their exact meaning has not been verified.
 */
#define IPC_GPRS_PROTOCOL_IPV4					0x02

/* Value of the first byte of IPC_GPRS_PS set requests. */
#define IPC_GPRS_PS_ATTACH					0x01
#define IPC_GPRS_PS_DETACH					0x00

/*
 * Structures
 */

struct ipc_gprs_define_pdp_context_data {
	unsigned char enable;
	unsigned char cid;
	unsigned char magic;
	unsigned char apn[124];
} __attribute__((__packed__));

struct ipc_gprs_ps_data {
	unsigned char cid;
	unsigned char attached;
} __attribute__((__packed__));

struct ipc_gprs_pdp_context_request_set_data {
	unsigned char enable;
	unsigned char cid;
	unsigned char magic1[4];
	unsigned char username[32];
	unsigned char password[32];
	unsigned char unknown[32];
	unsigned char magic2;
} __attribute__((__packed__));

struct ipc_gprs_pdp_context_request_get_entry {
	unsigned char cid;
	unsigned char active;
} __attribute__((__packed__));

struct ipc_gprs_pdp_context_request_get_data {
	unsigned char active_count;
	struct ipc_gprs_pdp_context_request_get_entry entries[3];
} __attribute__((__packed__));

struct ipc_gprs_ip_configuration_data {
	unsigned char cid;
	unsigned char field_flag;
	unsigned char fail_cause;	/*IPC_GPRS_FAIL_CAUSE */
	unsigned char ip[4];
	unsigned char dns1[4];
	unsigned char dns2[4];
	unsigned char gateway[4];
	unsigned char subnet_mask[4];
	unsigned char unknown[4];
} __attribute__((__packed__));

struct ipc_gprs_hsdpa_status_data {
	unsigned char status;		/* IPC_GPRS_HSDPA_STATUS */
} __attribute__((__packed__));

struct ipc_gprs_current_session_data_counter_data {
	unsigned char cid;
	unsigned int rx_count;
	unsigned int tx_count;
} __attribute__((__packed__));

struct ipc_gprs_call_status_data {
	unsigned char cid;
	unsigned char status;		/* IPC_GPRS_STATUS */
	unsigned char fail_cause;	/* IPC_GPRS_FAIL_CAUSE */
	unsigned char external;
} __attribute__((__packed__));

struct ipc_gprs_port_list_data {
	unsigned char magic[804];
} __attribute__((__packed__));

/*
 * Set request of IPC_GPRS_PS (Shannon modems). This is not the layout of the
 * reply to a get request, see struct ipc_gprs_ps_data for that one. When
 * detaching, the stock RIL also fills the two other bytes; when attaching it
 * sends 01 00 00. Layout taken from IpcProtocol41::IpcTxPsAttach().
 */
struct ipc_gprs_ps_request_set_data {
	unsigned char attach;		/* IPC_GPRS_PS_ATTACH or _DETACH */
	unsigned char detach_flag;
	unsigned char detach_reason;	/* 0 to 4 */
} __attribute__((__packed__));

/*
 * Payload of IPC_GPRS_MOBILE_DATA_STATUS (set), layout taken from
 * IpcProtocol41::IpcTxSetMobileDataSetting(bool, bool). The name of the
 * second field is inferred from how the RIL uses it.
 */
struct ipc_gprs_mobile_data_status_data {
	unsigned char enabled;
	unsigned char roaming_enabled;
} __attribute__((__packed__));

/*
 * Payload of IPC_GPRS_LTE_ATTACH_APN_INFO (set): the "initial attach" profile
 * that the modem uses when it attaches to LTE. Layout (194 bytes) taken from
 * IpcProtocol41::IpcTxSetLteAttachProfile().
 *
 * On a Galaxy J4 (Telkomsel, LTE) this message is required for data: without
 * it a PDP context activation is acknowledged but never completes and no
 * IPC_GPRS_IP_CONFIGURATION follows. The content of the APN field was not
 * checked by that network (an invalid name worked), other networks may well
 * validate it.
 *
 * The fields marked "unknown" are copied by the RIL from optional arguments
 * (an IPv6 identifier, an IPv4 address, P-CSCF related flags) that the
 * plain data setup does not use; they are left zero by the helper below.
 */
struct ipc_gprs_lte_attach_apn_info_data {
	unsigned char type;		/* 1, the RIL uses 3 in one case */
	unsigned char profile_id;
	unsigned char protocol;		/* IPC_GPRS_PROTOCOL_* */
	unsigned char apn[101];
	unsigned char unknown1[16];
	unsigned char unknown2[4];
	unsigned char unknown3[2];
	unsigned char unknown4;
	unsigned char username[32];
	unsigned char password[32];
	unsigned char auth;		/* 0 means no authentication */
	unsigned char protocol_roaming;	/* IPC_GPRS_PROTOCOL_* */
	unsigned char unknown5;
} __attribute__((__packed__));

/*
 * Helpers
 */

int ipc_gprs_define_pdp_context_setup(
	struct ipc_gprs_define_pdp_context_data *data, unsigned char enable,
	unsigned char cid, const char *apn);
int ipc_gprs_pdp_context_request_set_setup(
	struct ipc_gprs_pdp_context_request_set_data *data,
	unsigned char enable, unsigned char cid, const char *username,
	const char *password);
int ipc_gprs_port_list_setup(struct ipc_gprs_port_list_data *data);
int ipc_gprs_ps_request_set_setup(struct ipc_gprs_ps_request_set_data *data,
				  unsigned char attach);
int ipc_gprs_mobile_data_status_setup(
	struct ipc_gprs_mobile_data_status_data *data, unsigned char enabled,
	unsigned char roaming_enabled);
int ipc_gprs_lte_attach_apn_info_setup(
	struct ipc_gprs_lte_attach_apn_info_data *data,
	unsigned char profile_id, const char *apn, const char *username,
	const char *password);

#endif /* __SAMSUNG_IPC_GPRS_H__ */
